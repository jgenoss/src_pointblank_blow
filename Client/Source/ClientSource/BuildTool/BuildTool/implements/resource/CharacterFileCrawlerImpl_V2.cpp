#include "stdafx.h"
#include "CharacterFileCrawlerImpl_V2.h"
#include "RscTool/i3RscPartialFile.h"
#include "RscTool/RSCFileMgr.h"
#include "PathHelper.h"
#include <array>
#include <tuple>
#include "i3sCrawler.h"
#include "FolderFilesCrawler.h"
#include "i3chrCrawler.h"
#include "EquipmentFileCrawlerImpl_V2_Avatar.h"
#include "FileCrawlerFactory.h"
#include "../util/CustomUtil.h"

namespace rsc
{
	CharacterFileCrawlerImpl_V2::CharacterFileCrawlerImpl_V2(ContentsDataPtr contentsData,
															 const Path& workdir,
															 const Path& charaPefRelPath,
															 const Path& equipPefRelPath,
															 const std::string& strCountryName)
		: CharacterFileCrawlerImpl(contentsData, workdir, charaPefRelPath, strCountryName),
		m_equipPefRelPath(equipPefRelPath)
	{
		m_avatarPartsInfoList.reserve(9);
	}

	CharacterFileCrawlerImpl_V2::~CharacterFileCrawlerImpl_V2()
	{}

	bool CharacterFileCrawlerImpl_V2::LoadPef()
	{
		if (m_contentsData.get() == nullptr)
			return false;

		if (!OpenPef())
			return false;

		const int idxNation = IPefable::GetPefNationIdx();
		if (idxNation == -1)
			return false;

		// 입력된 컨텐츠의 PEF Key.
		i3ResourceFileObjectInfo2* registryKey = GetPef()->Findi3RegKeyRoot();
		if (registryKey == nullptr)
			return false;

		// Character, Dino.
		registryKey = GetPef()->FindChildi3TreeElement(registryKey, m_contentsData->strSubcategoryName.c_str());
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, m_contentsData->strContentsName.c_str());
		if (registryKey == nullptr)
			return false;

		m_contentsRegistryKey = registryKey;

