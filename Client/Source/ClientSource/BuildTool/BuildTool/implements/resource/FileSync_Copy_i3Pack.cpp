#include "stdafx.h"
#include "FileSync_Copy_i3Pack.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include "LiveFileComparator.h"

namespace rsc
{
	FileSync_Copy_i3Pack::FileSync_Copy_i3Pack(IFileSync* p) : FileSync_Wrapper(p) {}

	FileSync_Copy_i3Pack::~FileSync_Copy_i3Pack() {}

	bool FileSync_Copy_i3Pack::operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const
	{


		if (!FileSync_Wrapper::operator()(fileRelPath, srcWorkdir, dstWorkdir))
			return false;

		return true;
	}
}