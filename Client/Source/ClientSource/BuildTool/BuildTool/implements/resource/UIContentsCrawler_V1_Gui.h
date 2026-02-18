#pragma once

#include "IContentsCrawler.h"
#include "IFilePathable.h"

namespace rsc
{
	class UIContentsCrawler_V1_Gui : public IContentsCrawler, public IFilePathable
	{
	public:
		UIContentsCrawler_V1_Gui(const Path& workdir, const Path& GUIFolderTgtRelPath);
		virtual ~UIContentsCrawler_V1_Gui();

	private:
		virtual bool __Load() override;
		virtual bool __PreGather() override;
		virtual bool __Gather() override;
		virtual bool __PostGather() override;
	};
}