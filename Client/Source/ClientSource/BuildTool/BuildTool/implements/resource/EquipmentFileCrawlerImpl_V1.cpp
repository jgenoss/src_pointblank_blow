#include "stdafx.h"
#include "EquipmentFileCrawlerImpl_V1.h"
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
#include <boost/format.hpp>
#include "Settings.h"

namespace rsc
{
	EquipmentFileCrawlerImpl_V1::EquipmentFileCrawlerImpl_V1(ContentsDataPtr contentsData,
															 const Path& workdir,
															 const Path& PEFRelPath,
															 const std::string& strCountryName)
		: EquipmentFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName)
	{}

	EquipmentFileCrawlerImpl_V1::~EquipmentFileCrawlerImpl_V1()
	{}

	bool EquipmentFileCrawlerImpl_V1::LoadPef()
	{
		return EquipmentFileCrawlerImpl::LoadPef();
	}

	void EquipmentFileCrawlerImpl_V1::GatherPefFiles(std::vector<PathPtr>& out)
	{
		EquipmentFileCrawlerImpl::GatherPefFiles(out);
	}

	void EquipmentFileCrawlerImpl_V1::GatherMeshAssocFiles(std::vector<PathPtr>& out)
	{
		i3::rc_wstring wstrValue;
		i3ResourceFileObjectInfo2* registryKey = nullptr;

		const int idxNation = IPefable::GetPefNationIdx();

		std::vector<PathPtr> i3chrRelPathPtrList;

		// 메시 공유 검색을 반드시 해야하는 케이스. 마스크일 경우에만.
		registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "ResPostName_i3s");
		if (registryKey && GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue) && !wstrValue.empty())
		{
			// 캐릭터 이름을 구한다. texture_Change_Equip_i3s.pef 의 "Equip/Mask" 하위 키값들을 활용한다. 
			i3RscPartialFile texChangePef;
			Path texChangePefAbsPath = GetWorkdir() / "config/texture_Change_Equip_i3s.pef";

			std::vector<i3::rc_string> strCharaNameList;
			Path i3sRelPath;
			i3::rc_string stri3sRelPath, strCharaName;

			if (texChangePef.Open(texChangePefAbsPath.string().c_str()))
			{
				registryKey = texChangePef.Findi3RegKeyRoot();
				if (registryKey)
				{
					registryKey = texChangePef.FindChildi3TreeElement(registryKey, "Equip/Mask");
					if (registryKey)
					{
						i3::vector<i3ResourceFileObjectInfo2*> equipMaskRegKeyList;
						texChangePef.ReadChildListi3TreeElement(equipMaskRegKeyList);

						i3::rc_string strCharaName;
						for (auto &currRegKey : equipMaskRegKeyList)
						{
							if (currRegKey == nullptr)
								continue;

							texChangePef.ReadNamei3NamedElement(currRegKey, stri3sRelPath);

							i3sRelPath = stri3sRelPath.c_str();
							strCharaName = i3sRelPath.filename().string().c_str();

							auto pos = strCharaName.find("_Mask_");
							if (pos == std::string::npos)
								continue;

							strCharaName.erase(pos);

							strCharaNameList.push_back(strCharaName.c_str());
						}

						strCharaNameList.erase(std::unique(strCharaNameList.begin(), strCharaNameList.end()), strCharaNameList.end());	// 중복 제거.
					}
				}
			}

			//  위에서 구한 캐릭터 이름을 활용하여 메시 공유 텍스처 검색.
			i3::rc_string strTexChangei3sName;

			i3::rc_string strResPostName_i3s;	i3::utf16_to_mb(wstrValue, strResPostName_i3s);

			std::vector<PathPtr> texChangeAssocRelPathPtrList;
			std::vector<PathPtr> i3chrRelPathPtrList;
			PathPtr i3chrRelPathPtr;
			for (auto &currCharaName : strCharaNameList)
			{
				strTexChangei3sName = currCharaName;
				strTexChangei3sName += "_Mask_";
				strTexChangei3sName += strResPostName_i3s;
				strTexChangei3sName += ".i3s";

				if (!GatherEquipMeshSharingAssocFiles(GetWorkdir(), strTexChangei3sName.c_str(), "config/texture_Change_Equip_i3s.pef", texChangeAssocRelPathPtrList))
					continue;
				
				// 
				for (auto &currRelPathPtr : texChangeAssocRelPathPtrList)
				{
					if (i3::generic_is_iequal(currRelPathPtr->extension().string(), ".i3s"))
					{
						i3chrRelPathPtr = std::make_shared<Path>(*currRelPathPtr);
						i3chrRelPathPtr->replace_extension(".i3CHR");
						i3chrRelPathPtrList.push_back(i3chrRelPathPtr);
					}
				}
				
			}

			std::copy(texChangeAssocRelPathPtrList.begin(), texChangeAssocRelPathPtrList.end(), std::back_inserter(out));
			std::copy(i3chrRelPathPtrList.begin(), i3chrRelPathPtrList.end(), std::back_inserter(out));
		}

		// "ResLocalPath" 폴더 내 파일들 중 이름이 "ResPostName" 을 포함하는 파일을 긁어모은다.
		registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "ResLocalPath");
		if (registryKey && GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue) && !wstrValue.empty())
		{
			i3::rc_string strResourceLocalPath;
			i3::utf16_to_mb(wstrValue, strResourceLocalPath);

			registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "ResPostName");
			if (registryKey && GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue) && !wstrValue.empty())
			{
				i3::rc_string stri3chrPostName;
				i3::utf16_to_mb(wstrValue, stri3chrPostName);	stri3chrPostName += ".i3CHR";

				// 규칙에 따라 폴더 내 모든 파일들을 긁어 모은다.
				i3::rc_string strFileRelPath("Equip/");		strFileRelPath += strResourceLocalPath;

				FolderFilesCrawler folderCwl;
				dynamic_cast<IFilePathable*>(&folderCwl)->SetWorkdir(m_workdir);
				dynamic_cast<IFilePathable*>(&folderCwl)->SetRelativePath(strFileRelPath.c_str());
				folderCwl.SetRecursive(false);
				folderCwl.SetFileSearchKeys( { stri3chrPostName.c_str() } );
				folderCwl.operator()(i3chrRelPathPtrList);

				std::copy(i3chrRelPathPtrList.begin(), i3chrRelPathPtrList.end(), std::back_inserter(out));

				// i3chr 파일들에 한하여 재검색.
				i3chrCrawler i3chrCwl;
				i3chrCwl.SetWorkdir(m_workdir);
				for (auto &i3chrRelPathPtr : i3chrRelPathPtrList)
				{
					i3chrCwl.SetRelativePath(*(i3chrRelPathPtr.get()));
					i3chrCwl.operator()(out);
				}

				// i3s 메시 공유 검색.
				std::vector<PathPtr> i3sRelPathList;

				std::copy_if(out.begin(), out.end(), std::back_inserter(i3sRelPathList),
					[](const PathPtr& src) 
					{ 
						return i3::generic_is_iequal(src->extension().string(), ".i3s"); 
					}
				);

				for (auto &i3sRelPath : i3sRelPathList)
					GatherEquipMeshSharingAssocFiles(GetWorkdir(), *(i3sRelPath.get()), "config/texture_Change_Equip_i3s.pef", out);
			}
		}
	}

	void EquipmentFileCrawlerImpl_V1::GatherTextures(std::vector<PathPtr>& out)
	{
		i3::rc_wstring wstrValue;
		const int idxNation = IPefable::GetPefNationIdx();

		i3ResourceFileObjectInfo2* registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "EmblemTexture");
		if (registryKey)
		{
			if (GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue) && !wstrValue.empty())
			{
				Path texRelPath(wstrValue.c_str());

				std::string strEmblemTexCommonName(texRelPath.filename().string());
				std::string strEmblemTexParentRelpath(texRelPath.parent_path().string());

				Path emblemTexRelpath;
				std::string strEmblemTexName;

				std::array<const char*, 8> arrEmblemTexPostfix= 
				{
					".i3i",
					"_NM.i3i",
					"_RM.i3i",
					"_SP.i3i",
					"_Diff.i3i",
					"_Norm.i3i",
					"_Ref.i3i",
					"_Spec.i3i"
				};

				for (auto &strPostfix : arrEmblemTexPostfix)
				{
					strEmblemTexName = strEmblemTexCommonName;		
					strEmblemTexName += strPostfix;
					
					emblemTexRelpath = strEmblemTexParentRelpath;	
					emblemTexRelpath /= strEmblemTexName.c_str();

					out.push_back(std::make_shared<Path>(emblemTexRelpath));
				}
			}
		}
		
		std::array<const char*, 3> arrTexRegistryDataName = 
		{
			"DiffuseTexture",
			"SpecularTexture",
			"NormalTexture"
		};

		for (auto &texRegistryDataName : arrTexRegistryDataName)
		{
			registryKey = GetPef()->Findi3RegData(m_currRegistryKey, texRegistryDataName);	// i3i 파일의 상대경로가 입력되어 있음.
			if (registryKey == nullptr)
				continue;

			if (!GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue))
				continue;

			if (wstrValue.empty())
				continue;

			i3::rc_string strTexRelPath;
			i3::utf16_to_mb(wstrValue, strTexRelPath);

			out.push_back(std::make_shared<Path>(strTexRelPath.c_str()));
		}

		// 상점 UI 텍스처.
		registryKey = GetPef()->Findi3RegData(m_currRegistryKey, "UIShapeIndex");
		if (registryKey)
		{
			INT32 shapeIdx = 0;
			if (GetPef()->Readi3RegIntArrayElement(registryKey, idxNation, shapeIdx))
			{
				const int shapeCapacity_i3i = 18;

				const int idx_i3i = (shapeIdx / shapeCapacity_i3i) + 1;	// item_Mask i3i는 1부터 시작.

				const Path& localePath = CountrySettings::i()->FindData(GetCountryName())->GetMediaLocaleFolderRelPath();
				std::string strLocalei3iRelPath = boost::io::str(boost::format("%s/Gui/Item/item_Mask_%d.i3i") % localePath % idx_i3i);

				out.push_back(std::make_shared<Path>(strLocalei3iRelPath));
			}
		}

	}

	void EquipmentFileCrawlerImpl_V1::GatherMiscs(std::vector<PathPtr>& out)
	{
		out.push_back(std::make_shared<Path>("Equip/Equip.RSC"));
		out.push_back(std::make_shared<Path>("Equip/Equip.RSG"));
		out.push_back(std::make_shared<Path>("Equip/EquipTexture.RSC"));
		out.push_back(std::make_shared<Path>("Equip/EquipTexture.RSG"));
	}
}