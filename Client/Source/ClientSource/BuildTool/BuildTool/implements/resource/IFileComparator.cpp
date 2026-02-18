#include "stdafx.h"
#include "IFileComparator.h"
#include <boost/filesystem.hpp>
#include "RscTool/md5.h"

namespace rsc
{
	IFileComparator::IFileComparator(const Path& workdir) 
		: m_workdir(workdir), m_md5Handle(Crypto::MD5_CreateHandle())
	{}

	IFileComparator::~IFileComparator()
	{
		Crypto::MD5_DeleteHandle(m_md5Handle);
	}

	void IFileComparator::SetWorkdir(const Path& workdir)
	{
		m_workdir = workdir;
	}

	void IFileComparator::AddFileRelPathList(const std::vector<PathPtr>& list)
	{
		std::copy(list.begin(), list.end(), std::back_inserter(m_compTgtRelPathList));
	}

	void IFileComparator::Clear()
	{
		m_compTgtRelPathList.clear();
	}

	bool IFileComparator::CheckFiles()
	{
		// 파일 존재 유무만 확인한다.

		if (m_compTgtRelPathList.empty())
			return false;

		for (auto &currCompTgt : m_compTgtRelPathList)
		{
			if (!_IsExist(*(currCompTgt.get())))
				return false;
		}

		return true;
	}

	bool IFileComparator::IsEqualTo(const IFileComparator& rhs)
	{
		// 비용이 적게 드는 순으로
		// 파일 사이즈 비교 후 MD5 값 비교.

		for (auto &currCompTgt : m_compTgtRelPathList)
		{
			if (_GetFileSize(*(currCompTgt.get())) != rhs._GetFileSize(*(currCompTgt.get())))
				return false;
		}

		for (auto &currCompTgt : m_compTgtRelPathList)
		{
			Crypto::MD5Result md5_lhs;
			_CalcMD5(*(currCompTgt.get()), md5_lhs);

			Crypto::MD5Result md5_rhs;
			rhs._CalcMD5(*(currCompTgt.get()), md5_rhs);

			if (*reinterpret_cast<UINT128*>(md5_lhs.data) != *reinterpret_cast<UINT128*>(md5_rhs.data))
				return false;
		}

		return true;
	}
}