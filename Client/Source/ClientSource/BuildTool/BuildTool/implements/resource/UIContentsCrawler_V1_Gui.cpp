#include "stdafx.h"
#include "UIContentsCrawler_V1_Gui.h"
#include "ResourceController.h"
#include "ContentsCrawlerCallback.h"
#include "FolderFilesCrawler.h"
#include <boost/filesystem.hpp>

namespace rsc
{
	UIContentsCrawler_V1_Gui::UIContentsCrawler_V1_Gui(const Path& workdir, const Path& GUIFolderTgtRelPath)
		: IFilePathable(workdir, GUIFolderTgtRelPath)
	{}

	UIContentsCrawler_V1_Gui::~UIContentsCrawler_V1_Gui()
	{}

	bool UIContentsCrawler_V1_Gui::__Load()
	{
		return fs::exists(IFilePathable::GetAbsolutePath());
	}

	bool UIContentsCrawler_V1_Gui::__Gather()
	{

		return true;
	}

	bool UIContentsCrawler_V1_Gui::__PreGather()
	{

		return true;
	}

	bool UIContentsCrawler_V1_Gui::__PostGather()
	{

		return true;
	}
}