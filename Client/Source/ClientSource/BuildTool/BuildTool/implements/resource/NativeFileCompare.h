#pragma once

#include "IFileCompare.h"

namespace rsc
{
	struct NativeFileCompare : IFileCompare
	{
		virtual ~NativeFileCompare() {}

		virtual bool IsExist(const Path& workdir, const Path& fileRelPath) override;
		virtual bool CalcMD5(const Path& workdir, const Path& fileRelPath, const Crypto::MD5Handle md5handle, Crypto::MD5Result& outMD5Value) override;
		virtual unsigned long long GetFileSize(const Path& workdir, const Path& fileRelPath) override;
	};
}