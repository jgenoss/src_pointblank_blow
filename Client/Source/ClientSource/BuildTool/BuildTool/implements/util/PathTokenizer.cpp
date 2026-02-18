#include "stdafx.h"
#include "PathTokenizer.h"
#include <boost/tokenizer.hpp>
#include <algorithm>

using tok = boost::tokenizer < boost::char_separator<char> >;

bool GetPathToken(const Path& path, const size_t nth, std::string& outToken)
{
	boost::char_separator<char> sep("/\\");

	std::string strPath(path.string());
	tok tokens(strPath, sep);

	tok::const_iterator it = tokens.begin();
	for (; it != tokens.end(); it++)
	{
		const size_t dist = std::distance(tokens.begin(), it);
		if (dist == nth)
		{
			outToken = *it;
			return true;
		}
	}

	return false;
}

void GetPathTokenList(const Path& path, std::vector<std::string>& outTokenList)
{
	boost::char_separator<char> sep("/\\");

	outTokenList.clear();

	std::string strPath(path.string());
	tok tokens(strPath, sep);
	tok::const_iterator it = tokens.begin();
	for (; it != tokens.end(); it++)
		outTokenList.push_back(*it);
}

void GetPathTokenList(const Path& path, std::vector<std::wstring>& outTokenList)
{
	boost::char_separator<char> sep("/\\");

	outTokenList.clear();

	std::string strPath(path.string());
	tok tokens(strPath, sep);
	tok::const_iterator it = tokens.begin();
	for (; it != tokens.end(); it++)
	{
		const std::string& strToken = *it;
		outTokenList.push_back(std::wstring(strToken.begin(), strToken.end()));
	}
}

void GetPathTokenList(const Path& path, const size_t begin, const size_t end, std::vector<std::string>& outTokenList)
{
	boost::char_separator<char> sep("/\\");

	outTokenList.clear();

	std::string strPath(path.string());
	tok tokens(strPath, sep);

	const int diff = end - begin;
	if (diff <= 0)
		return;

	size_t count = 0;
	tok::const_iterator it = tokens.begin();
	for (; it != tokens.end(); it++)
	{
		outTokenList.push_back(*it);
		
		if (count == diff)
			break;

		count++;
	}
}

size_t GetNumberOfTokens(const Path& path)
{
	boost::char_separator<char> sep("/\\");

	std::string strPath(path.string());
	tok tokens(strPath, sep);

	size_t count = 0;

	tok::const_iterator it = tokens.begin();
	for (; it != tokens.end(); it++)
		count++;

	return count;
}

void ClassifyCapitalAndSmallLetter(PathPtr& inout, const std::wstring& wstrWorkdir)
{
	if (inout->empty())
		return;

	std::vector<std::wstring> tokenList;
	GetPathTokenList(*(inout.get()), tokenList);

	std::wstring wstrClassified;
	bool ok = false;

	std::wstring wstr = wstrWorkdir;

	for (auto &token : tokenList)
	{
		wstr += L"\\";
		wstr += token;

		CFileFind fd;
		if (fd.FindFile(wstr.c_str()))
		{
			fd.FindNextFile();

			wstrClassified += fd.GetFileName().GetString();
			wstrClassified += L"\\";

			ok = true;
		}
	}

	if (ok)
	{
		wstrClassified.erase(wstrClassified.end() - 1);
		inout->assign(wstrClassified.begin(), wstrClassified.end());
	}
}