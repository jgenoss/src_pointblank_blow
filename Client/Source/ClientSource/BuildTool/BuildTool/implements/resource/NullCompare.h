#pragma once

#include "IFileCompare.h"

namespace rsc
{
	struct NullCompare : IFileCompare
	{
		virtual ~NullCompare() {}

		virtual bool IsExist(const Path& workdir, const Path& fileRelPath) override { return false; }
		virtual bool CalcMD5(const Path& workdir, const Path& fileRelPath, const Crypto::MD5Handle md5handle, Crypto::MD5Result& outMD5Value) override { return false; }
		virtual unsigned long long GetFileSize(const Path& workdir, const Path& fileRelPath) override { return 0; }
	};
}