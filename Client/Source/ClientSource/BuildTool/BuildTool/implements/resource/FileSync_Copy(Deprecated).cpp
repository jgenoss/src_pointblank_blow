#include "stdafx.h"
#include "FileSync_Copy(Deprecated).h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

namespace rsc
{
	FileSync_Copy_Deprecated::~FileSync_Copy_Deprecated() {}

	bool FileSync_Copy_Deprecated::operator()(const Path& fileRelPath, const Path& srcWorkdir, const Path& dstWorkdir) const
	{
		const Path srcFileAbsPath = srcWorkdir / fileRelPath;
		const Path dstFileAbsPath = dstWorkdir / fileRelPath;

		if (!fs::exists(srcFileAbsPath))	// 원본 파일은 반드시 존재해야 한다.
		{
			const std::wstring wstrErrMessage =
				boost::io::str(boost::wformat(L"%s\\%s is not exists") % srcWorkdir % fileRelPath);

			GetCallback()->OnFailed(fileRelPath, srcWorkdir, dstWorkdir, wstrErrMessage.c_str());

			return false;
		}

		if (!fs::exists(dstFileAbsPath))
		{
			Path dir;

			if (fs::is_directory(dstFileAbsPath))
				dir = dstFileAbsPath;
			else
				dir = dstFileAbsPath.parent_path();	// 파일이면 상위 폴더를 대상으로 생성.

			if (!fs::exists(dir))
				fs::create_directories(dir);
		}

		try
		{
			const std::string& srcFileName = srcFileAbsPath.filename().string();
			bool replaceEqipmentPefNameOK = false;

			if (i3::generic_is_iequal(srcFileName, "Equipment_Backup.pef") ||
				i3::generic_is_iequal(srcFileName, "Equipment_Backup.i3RegXML"))
			{
				replaceEqipmentPefNameOK = true;
			}

			if (replaceEqipmentPefNameOK)	// 임시로 Equipment_Backup.pef 파일은 Equipment.pef 로 복사. 양승천 작업 끝나면 삭제 처리해야함.
			{
				std::string relpath(fileRelPath.string());
				std::string match("Equipment_Backup");

				const size_t offset = relpath.find(match);
				relpath.replace(offset, match.length(), "Equipment");

				Path dst_equip(dstWorkdir);
				dst_equip /= relpath;

				fs::copy_file(srcFileAbsPath, dst_equip, fs::copy_option::overwrite_if_exists);
				GetCallback()->OnSucceeded(fileRelPath, srcWorkdir, dstWorkdir);

				return true;
			}

			fs::copy_file(srcFileAbsPath, dstFileAbsPath, fs::copy_option::overwrite_if_exists);
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