#include "stdafx.h"
#include "CharacterFileCrawlerImpl_V1.h"
#include "RscTool/i3RscPartialFile.h"
#include "RscTool/RSCFileMgr.h"
#include "PathHelper.h"
#include <array>
#include <tuple>
#include "i3chrCrawler.h"
#include "FolderFilesCrawler.h"
#include <boost/filesystem.hpp>
#include "i3sCrawler.h"
#include <algorithm>
#include "EquipmentFileCrawlerImpl_V1.h"
#include "FileCrawlerFactory.h"

namespace rsc
{
	CharacterFileCrawlerImpl_V1::CharacterFileCrawlerImpl_V1(ContentsDataPtr contentsData,
															 const Path& workdir,
															 const Path& charaPefRelPath,
															 const Path& equipPefRelPath,
															 const std::string& strCountryName)
		: CharacterFileCrawlerImpl(contentsData, workdir, charaPefRelPath, strCountryName), m_equipPefRelPath(equipPefRelPath)
	{}

	CharacterFileCrawlerImpl_V1::~CharacterFileCrawlerImpl_V1()
	{}

	bool CharacterFileCrawlerImpl_V1::LoadPef()
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

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, "Character");
		if (registryKey == nullptr)
			return false;

		registryKey = GetPef()->FindChildi3TreeElement(registryKey, m_contentsData->strContentsName.c_str());
		if (registryKey == nullptr)
			return false;

		m_contentsRegistryKey = registryKey;

		return true;
	}

	void CharacterFileCrawlerImpl_V1::GatherPefFiles(std::vector<PathPtr>& out)
	{
		CharacterFileCrawlerImpl::GatherPefFiles(out);
	}

	void CharacterFileCrawlerImpl_V1::GatherMeshAssocFiles(std::vector<PathPtr>& out)
	{
		i3::rc_wstring wstrValue;
		i3ResourceFileObjectInfo2* registryKey = nullptr;

		const int idxNation = IPefable::GetPefNationIdx();

		registryKey = GetPef()->Findi3RegData(m_contentsRegistryKey, "_ResPath");
		if (registryKey)
		{
			if (GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue) && !wstrValue.empty())
			{
				i3::rc_string strCharacteri3chrRelPath;	i3::utf16_to_mb(wstrValue, strCharacteri3chrRelPath);

				out.push_back(std::make_shared<Path>(strCharacteri3chrRelPath.c_str()));

				Path characteri3sRelPath(strCharacteri3chrRelPath.c_str());
				characteri3sRelPath.replace_extension(".i3s");

				i3sCrawler i3sCwl;		
				i3sCwl.SetWorkdir(GetWorkdir());
				i3sCwl.SetRelativePath(characteri3sRelPath);
				i3sCwl.operator()(out);

				out.push_back(std::make_shared<Path>(characteri3sRelPath));
			}
		}
	}

	void CharacterFileCrawlerImpl_V1::GatherEquipmentAssocFiles(std::vector<PathPtr>& out)
	{
		// 우선 모든 캐릭터에 관련된 Equip 파일을 긁어모은다.
		std::vector<ContentsDataPtr> equipContentsDataList;
		GatherCharaAssocEquipContentsList(equipContentsDataList);

		std::vector<PathPtr> equipAssocFileRelPathList;
		for (auto &currContentsData : equipContentsDataList)
		{
			auto equipCrawler = FileCrawlerFactory::i()->Create(
				std::make_pair("v1", "Equipment"), currContentsData, GetWorkdir(), GetCountryName());

			equipCrawler->operator()(equipAssocFileRelPathList);
		}

		// 특정 캐릭터 관련 Equip 파일을 선별 추출.
		const std::string& strCharaName = m_contentsData->strContentsName;

		i3chrCrawler i3chrCwl;	
		i3chrCwl.SetWorkdir(GetWorkdir());
		
		for (auto &currFileRelPath : equipAssocFileRelPathList)
		{
			const std::string strCurrExt = currFileRelPath->extension().string();
			if (i3::generic_is_iequal(strCurrExt, ".I3CHR") )
			{
				const std::string strCurrPath = currFileRelPath->string();
				if (strCurrPath.find(strCharaName) != std::string::npos)
				{
					// i3chr, i3s, 이미지 파일 등 기타 관련 파일 추출.
					out.push_back(currFileRelPath);

					i3chrCwl.SetRelativePath(*currFileRelPath);
					i3chrCwl.operator()(out);
				}
			}
		}
	}

	void CharacterFileCrawlerImpl_V1::GatherRSCAssocFiles(std::vector<PathPtr>& out)
	{
		out.push_back(std::make_shared<Path>("Chara/Character.RSC"));
		out.push_back(std::make_shared<Path>("Chara/Character.RSG"));
		out.push_back(std::make_shared<Path>("Chara/CharaTexture.RSC"));
		out.push_back(std::make_shared<Path>("Chara/CharaTexture.RSG"));
	}

	void CharacterFileCrawlerImpl_V1::GatherMiscs(std::vector<PathPtr>& out)
	{
	}

	void CharacterFileCrawlerImpl_V1::GatherCharaAssocEquipContentsList(std::vector<ContentsDataPtr>& out)
	{
		const int idxNation = GetPefNationIdx();

		i3RscPartialFile pef;

		Path equipPefAbsPath = GetWorkdir() / m_equipPefRelPath;
		if (!pef.Open(equipPefAbsPath.string().c_str()))
			return;

		i3ResourceFileObjectInfo2* registryKey = pef.Findi3RegKeyRoot();
		if (registryKey == nullptr)
			return;

		registryKey = pef.FindChildi3TreeElement(registryKey, "Equipment");
		if (registryKey == nullptr)
			return;

		registryKey = pef.FindChildi3TreeElement(registryKey, "Head");
		if (registryKey == nullptr)
			return;

		i3::vector<i3ResourceFileObjectInfo2*> helmetRegistryKeyList;
		pef.ReadChildListi3TreeElement(helmetRegistryKeyList);

		i3::rc_string strHelmetName;
		for (auto &helmetRegistryKey : helmetRegistryKeyList)
		{
			pef.ReadNamei3NamedElement(helmetRegistryKey, strHelmetName);

			auto contentsData = std::make_shared<ContentsData>();
			contentsData->strContentsName = strHelmetName.c_str();
			contentsData->strCategoryName = "Equipment";
			contentsData->strSubcategoryName = "Head";

			out.push_back(contentsData);
		}
	}
}