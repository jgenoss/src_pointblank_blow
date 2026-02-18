#include "stdafx.h"
#include "UIContentsCrawler_V3.h"
#include "ResourceController.h"
#include "ContentsCrawlerCallback.h"
#include "FolderFilesCrawler.h"
#include <boost/filesystem.hpp>

namespace rsc
{
	UIContentsCrawler_V3::UIContentsCrawler_V3(const Path& workdir, const std::vector<Path>& UIFolderRelPathList) 
		: m_workdir(workdir), m_UIFolderRelPathList(std::move(UIFolderRelPathList))
	{}

	UIContentsCrawler_V3::~UIContentsCrawler_V3()
	{}

	bool UIContentsCrawler_V3::__Load()
	{
		std::vector<Path>::iterator it = m_UIFolderRelPathList.begin();
		for (; it != m_UIFolderRelPathList.end();)
		{
			const Path& currUIFolderRelPath = *it;
			if (!fs::exists(m_workdir / currUIFolderRelPath))
				it = m_UIFolderRelPathList.erase(it);
			else
				++it;
		}

		return !m_UIFolderRelPathList.empty();
	}

	bool UIContentsCrawler_V3::__Gather()
	{
		// UI_V10, UI_V11, UI_V15 각 폴더별로 동일한 파일명을 가지는 i3UIs 파일명을 하나의 컨텐츠로 가정하고 긁어모은다.

		using i3UIsNameList = std::vector<std::string>;
		using i3UIsNameListPtr = std::shared_ptr<i3UIsNameList>;
		std::map<std::string, i3UIsNameListPtr> i3UIsNameListBySubCategory;

		for (auto &currUIFolderRelPath : m_UIFolderRelPathList)
		{
			// UIRe/Scene 하위 폴더들 경로만 긁어모은다.
			FolderFilesCrawler folderCwl;
			folderCwl.SetWorkdir(m_workdir);
			folderCwl.SetRelativePath(currUIFolderRelPath/"Scene");
			folderCwl.SetIncludeSubFolder(true);
			folderCwl.SetRecursive(false);	// 현재 폴더만.

			std::vector<PathPtr> subFoldersOfUIReScene;
			folderCwl.operator()(subFoldersOfUIReScene);

			// i3UIs 파일들을 대상으로 설정.
			folderCwl.SetIncludeSubFolder(false);
			folderCwl.SetRecursive(true);
			folderCwl.SetFileSearchKeys({ ".i3UIs" });

			std::vector<PathPtr> i3UIsRelPathList;

			for (auto &subFolder : subFoldersOfUIReScene)
			{
				if (subFolder->has_extension())
					continue;

				folderCwl.SetRelativePath(*(subFolder.get()));

				i3UIsRelPathList.clear();
				folderCwl.operator()(i3UIsRelPathList);

				if (i3UIsRelPathList.empty())
					continue;

				const std::string& strSubCategoryName = subFolder->filename().string();	// "Clan", "HUD", "Main" 등 폴더명들.
				auto it = i3UIsNameListBySubCategory.find(strSubCategoryName);
				if (it == i3UIsNameListBySubCategory.end())
				{
					auto pr = i3UIsNameListBySubCategory.insert(
						std::map<std::string, i3UIsNameListPtr>::value_type(strSubCategoryName, std::make_unique<i3UIsNameList>()));

					if (pr.second == false)
						continue;

					it = pr.first;
				}

				for (auto &i3UIsRelPath : i3UIsRelPathList)
				{
					i3UIsRelPath->replace_extension("");	// 확장자를 없애버려서 순수 파일명만 사용하도록.
					const std::string stri3UIsName = i3UIsRelPath->filename().string();
					(it->second)->push_back(stri3UIsName);
				}
			}
		}

		i3UIsNameListPtr i3UIsList;
		std::string strSubCategoryName;
		for (auto &it : i3UIsNameListBySubCategory)
		{
			strSubCategoryName = it.first;
			i3UIsList = it.second;

			std::sort(i3UIsList->begin(), i3UIsList->end());
			i3UIsList->erase(std::unique(i3UIsList->begin(), i3UIsList->end()), i3UIsList->end());

			for (auto &i3UIsName : *i3UIsList)
			{
				if (GetCallback())
					GetCallback()->OnGather(IContentsCrawler::GetCountryName(), i3UIsName, "UI", strSubCategoryName);
			}
		}

		return true;
	}

	bool UIContentsCrawler_V3::__PreGather()
	{
		if (GetCallback())
			GetCallback()->OnPreGather(IContentsCrawler::GetCountryName(), "UI");

		return true;
	}

	bool UIContentsCrawler_V3::__PostGather()
	{
		if (GetCallback())
			GetCallback()->OnPostGather(IContentsCrawler::GetCountryName(), "UI");

		return true;
	}
}