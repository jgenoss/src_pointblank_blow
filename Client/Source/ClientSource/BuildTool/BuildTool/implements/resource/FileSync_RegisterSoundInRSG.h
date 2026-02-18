#pragma once

#include "FileSync_Wrapper.h"

namespace rsc
{
	struct FileSync_RegisterSoundInRSG : FileSync_Wrapper
	{
		FileSync_RegisterSoundInRSG(IFileSync* p);
		virtual ~FileSync_RegisterSoundInRSG();

		virtual bool operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const override;
	};
}