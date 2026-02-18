#pragma once

#include "FileSync_Wrapper.h"

namespace rsc
{
	struct FileSync_SVNCheckConflict : FileSync_Wrapper
	{
		FileSync_SVNCheckConflict(IFileSync* p);
		virtual ~FileSync_SVNCheckConflict();

		virtual bool operator()(const Path& fileRelpath, const Path& srcWorkdir, const Path& dstWorkdir) const override;
	};
}