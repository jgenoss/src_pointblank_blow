#pragma once

#include "IFileComparator.h"

namespace rsc
{
	class MediaFileComparator : public IFileComparator
	{
	public:
		MediaFileComparator() = default;
		MediaFileComparator(const Path& workdir);
		virtual ~MediaFileComparator();

	protected:
		virtual bool _IsExist(const Path& fileRelPath) const override;
		virtual bool _CalcMD5(const Path& fileRelPath, Crypto::MD5Result& outMD5Value) const override;
		virtual unsigned long long _GetFileSize(const Path& fileRelPath) const override;
	};

	using QAFileComparator = MediaFileComparator;	// QA 폴더와 Media 폴더 구조는 다를게 없다. 이름만 바꿔 사용한다.
}