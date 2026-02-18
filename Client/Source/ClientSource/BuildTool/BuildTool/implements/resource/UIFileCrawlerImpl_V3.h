#pragma once

#include "UIFileCrawlerImpl.h"

namespace rsc
{
	class UIFileCrawlerImpl_V2_UIRe;

	class UIFileCrawlerImpl_V3 : public UIFileCrawlerImpl
	{
	public:
		UIFileCrawlerImpl_V3(ContentsDataPtr contentsData, const Path& workdir, const std::string& strCountryName);
		virtual ~UIFileCrawlerImpl_V3();

		virtual bool Load() override;
		virtual void GatherSceneFiles(std::vector<PathPtr>& out) override;
		virtual void GatherSceneAssocFiles(std::vector<PathPtr>& out) override;

	private:
		using V2UICrawlerImplPtr = std::unique_ptr<UIFileCrawlerImpl_V2_UIRe>;
		using V2UICrawlerImplPtrList = std::vector<V2UICrawlerImplPtr>;
		V2UICrawlerImplPtrList m_V2ImplList;
	};
}
