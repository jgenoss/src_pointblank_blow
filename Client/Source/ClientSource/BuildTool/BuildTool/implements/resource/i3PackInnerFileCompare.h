#pragma once

#include "IFileCompare.h"

namespace rsc
{
	// i3pack 에 들어있는 특정 파일을 대상으로 비교 수행.
	struct i3PackInnerFileCompare : IFileCompare
	{
		virtual ~i3PackInnerFileCompare() {}

		virtual bool IsExist(const Path& workdir, const Path& fileRelPath) override;
		virtual bool CalcMD5(const Path& workdir, const Path& fileRelPath, const Crypto::MD5Handle md5handle, Crypto::MD5Result& outMD5Value) override;
		virtual unsigned long long GetFileSize(const Path& workdir, const Path& fileRelPath) override;
	};
}