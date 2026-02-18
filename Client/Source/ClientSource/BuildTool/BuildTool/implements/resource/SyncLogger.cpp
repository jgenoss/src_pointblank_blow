#include "stdafx.h"
#include "SyncLogger.h"
#include <chrono>
#include <boost/filesystem.hpp>

namespace rsc
{
	SyncLogger::SyncLogger(const std::string& name)
	{
		_CreateLogFolder("", name);	// 실행파일 위치에 로그폴더 생성.
	}

	SyncLogger::SyncLogger(const Path& rootDir, const std::string& name)
	{
		_CreateLogFolder(rootDir, name);
	}

	SyncLogger::~SyncLogger()
	{
	}

	void SyncLogger::_CreateLogFolder(const Path& rootDir, const std::string& name)
	{
		std::chrono::system_clock::time_point tpoint = std::chrono::system_clock::now();
		std::time_t tt = std::chrono::system_clock::to_time_t(tpoint);
		std::tm* t = std::localtime(&tt);

		char affix_time[_MAX_PATH] = { 0, };
		sprintf_s(affix_time, _MAX_PATH, "_%d%d%d_%d%d%d", t->tm_year, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

		m_logWorkdir = rootDir;

		if (m_logWorkdir.string().empty())	// 주어진 기본 경로가 없으면 실행파일 위치에 로그폴더를 만든다.
		{
			CString rootPath;
			GetModuleFileName(NULL, rootPath.GetBuffer(_MAX_PATH), _MAX_PATH);
			rootPath.ReleaseBuffer();
			rootPath = rootPath.Left(rootPath.ReverseFind(L'\\'));

			const std::wstring wstrRootPath(rootPath);
			const std::string strRootPath(wstrRootPath.begin(), wstrRootPath.end());

			m_logWorkdir = strRootPath;
		}

		m_logWorkdir /= "SyncLog";
		m_logWorkdir /= name;

		std::string uniqueFolderName(name);
		uniqueFolderName += affix_time;

		m_logWorkdir /= uniqueFolderName;

		if (!fs::exists(m_logWorkdir))
			fs::create_directories(m_logWorkdir);
	}

	void SyncLogger::Log(const Path& workdir, const Path& fileRelPath)
	{	
		Path srcFileAbsPath;
		Path dstFileAbsPath;

		srcFileAbsPath = workdir / fileRelPath;
		dstFileAbsPath = m_logWorkdir / fileRelPath;

		if (!fs::exists(dstFileAbsPath))
		{
			Path dstAbsDir;

			if (fs::is_directory(dstFileAbsPath))
				dstAbsDir = dstFileAbsPath;
			else
				dstAbsDir = dstFileAbsPath.parent_path();	// 파일이면 상위 폴더를 대상으로 생성.

			if (!fs::exists(dstAbsDir))
				fs::create_directories(dstAbsDir);
		}

		try
		{
			if (fs::exists(srcFileAbsPath))
				fs::copy_file(srcFileAbsPath, dstFileAbsPath, fs::copy_option::overwrite_if_exists);
		}
		catch (const boost::system::system_error &err)
		{
			err.what();
		}
	}
}