		return true;
	}

	void CharacterFileCrawlerImpl_V2::GatherPefFiles(std::vector<PathPtr>& out)
	{
		CharacterFileCrawlerImpl::GatherPefFiles(out);

		// Equipment.pef 파일. 캐릭터에 연관된 기본 장비들이 있기 때문.
		Path equipPefRelPath(m_equipPefRelPath);

		out.push_back(std::make_shared<Path>(equipPefRelPath));
		out.push_back(std::make_shared<Path>(equipPefRelPath.replace_extension(".i3RegXML")));
	}

	void CharacterFileCrawlerImpl_V2::GatherMeshAssocFiles(std::vector<PathPtr>& out)
	{
		i3::rc_wstring wstrValue;
		i3ResourceFileObjectInfo2* registryKey = nullptr;

		const int idxNation = IPefable::GetPefNationIdx();

		registryKey = GetPef()->Findi3RegData(m_contentsRegistryKey, "ResPostName");
		if (registryKey)
		{
			// 기본 마스크. Facegear 아니고 얼굴이다.
			if (GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue) && !wstrValue.empty())
			{
				i3::rc_string strResPostName;
				i3::utf16_to_mb(wstrValue, strResPostName);

				Path avatarMaskRelPath("Avatar/Mask");	
				avatarMaskRelPath /= strResPostName.c_str();

				FolderFilesCrawler folderCwl;	
				folderCwl.SetWorkdir(GetWorkdir());
				folderCwl.SetRelativePath(avatarMaskRelPath);
				folderCwl.operator()(out);

				i3sCrawler i3sCwl;	
				i3sCwl.SetWorkdir(GetWorkdir());

				std::vector<PathPtr> i3sRelPathList;
				
				std::copy_if(out.begin(), out.end(), std::back_inserter(i3sRelPathList),
					[](const PathPtr& fileRelPathPtr) 
					{ 
						return i3::generic_is_iequal(fileRelPathPtr->extension().string(), ".i3s"); 
					}
				);

				RemoveDuplicates(i3sRelPathList);

				for (auto &i3sRelPath : i3sRelPathList)
				{
					// 메시 공유 파일일 경우 : 원본 i3s 파일 + texture_change_Avatar.pef 파일에 적혀있는 i3i 파일들.
					// 메시 공유 파일이 아닐 경우 : i3sRelativePath 경로의 i3s 파일 + i3s 연관 파일.
					const bool meshSharingOK =
						GatherEquipMeshSharingAssocFiles(GetWorkdir(), *(i3sRelPath.get()), "config/texture_change_Avatar.pef", out);

					if (!meshSharingOK)
					{
						dynamic_cast<IFilePathable*>(&i3sCwl)->SetRelativePath(*(i3sRelPath.get()));
						i3sCwl.operator()(out);
					}
				}
			}
		}

		// 원본 캐릭터 아이템 아이디 조사.
		INT32 representItemID = 0;

		registryKey = GetPef()->Findi3RegData(m_contentsRegistryKey, "RepresentID");
		if (registryKey)
		{
			GetPef()->Readi3RegIntArrayElement(registryKey, idxNation, representItemID);
		}

		// 아바타 파츠 리스트 아이템 아이디 저장.
		INT32 avatarPartsItemID = 0;

		std::array<std::pair<const char*, const char*>, 9> arrPartsListDataInfo =
		{
			std::make_pair("ResDftPartBelt",	"Belt"),
			std::make_pair("ResDftPartBeret",	"Beret"),	// V3에 포함될 예정이라고 함.
			std::make_pair("ResDftPartFace",	"Facegear"),
			std::make_pair("ResDftPartGlove",	"Glove"),
			std::make_pair("ResDftPartHead",	"Head"),
			std::make_pair("ResDftPartHolster",	"Holster"),
			std::make_pair("ResDftPartJacket",	"Upper"),
			std::make_pair("ResDftPartPoket",	"Lower"),
			std::make_pair("ResDftPartSkin",	"Skin")
		};

		i3ResourceFileObjectInfo2* partsListRegKey =
			GetPef()->FindChildi3TreeElement(m_contentsRegistryKey, "PartsList");

		if (partsListRegKey)
		{
			for (auto &partsDataInfo : arrPartsListDataInfo)
			{
				const std::string& strDataName = std::get<0>(partsDataInfo);
				const std::string& strEquipRegistryKeyName = std::get<1>(partsDataInfo);

				registryKey = GetPef()->Findi3RegData(partsListRegKey, strDataName.c_str());
				if (registryKey == nullptr)
					continue;

				if (!GetPef()->Readi3RegIntArrayElement(registryKey, idxNation, avatarPartsItemID))
					continue;

				if (avatarPartsItemID != 0)
					m_avatarPartsInfoList.push_back(std::make_unique<AvatarPartsInfo>(avatarPartsItemID, strEquipRegistryKeyName));
			}
		}

		// 원본 캐릭터 조사.
		// V2는 남자 캐릭터 SWAT_Male.I3CHR, 여자 캐릭터 SWAT_Female.I3CHR 두 개만 사용 중.
		// RepresentID 키값으로 원본 캐릭터의 아이템 아이디를 구한다.
		i3ResourceFileObjectInfo2* registryRootKey = GetPef()->Findi3RegKeyRoot();
		if (registryRootKey == nullptr)
			return;

		std::array<const char*, 2> arrCharacterTypeRegKeyName =
		{
			"Character", "Dino"
		};

		//i3chrCrawler i3chrCwl;	
		//i3chrCwl.SetWorkdir(GetWorkdir());
		i3sCrawler i3sCwl;		
		i3sCwl.SetWorkdir(GetWorkdir());

		Path characteri3sRelPath;

		for (auto &charaTypeRegKeyName : arrCharacterTypeRegKeyName)
		{
			registryKey = GetPef()->FindChildi3TreeElement(registryRootKey, charaTypeRegKeyName);
			if (registryKey == nullptr)
				continue;

			i3::vector<i3ResourceFileObjectInfo2*> characterRegistryKeyList;
			GetPef()->ReadChildListi3TreeElement(characterRegistryKeyList);

			i3::rc_string characterName;
			INT32 itemID = 0;
			i3::rc_string strCharacteri3chrRelPath;

			for (auto &characterRegistryKey : characterRegistryKeyList)
			{
				GetPef()->ReadNamei3NamedElement(characterRegistryKey, characterName);

				registryKey = GetPef()->Findi3RegData(characterRegistryKey, "ITEMID");
				if (registryKey == nullptr)
					continue;

				if (!GetPef()->Readi3RegIntArrayElement(registryKey, idxNation, itemID))
					continue;

				if (representItemID == itemID)
				{
					// I3CHR 검색.
					registryKey = GetPef()->Findi3RegData(characterRegistryKey, "_ResPath");
					if (registryKey == nullptr)
						continue;

					if (!GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue) && wstrValue.empty())
						continue;

					i3::utf16_to_mb(wstrValue, strCharacteri3chrRelPath);
					out.push_back(std::make_shared<Path>(strCharacteri3chrRelPath.c_str()));

					// 캐릭터 i3chr 의 내부 연관 파일 갯수가 3000 개쯤 된다. 
					// 전부 무기 관련 애니메이션 파일이고 이 파일들은 무기 추가 작업과 연관되어 있으므로
					// 여기서는 i3chr 내부 파일 검색 필요없다.
					//i3chrCwl.SetRelativePath(stri3chrRelPath.c_str());
					//i3chrCwl.operator()(outFileRelPathList);

					// i3s는 캐릭터 공유 메시이기 때문에 반드시 추가되어야 한다.
					characteri3sRelPath = strCharacteri3chrRelPath.c_str();	characteri3sRelPath.replace_extension(".i3s");
					i3sCwl.SetRelativePath(characteri3sRelPath);
					i3sCwl.operator()(out);

					out.push_back(std::make_shared<Path>(characteri3sRelPath));

					break;
				}
			}
		}
	}

	void CharacterFileCrawlerImpl_V2::GatherEquipmentContentsDataList(std::vector<ContentsDataPtr>& out)
	{
		const int idxNation = GetPefNationIdx();

		i3RscPartialFile pef;

		Path equipPefAbsPath = GetWorkdir() / m_equipPefRelPath;
		if (!pef.Open(equipPefAbsPath.string().c_str()))
			return;

		i3ResourceFileObjectInfo2* registryKey = nullptr;
		for (auto& avatarPartsInfo : m_avatarPartsInfoList)
		{
			registryKey = pef.Findi3RegKeyRoot();
			if (registryKey == nullptr)
				continue;

			registryKey = pef.FindChildi3TreeElement(registryKey, "Equipment");
			if (registryKey == nullptr)
				continue;

			registryKey = pef.FindChildi3TreeElement(registryKey, avatarPartsInfo->strEquipRegKeyName.c_str());
			if (registryKey == nullptr)
				continue;

			i3::rc_string partsRegistryKeyName;
			INT32 itemID = 0;

			i3::vector<i3ResourceFileObjectInfo2*> partsRegistryKeyList;
			pef.ReadChildListi3TreeElement(partsRegistryKeyList);

			for (auto &partsRegistryKey : partsRegistryKeyList)
			{
				pef.ReadNamei3NamedElement(partsRegistryKey, partsRegistryKeyName);

				registryKey = pef.Findi3RegData(partsRegistryKey, "ITEMID");
				if (registryKey == nullptr)
					continue;

				if (pef.Readi3RegIntArrayElement(registryKey, idxNation, itemID) && (itemID > 0))
				{
					if (itemID != avatarPartsInfo->itemID)
						continue;

					auto contentsData = std::make_shared<ContentsData>();
					contentsData->strContentsName = partsRegistryKeyName.c_str();
					contentsData->strCategoryName = "Equipment";
					contentsData->strSubcategoryName = avatarPartsInfo->strEquipRegKeyName.c_str();

					out.push_back(contentsData);
				}
			}
		}
	}

	void CharacterFileCrawlerImpl_V2::GatherEquipmentAssocFiles(std::vector<PathPtr>& out)
	{
		std::vector<ContentsDataPtr> equipContentsDataList;
		GatherEquipmentContentsDataList(equipContentsDataList);

		for (auto &currContentsData : equipContentsDataList)
		{
			auto equipCrawler = FileCrawlerFactory::i()->Create(
				std::make_pair("v2", "Equipment"), currContentsData, GetWorkdir(), GetCountryName());

			equipCrawler->operator()(out);
		}
	}

	void CharacterFileCrawlerImpl_V2::GatherRSCAssocFiles(std::vector<PathPtr>& out)
	{
		out.push_back(std::make_shared<Path>("Chara/Character.RSC"));
		out.push_back(std::make_shared<Path>("Chara/Character.RSG"));
		out.push_back(std::make_shared<Path>("Chara/CharaTexture.RSC"));
		out.push_back(std::make_shared<Path>("Chara/CharaTexture.RSG"));
	}

	void CharacterFileCrawlerImpl_V2::GatherMiscs(std::vector<PathPtr>& out)
	{
	}
}