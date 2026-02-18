#pragma once

#include "util/ObjectFactory.h"

namespace rsc
{
	class IContentsCrawler;

	// 牧刨明 农费矾 积己.
	struct ContentsCrawlers
	{
		std::vector<std::shared_ptr<IContentsCrawler>> list;
	};

	class ContentsCrawlerFactory final :

		public Factory<ContentsCrawlers,
					   std::string,
					   ContentsCrawlers,
					   ContentsCrawlers* (*)(const Path&, const std::string&)>,

		public i3::shared_ginst<ContentsCrawlerFactory>
	{
	public:
		ContentsCrawlerFactory();
		virtual ~ContentsCrawlerFactory();
	};
}