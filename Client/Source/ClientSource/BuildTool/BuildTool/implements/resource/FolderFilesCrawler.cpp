#include "stdafx.h"
#include "FolderFilesCrawler.h"
#include "RscTool/RSCFileMgr.h"
#include "i3Base/string/ext/extract_fileext.h"
#include <boost/filesystem.hpp>
#include <algorithm>
#include <boost/tokenizer.hpp>

using tok = boost::tokenizer < boost::char_separator<char> >;

namespace
{
	template <class IteratorType>
	void CrawlFolder(const Path& workdir, const Path& folderRelPath, const std::vector<std::string>& fileSearchKeyList, const bool includeDirOK, std::vector<PathPtr>& out)
	{
		Path folderAbsPath(workdir/folderRelPath);	
		folderAbsPath.make_preferred();

		if (!fs::exists(folderAbsPath))
			return;

		if (!fs::is_directory(folderAbsPath))
			return;

		Path currFileAbsPath;
		std::string strCurrFileName, strFileRelPath;

		for (IteratorType it(folderAbsPath); it != IteratorType(); ++it)
		{
			if (!includeDirOK)
			{
				if (fs::is_directory(it->status()))	// 디렉터리는 제외.
					continue;
			}

			currFileAbsPath = (*it).path();
			currFileAbsPath.make_preferred();

			// workdir 을 제거하여 상대경로로 만들어 집어넣음.
			strFileRelPath = currFileAbsPath.string().c_str();	
			strFileRelPath.erase(0, workdir.string().size() + 1);	// +1 는 마지막 \\.

			if (fileSearchKeyList.empty())
			{
				out.push_back(std::make_shared<Path>(strFileRelPath.c_str()));
				continue;
			}

			for (auto &strKey : fileSearchKeyList)
			{
				strCurrFileName = currFileAbsPath.filename().string();
				std::transform(strCurrFileName.begin(), strCurrFileName.end(), strCurrFileName.begin(), toupper);

				// 파일명에 '*' 이 들어간 경우.
				if (strKey.find("*") != std::string::npos)
				{
					boost::char_separator<char> sep("*");

					std::vector<std::string> strTokenList;
					tok tokens(strKey, sep);

					tok::const_iterator it = tokens.begin();
					for (; it != tokens.end(); it++)
						strTokenList.push_back(*it);

					bool matchOK = true;
					for (auto &strTok : strTokenList)
					{
						if (strCurrFileName.find(strTok) == std::string::npos)
						{
							matchOK = false;
							break;
						}
					}

					if (matchOK)
						out.push_back(std::make_shared<Path>(strFileRelPath.c_str()));
				}
				else
				{
					auto pos = strCurrFileName.find(strKey);
					if (pos != std::string::npos)
						out.push_back(std::make_shared<Path>(strFileRelPath.c_str()));
				}
			}
		}
	}
}

namespace rsc
{
	FolderFilesCrawler::~FolderFilesCrawler() 
	{}

	void FolderFilesCrawler::SetRecursive(const bool ok)
	{
		m_recursive = ok;
	}

	void FolderFilesCrawler::SetFileSearchKeys(const std::initializer_list<std::string>& strKeyList)
	{
		m_strSearchKeyList.clear();
		m_strSearchKeyList = std::move(strKeyList);

		for (auto &strSearchKey : m_strSearchKeyList)
			std::transform(strSearchKey.begin(), strSearchKey.end(), strSearchKey.begin(), toupper);
	}

	void FolderFilesCrawler::ClearFileSearchKeys()
	{
		m_strSearchKeyList.clear();
	}

	void FolderFilesCrawler::SetIncludeSubFolder(const bool ok)
	{
		m_isIncludeSubFolder = ok;
	}

	bool FolderFilesCrawler::_Load()
	{
		const Path& fileAbsPath = IFilePathable::GetAbsolutePath();

		if (!fs::exists(fileAbsPath))
			return false;

		if (!fs::is_directory(fileAbsPath))
			return false;

		return true;
	}

	bool FolderFilesCrawler::_Gather(std::vector<PathPtr>& out)
	{
		if (m_recursive) // 하위 폴더 포함.
		{
			CrawlFolder<fs::recursive_directory_iterator>(GetWorkdir(), GetRelativePath(), m_strSearchKeyList, m_isIncludeSubFolder, out);
		}
		else // 현재 폴더만.
		{
			CrawlFolder<fs::directory_iterator>(GetWorkdir(), GetRelativePath() , m_strSearchKeyList, m_isIncludeSubFolder, out);
		}

		return true;
	}
}