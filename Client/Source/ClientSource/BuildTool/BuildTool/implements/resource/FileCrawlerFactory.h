#pragma once

#include "util/ObjectFactory.h"
#include "IFileCrawler.h"
#include "FileCrawlerNull.h"
#include "ContentsDefines.h"

namespace rsc
{
	class FileCrawlerFactory final: 

		public Factory<IFileCrawler, 
					   std::pair<const std::string, const std::string>, 
					   FileCrawlerNull, 
					   IFileCrawler* (*)(ContentsDataPtr, const Path&, const std::string&)>,

		public i3::shared_ginst<FileCrawlerFactory>
	{
	public:
		FileCrawlerFactory();
		virtual ~FileCrawlerFactory();
	};

	class CompatiblePackScriptsCrawlerFactory final :

		public Factory<IFileCrawler,
					   std::string,
					   FileCrawlerNull>,

		public i3::shared_ginst<CompatiblePackScriptsCrawlerFactory>
	{
	public:
		CompatiblePackScriptsCrawlerFactory();
		virtual ~CompatiblePackScriptsCrawlerFactory();
	};
}