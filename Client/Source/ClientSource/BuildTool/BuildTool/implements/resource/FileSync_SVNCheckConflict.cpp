#include "stdafx.h"
#include "FileSync_SVNCheckConflict.h"
#include "RscTool/ExecuteSVN.h"
#include <boost/filesystem.hpp>
#include "RscTool/ExecuteTSVN.h"

namespace rsc
{
	FileSync_SVNCheckConflict::FileSync_SVNCheckConflict(IFileSync* p) : FileSync_Wrapper(p)
	{}

	FileSync_SVNCheckConflict::~FileSync_SVNCheckConflict()
	{}

	bool FileSync_SVNCheckConflict::operator()(const Path& fileRelpath, const Path& srcWorkdir, const Path& dstWorkdir) const
	{
		// 충돌 상태 검사.
		i3::vector<i3::rc_string> outConflictedAbsPathList;
		
		const Path srcAbsPath(srcWorkdir/fileRelpath);
		if (!fs::exists(srcAbsPath))
			return false;

		const i3::rc_string strSrcAbsPath(srcAbsPath.string().c_str());
		svn::GetConflictStatusFiles(strSrcAbsPath, outConflictedAbsPathList);

		for (auto& conflictedAbsPath : outConflictedAbsPathList)
		{
			fs::remove(conflictedAbsPath.c_str());	// 삭제.
			tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(conflictedAbsPath));
		}

		return FileSync_Wrapper::operator()(fileRelpath, srcWorkdir, dstWorkdir);
	}
}