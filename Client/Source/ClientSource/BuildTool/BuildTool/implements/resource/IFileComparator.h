#pragma once

#include "RscTool/md5.h"
#include <vector>

namespace rsc
{
	class IFileComparator
	{
	public:
		IFileComparator() = default;
		IFileComparator(const Path& workdir);
		virtual ~IFileComparator();

		void SetWorkdir(const Path& workdir);
		void AddFileRelPathList(const std::vector<PathPtr>& list);
		void Clear();

		bool CheckFiles();
		bool IsEqualTo(const IFileComparator& rhs);

	protected:
		virtual bool _IsExist(const Path& fileRelPath) const = 0;
		virtual bool _CalcMD5(const Path& fileRelPath, Crypto::MD5Result& outMD5Value) const = 0;
		virtual unsigned long long _GetFileSize(const Path& fileRelPath) const = 0;

		Path m_workdir;
		std::vector<PathPtr> m_compTgtRelPathList;
		Crypto::MD5Handle m_md5Handle;
	};
}