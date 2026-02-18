#pragma once

#include "FileSync_Wrapper.h"

namespace rsc
{
	struct FileSync_RegisterInRSG : FileSync_Wrapper
	{
		FileSync_RegisterInRSG(IFileSync* p);
		virtual ~FileSync_RegisterInRSG();

		virtual bool operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const override;
	};
}