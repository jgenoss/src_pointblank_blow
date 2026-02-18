#pragma once

#include "IFileSync.h"

namespace rsc
{
	struct FileSync_Copy_FailIfExists : IFileSync
	{
	public:
		virtual ~FileSync_Copy_FailIfExists();

		virtual bool operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const override;
	};
}