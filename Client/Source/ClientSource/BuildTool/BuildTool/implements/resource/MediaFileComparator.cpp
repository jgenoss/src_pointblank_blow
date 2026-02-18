#include "stdafx.h"
#include "MediaFileComparator.h"
#include <boost/filesystem.hpp>
#include "FileCompareFactory.h"
#include "NativeFileCompare.h"

namespace rsc
{
	MediaFileComparator::MediaFileComparator(const Path& workdir)
		: IFileComparator(workdir)
	{}
	
	MediaFileComparator::~MediaFileComparator() 
	{}

	bool MediaFileComparator::_IsExist(const Path& fileRelPath) const
	{
		const std::string& strExt = fileRelPath.extension().string();
		auto comp = NativeFileCompareFactory::i()->RequestObj(strExt);

		return comp->IsExist(m_workdir, fileRelPath);
	}

	bool MediaFileComparator::_CalcMD5(const Path& fileRelPath, Crypto::MD5Result& outMD5Value) const
	{
		const std::string& strExt = fileRelPath.extension().string();
		auto comp = NativeFileCompareFactory::i()->RequestObj(strExt);

		return comp->CalcMD5(m_workdir, fileRelPath, m_md5Handle, outMD5Value);
	}

	unsigned long long MediaFileComparator::_GetFileSize(const Path& fileRelPath) const
	{
		const std::string& strExt = fileRelPath.extension().string();
		auto comp = NativeFileCompareFactory::i()->RequestObj(strExt);

		return comp->GetFileSize(m_workdir, fileRelPath);
	}
}