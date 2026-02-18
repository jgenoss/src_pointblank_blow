#include "stdafx.h"
#include "EquipmentFileCrawlerImpl_V2_Avatar.h"
#include "RscTool/i3RscPartialFile.h"
#include "RscTool/RSCFileMgr.h"
#include "PathHelper.h"
#include <array>
#include <tuple>
#include "i3sCrawler.h"
#include "FolderFilesCrawler.h"
#include "../util/CustomUtil.h"

namespace rsc
{
	EquipmentFileCrawlerImpl_V2_Avatar::EquipmentFileCrawlerImpl_V2_Avatar(ContentsDataPtr contentsData, 
																		   const Path& workdir, 
																		   const Path& PEFRelPath, 
																		   const std::string& strCountryName)
		: EquipmentFileCrawlerImpl(contentsData, workdir, PEFRelPath, strCountryName)
	{
	}

	EquipmentFileCrawlerImpl_V2_Avatar::~EquipmentFileCrawlerImpl_V2_Avatar()
	{
	}

	bool EquipmentFileCrawlerImpl_V2_Avatar::LoadPef()
	{
		return EquipmentFileCrawlerImpl::LoadPef();
	}

	void EquipmentFileCrawlerImpl_V2_Avatar::GatherPefFiles(std::vector<PathPtr>& out)
	{
		EquipmentFileCrawlerImpl::GatherPefFiles(out);
	}

	void EquipmentFileCrawlerImpl_V2_Avatar::GatherMeshAssocFiles(std::vector<PathPtr>& out)
	{
		i3::rc_wstring wstrValue;
		i3ResourceFileObjectInfo2* registryKey = nullptr;

		const int idxNation = IPefable::GetPefNationIdx();

		FolderFilesCrawler folderCwl;	
		folderCwl.SetWorkdir(GetWorkdir());
		
		i3sCrawler i3sCwl;				
		i3sCwl.SetWorkdir(GetWorkdir());

		std::array<const char*, 3> arrResRegistryDataName =
		{
			"ResPostName",
			"ResName_Blue",
			"ResName_Red"
		};

		const std::string strSubCategoryFolderName = 
			i3::generic_is_iequal(m_contentsData->strSubcategoryName, "Head") ? "Headgear" : m_contentsData->strSubcategoryName;
		
		for (auto &resRegistryDataName : arrResRegistryDataName)
		{
			registryKey = GetPef()->Findi3RegData(m_currRegistryKey, resRegistryDataName);
			if (registryKey == nullptr)
				continue;

			if (!GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue))
				continue;
			
			i3::rc_string strResourceName;
			i3::utf16_to_mb(wstrValue, strResourceName);

			Path resourceFolderRelPath("Avatar");
			resourceFolderRelPath /= strSubCategoryFolderName;
			resourceFolderRelPath /= strResourceName.c_str();	// 폴더명.

			// 폴더로 검색.
			folderCwl.SetRelativePath(resourceFolderRelPath);
			folderCwl.operator()(out);

			// i3s 파일이 없이 텍스처만 공유되어 있을 경우.
			std::string stri3sName(strResourceName.c_str());	
			stri3sName += ".i3S";
			
			GatherEquipMeshSharingAssocFiles(GetWorkdir(), stri3sName, "config/texture_change_Avatar.pef", out);
		}

		RemoveDuplicates(out);

		std::vector<PathPtr> i3sRelPathList;

		std::copy_if(out.begin(), out.end(), std::back_inserter(i3sRelPathList),
			[](const PathPtr& fileRelPath) 
			{ 
				return i3::generic_is_iequal(fileRelPath->extension().string(), ".i3s");
			}
		);

		for (auto &i3sRelPath : i3sRelPathList)
		{
			// 메시 공유 파일일 경우 : 원본 i3s 파일 + texture_change_Avatar.pef 파일에 적혀있는 i3i 파일들.
			// 메시 공유 파일이 아닐 경우 : i3sRelativePath 경로의 i3s 파일 + i3s 연관 파일.
			const bool meshSharingOK =
				GatherEquipMeshSharingAssocFiles(GetWorkdir(), i3sRelPath->string(), "config/texture_change_Avatar.pef", out);

			if (!meshSharingOK)
			{
				dynamic_cast<IFilePathable*>(&i3sCwl)->SetRelativePath(*(i3sRelPath.get()));
				i3sCwl.operator()(out);
			}
		}
	}

	void EquipmentFileCrawlerImpl_V2_Avatar::GatherTextures(std::vector<PathPtr>& out)
	{
		i3::rc_wstring wstrValue;
		const int idxNation = IPefable::GetPefNationIdx();

		std::array<const char*, 1> arrTexRegistryDataName =
		{
			"DiffuseTexture"
		};

		i3ResourceFileObjectInfo2* registryKey = nullptr;

		for (auto &texRegistryDataName : arrTexRegistryDataName)
		{
			registryKey = GetPef()->Findi3RegData(m_currRegistryKey, texRegistryDataName);	// i3i 파일의 상대경로가 입력되어 있음.
			if (registryKey == nullptr)
				continue;

			if (!GetPef()->Readi3RegStringArrayElement(registryKey, idxNation, wstrValue))
				continue;

			if (wstrValue.empty())
				continue;

			i3::rc_string stri3iRelPath;	
			i3::utf16_to_mb(wstrValue, stri3iRelPath);

			out.push_back(std::make_shared<Path>(stri3iRelPath.c_str()));
		}
	}

	void EquipmentFileCrawlerImpl_V2_Avatar::GatherMiscs(std::vector<PathPtr>& out)
	{
	}
}