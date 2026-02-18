#include "stdafx.h"
#include "UIFileCrawlerImpl_V2_UIRe.h"
#include <boost/filesystem.hpp>
#include <array>
#include "i3UIeCrawler.h"

namespace rsc
{
	UIFileCrawlerImpl_V2_UIRe::UIFileCrawlerImpl_V2_UIRe(ContentsDataPtr contentsData,
														 const Path& workdir,
														 const Path& folderRelPath,
														 const std::string& strCountryName)
		: UIFileCrawlerImpl(contentsData, workdir, folderRelPath, strCountryName)
	{}

	UIFileCrawlerImpl_V2_UIRe::~UIFileCrawlerImpl_V2_UIRe()
	{}

	bool UIFileCrawlerImpl_V2_UIRe::Load()
	{
		Path UISceneRelPath(IFilePathable::GetRelativePath());
		UISceneRelPath /= "Scene";

		Path i3UIeRelPath(UISceneRelPath.string());
		i3UIeRelPath /= m_contentsData->strSubcategoryName;
		i3UIeRelPath /= m_contentsData->strContentsName;
		i3UIeRelPath += ".i3UIe";

		// 기본적으로 i3UIs 파일 바탕으로 관련 리소스를 긁어 모은다.
		if (!fs::exists(GetWorkdir()/i3UIeRelPath))
			return false;

		m_i3UIeRelPath = i3UIeRelPath;
		return true;
	}

	void UIFileCrawlerImpl_V2_UIRe::GatherSceneFiles(std::vector<PathPtr>& out)
	{
		// scene 파일.
		out.push_back(std::make_shared<Path>(m_i3UIeRelPath));

		Path i3UIsRelPath(m_i3UIeRelPath);
		i3UIsRelPath.replace_extension(".i3UIs");
		out.push_back(std::make_shared<Path>(i3UIsRelPath));

		// 관련 파일들.
		std::vector<Path> i3UIeAssocRelPathList;

		i3UIeCrawler i3UIeCwl(GetWorkdir(), m_i3UIeRelPath);
		i3UIeCwl.operator()(out);
	}

	void UIFileCrawlerImpl_V2_UIRe::GatherSceneAssocFiles(std::vector<PathPtr>& out)
	{}

	void UIFileCrawlerImpl_V2_UIRe::GatherMiscs(std::vector<PathPtr>& out)
	{}
}