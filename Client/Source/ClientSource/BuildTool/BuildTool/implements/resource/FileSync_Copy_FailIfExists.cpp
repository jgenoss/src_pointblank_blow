#include "stdafx.h"
#include "FileSync_Copy_FailIfExists.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

namespace rsc
{
	FileSync_Copy_FailIfExists::~FileSync_Copy_FailIfExists() {}

	bool FileSync_Copy_FailIfExists::operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const
	{
		const Path srcAbsPath = srcWorkdir / fileRelPath;
		if (!fs::exists(srcAbsPath))	// 원본 파일은 반드시 존재해야 한다.
		{
			const std::wstring wstrErrMessage =
				boost::io::str(boost::wformat(L"%s\\%s is not exists") % srcWorkdir % fileRelPath);

			GetCallback()->OnFailed(fileRelPath, srcWorkdir, dstWorkdir, wstrErrMessage.c_str());

			return false;
		}

		const Path dstAbsPath = dstWorkdir / fileRelPath;
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
			// 카피하려는 경로에 없으면 복사. 있으면 skip.
			if (!fs::exists(dstAbsPath))
			{
				fs::copy_file(srcAbsPath, dstAbsPath);

				GetCallback()->OnSucceeded(fileRelPath, srcWorkdir, dstWorkdir);
			}
			else
			{
				const std::wstring wstrErrMessage =
					boost::io::str(boost::wformat(L"[FileSync_Copy_FailIfExists] %s\\%s is already exists. so skip this.") % srcWorkdir % fileRelPath);
				
				GetCallback()->OnFailed(fileRelPath, srcWorkdir, dstWorkdir, wstrErrMessage.c_str());
			}
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