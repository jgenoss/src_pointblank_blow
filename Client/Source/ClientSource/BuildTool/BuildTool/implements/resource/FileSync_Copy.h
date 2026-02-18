#pragma once

#include "IFileSync.h"

namespace rsc
{
	struct FileSync_Copy : IFileSync
	{
	public:
		virtual ~FileSync_Copy();

		virtual bool operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const override;
	};
}