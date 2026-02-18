#pragma once

#include "RscTool/md5.h"

namespace rsc
{
	// 파일별(확장자 구분에 따른) 비교 인터페이스.
	struct IFileCompare
	{
		virtual ~IFileCompare() = 0;

		virtual bool IsExist(const Path& workdir, const Path& fileRelPath) = 0;
		virtual bool CalcMD5(const Path& workdir, const Path& fileRelPath, const Crypto::MD5Handle md5handle, Crypto::MD5Result& outMD5Value) = 0;
		virtual unsigned long long GetFileSize(const Path& workdir, const Path& fileRelPath) = 0;
	};
}