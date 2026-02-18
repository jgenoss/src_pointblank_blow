#include "stdafx.h"
#include "UIFileCrawlerImpl_V1_Gui.h"

namespace rsc
{
	UIFileCrawlerImpl_V1_Gui::UIFileCrawlerImpl_V1_Gui(ContentsDataPtr contentsData,
													   const Path& workdir,
													   const Path& folderRelPath,
													   const std::string& strCountryName)
		: UIFileCrawlerImpl(contentsData, workdir, folderRelPath, strCountryName)
	{}

	UIFileCrawlerImpl_V1_Gui::~UIFileCrawlerImpl_V1_Gui()
	{}

	bool UIFileCrawlerImpl_V1_Gui::Load()
	{
		return true;
	}

	void UIFileCrawlerImpl_V1_Gui::GatherSceneFiles(std::vector<PathPtr>& out)
	{}

	void UIFileCrawlerImpl_V1_Gui::GatherSceneAssocFiles(std::vector<PathPtr>& out)
	{}

	void UIFileCrawlerImpl_V1_Gui::GatherMiscs(std::vector<PathPtr>& out)
	{}
}