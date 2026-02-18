#pragma once

#include "IPackSync.h"

namespace rsc
{
	class PackSync_Avatar : public IPackSync
	{
	public:
		virtual ~PackSync_Avatar() {}

	protected:
		virtual void _PackingByRule(const std::vector<fs::path>& fileRelPathList,
									const fs::path& srcWorkdir,
									const fs::path& dstWorkdir,
									SyncLoggerPtr logger) override;
	};
}