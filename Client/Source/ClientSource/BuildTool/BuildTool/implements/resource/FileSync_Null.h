#pragma once

#include "IFileSync.h"

namespace rsc
{
	struct FileSync_Null : IFileSync
	{
	public:
		virtual ~FileSync_Null();

		virtual bool operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const override;
	};
}
