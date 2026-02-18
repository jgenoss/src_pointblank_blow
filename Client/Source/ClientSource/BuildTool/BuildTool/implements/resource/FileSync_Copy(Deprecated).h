#pragma once

#include "IFileSync.h"

namespace rsc
{
	struct FileSync_Copy_Deprecated : IFileSync
	{
	public:
		virtual ~FileSync_Copy_Deprecated();

		virtual bool operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const override;
	};
}