#include "stdafx.h"
#include "LiveFileComparator.h"
#include "FileCompareFactory.h"
#include "NativeFileCompare.h"
#include "i3PackInnerFileCompare.h"
#include <boost/filesystem.hpp>


namespace rsc
{
	LiveFileComparator::LiveFileComparator(const Path& workdir)
		: IFileComparator(workdir)
	{}
	
	LiveFileComparator::~LiveFileComparator() 
	{}

	bool LiveFileComparator::_IsExist(const Path& fileRelPath) const
	{
		const std::string& strExt = fileRelPath.extension().string();

		if (fs::exists(m_workdir / fileRelPath))
		{
			auto comp = NativeFileCompareFactory::i()->RequestObj(strExt);
			return comp->IsExist(m_workdir, fileRelPath);
		}
		
		auto comp = i3PackCompareFactory::i()->RequestObj(strExt);
		return comp->IsExist(m_workdir, fileRelPath);
	}

	bool LiveFileComparator::_CalcMD5(const Path& fileRelPath, Crypto::MD5Result& outMD5Value) const
	{
		const std::string& strExt = fileRelPath.extension().string();

		if (fs::exists(m_workdir / fileRelPath))
		{
			auto comp = NativeFileCompareFactory::i()->RequestObj(strExt);
			return comp->CalcMD5(m_workdir, fileRelPath, m_md5Handle, outMD5Value);
		}

		auto comp = i3PackCompareFactory::i()->RequestObj(strExt);
		return comp->CalcMD5(m_workdir, fileRelPath, m_md5Handle, outMD5Value);
	}

	unsigned long long LiveFileComparator::_GetFileSize(const Path& fileRelPath) const
	{
		const std::string& strExt = fileRelPath.extension().string();

		if (fs::exists(m_workdir / fileRelPath))
		{
			auto comp = NativeFileCompareFactory::i()->RequestObj(strExt);
			return comp->GetFileSize(m_workdir, fileRelPath);
		}

		auto comp = i3PackCompareFactory::i()->RequestObj(strExt);
		return comp->GetFileSize(m_workdir, fileRelPath);
	}
}