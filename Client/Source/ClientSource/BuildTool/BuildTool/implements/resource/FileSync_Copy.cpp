#include "stdafx.h"
#include "FileSync_Copy.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

namespace rsc
{
	FileSync_Copy::~FileSync_Copy() {}

	bool FileSync_Copy::operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const
	{ 
		const Path srcAbsPath(srcWorkdir/fileRelPath);
		if (!fs::exists(srcAbsPath))	// 원본 파일은 반드시 존재해야 한다.
		{
			const std::wstring wstrErrMessage =
				boost::io::str(boost::wformat(L"%s\\%s is not exists") % srcWorkdir % fileRelPath);

			GetCallback()->OnFailed(fileRelPath, srcWorkdir, dstWorkdir, wstrErrMessage.c_str());

			return false;
		}

		const Path dstAbsPath(dstWorkdir/fileRelPath);
		if (!fs::exists(dstAbsPath))
		{
			Path directory;

			if (fs::is_directory(dstAbsPath))
				directory = dstAbsPath;
			else
				directory = dstAbsPath.parent_path();	// 파일이면 상위 폴더를 대상으로 생성.

			if (!fs::exists(directory))
				fs::create_directories(directory);
		}

		try
		{
			fs::copy_file(srcAbsPath, dstAbsPath, fs::copy_option::overwrite_if_exists);
			GetCallback()->OnSucceeded(fileRelPath, srcWorkdir, dstWorkdir);
		}
		catch (const boost::system::system_error &err)
		{
			std::wstring wstrErrWhat;
			i3::mb_to_utf16(err.what(), wstrErrWhat);
			GetCallback()->OnFailed(fileRelPath, srcWorkdir, dstWorkdir, wstrErrWhat.c_str());
		}

		return true;
	}
}