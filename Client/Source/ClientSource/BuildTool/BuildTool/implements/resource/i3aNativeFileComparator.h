#pragma once

#include "IComparator.h"

namespace rsc
{
	class i3aNativeFileComparator : public IComparator
	{
	public:
		virtual ~i3aNativeFileComparator() {}

		virtual bool IsExist(const fs::path& workdir, const fs::path& file) const = 0;
		virtual bool CalcMD5(const fs::path& workdir, const fs::path& file, const Crypto::MD5Handle md5handle, Crypto::MD5Result& outMD5Value) const = 0;
		virtual unsigned long long GetFileSize(const fs::path& workdir, const fs::path& file) const = 0;
	};
}