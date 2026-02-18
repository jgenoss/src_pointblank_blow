#pragma once

#include "IPackSync.h"

namespace rsc
{
	class PackSync_World_V2 : public IPackSync
	{
	public:
		virtual ~PackSync_World_V2() {}

	protected:
		virtual void _PackingByRule(const std::vector<fs::path>& fileRelPathList,
									const fs::path& srcWorkdir,
									const fs::path& dstWorkdir,
									SyncLoggerPtr logger) override;
	};
}