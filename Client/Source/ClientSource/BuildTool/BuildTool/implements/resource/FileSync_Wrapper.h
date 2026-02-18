#pragma once

#include "IFileSync.h"

namespace rsc
{
	struct FileSync_Wrapper : IFileSync
	{
		FileSync_Wrapper(IFileSync* p);
		virtual ~FileSync_Wrapper();

		virtual bool operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const override;

	private:
		std::unique_ptr<IFileSync> m_sync;
	};
}