#pragma once

#include "FileSync_Wrapper.h"

namespace rsc
{
	struct FileSync_Copy_i3Pack : FileSync_Wrapper
	{
		FileSync_Copy_i3Pack(IFileSync* p);
		virtual ~FileSync_Copy_i3Pack();

		virtual bool operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const override;
	};
}