#include "stdafx.h"
#include "UIContentsCrawler_V2_UIRe.h"
#include "ResourceController.h"
#include "ContentsCrawlerCallback.h"
#include "FolderFilesCrawler.h"
#include <boost/filesystem.hpp>

namespace rsc
{
	UIContentsCrawler_V2_UIRe::UIContentsCrawler_V2_UIRe(const Path& workdir, const Path& UIReFolderTgtRelPath)
		: IFilePathable(workdir, UIReFolderTgtRelPath)
	{}

	UIContentsCrawler_V2_UIRe::~UIContentsCrawler_V2_UIRe()
	{}

	bool UIContentsCrawler_V2_UIRe::__Load()
	{
		return fs::exists(IFilePathable::GetAbsolutePath());
	}

	bool UIContentsCrawler_V2_UIRe::__Gather()
	{
		// UIRe/Scene 하위 폴더들 경로만 긁어모은다.
		FolderFilesCrawler folderCwl;
		folderCwl.SetWorkdir(GetWorkdir());
		folderCwl.SetRelativePath(GetRelativePath() / "Scene");
		folderCwl.SetIncludeSubFolder(true);
		folderCwl.SetRecursive(false);	// 현재 폴더만.

		std::vector<PathPtr> subFoldersOfUIReScene;
		folderCwl.operator()(subFoldersOfUIReScene);

		// i3UIs 파일들을 대상으로 설정.
		folderCwl.SetIncludeSubFolder(false);
		folderCwl.SetRecursive(true);
		folderCwl.SetFileSearchKeys( {".i3UIs"} );

		std::vector<PathPtr> i3UIsRelPathList;

		for (auto &subFolder : subFoldersOfUIReScene)
		{
			if (subFolder->has_extension())
				continue;

			folderCwl.SetRelativePath(*(subFolder.get()));

			i3UIsRelPathList.clear();
			folderCwl.operator()(i3UIsRelPathList);

			const std::string& strSubCategoryName = subFolder->filename().string();

			for (auto &i3UIsRelPath : i3UIsRelPathList)
			{
				i3UIsRelPath->replace_extension("");	// 확장자를 없애버려서 순수 파일명만 사용하도록.
				const std::string& stri3UIsName = i3UIsRelPath->filename().string();

				if (GetCallback())
					GetCallback()->OnGather(IContentsCrawler::GetCountryName(), stri3UIsName, "UI", strSubCategoryName);
			}
		}

		return true;
	}

	bool UIContentsCrawler_V2_UIRe::__PreGather()
	{
		if (GetCallback())
			GetCallback()->OnPreGather(IContentsCrawler::GetCountryName(), "UI");

		return true;
	}

	bool UIContentsCrawler_V2_UIRe::__PostGather()
	{
		if (GetCallback())
			GetCallback()->OnPostGather(IContentsCrawler::GetCountryName(), "UI");

		return true;
	}
}