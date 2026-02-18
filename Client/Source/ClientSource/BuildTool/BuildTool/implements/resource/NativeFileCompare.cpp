#include "stdafx.h"
#include "NativeFileCompare.h"
#include <boost/filesystem.hpp>

namespace rsc
{
	bool NativeFileCompare::IsExist(const Path& workdir, const Path& fileRelPath)
	{
		const Path fileAbsPath = workdir / fileRelPath;

		if (fs::is_directory(fileAbsPath))
			return false;

		if (!fs::exists(fileAbsPath))
			return false;

		return true;
	}

	bool NativeFileCompare::CalcMD5(const Path& workdir, const Path& fileRelPath, const Crypto::MD5Handle md5handle, Crypto::MD5Result& outMD5Value)
	{
		return GetMD5_MediaQA(workdir.wstring().c_str(), fileRelPath.string().c_str(), md5handle, outMD5Value);
	}

	unsigned long long NativeFileCompare::GetFileSize(const Path& workdir, const Path& fileRelPath)
	{
		return fs::file_size(workdir / fileRelPath);
	}
}