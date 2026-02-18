#include "stdafx.h"
#include "FileSync_Wrapper.h"

namespace rsc
{
	FileSync_Wrapper::FileSync_Wrapper(IFileSync* p) : m_sync(p)
	{}

	FileSync_Wrapper::~FileSync_Wrapper()
	{}

	bool FileSync_Wrapper::operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const
	{
		return m_sync->operator()(fileRelPath, srcWorkdir, dstWorkdir);
	}
}