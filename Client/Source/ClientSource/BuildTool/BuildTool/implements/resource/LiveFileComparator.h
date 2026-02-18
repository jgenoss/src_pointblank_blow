#pragma once

#include "IFileComparator.h"
#include "RscTool/md5.h"

namespace rsc
{
	class LiveFileComparator : public IFileComparator
	{
	public:
		LiveFileComparator() = default;
		LiveFileComparator(const Path& workdir);
		virtual ~LiveFileComparator();

	protected:
		virtual bool _IsExist(const Path& fileRelPath) const override;
		virtual bool _CalcMD5(const Path& fileRelPath, Crypto::MD5Result& outMD5Value) const override;
		virtual unsigned long long _GetFileSize(const Path& fileRelPath) const override;
	};
}