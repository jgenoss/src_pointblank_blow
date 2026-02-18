#include "stdafx.h"
#include "FileSync_Null.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

namespace rsc
{
	FileSync_Null::~FileSync_Null() {}

	bool FileSync_Null::operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const
	{
		return true;
	}
}