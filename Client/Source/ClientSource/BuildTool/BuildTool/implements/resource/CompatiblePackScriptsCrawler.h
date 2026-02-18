#pragma once

#include "IFileCrawler.h"
#include "IFilePathable.h"

namespace rsc
{
	class CompatiblePackScriptsCrawler : public IFileCrawler, public IFilePathable
	{
	public:
		CompatiblePackScriptsCrawler(const Path& workdir, 
									 const Path& fileRelPath,
									 const Path& packScriptWorkdir);
		
		virtual ~CompatiblePackScriptsCrawler();

		static IFileCrawler* Creator_V1();
		static IFileCrawler* Creator_V2();
		static IFileCrawler* Creator_V3();

	protected:
		virtual bool _Load() override;
		virtual bool _Gather(std::vector<PathPtr>& out) override;

	private:
		Path m_packScriptWorkdir;
	};
}