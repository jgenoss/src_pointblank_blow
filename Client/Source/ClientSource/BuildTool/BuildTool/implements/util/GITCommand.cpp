#include "stdafx.h"
#include "GITCommand.h"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include "CustomUtil.h"

namespace
{
	void RemoveDuplicates(std::vector<std::wstring>& list)
	{
		std::sort(list.begin(), list.end());
		list.erase(std::unique(list.begin(), list.end()), list.end());
	}

	void SplitListByExt(const std::vector<std::wstring>& origRelPathList, std::vector<std::wstring>& exceptList, std::vector<std::wstring>& remainList, const std::vector<std::wstring>& extList)
	{
		for (auto &ext : extList)
		{
			for (auto &currRelPath : origRelPathList)
			{
				if (currRelPath.find(ext) != std::wstring::npos)
					exceptList.push_back(currRelPath);
				else
					remainList.push_back(currRelPath);
			}
		}
	}

	bool IsExtIncluded(const std::vector<std::wstring>& list, const std::wstring& wstrExt)
	{
		for (const auto& wstr : list)
		{
			if (wstr.find(wstrExt) != std::wstring::npos)
				return true;
		}

		return false;
	}

	bool IsExtIncludedInDiffs(const git::LogStatusDiff& diff, const std::vector<std::wstring>& extList)
	{
		for (const auto& ext : extList)
		{
			if (IsExtIncluded(diff.addedList, ext))
				return true;

			if (IsExtIncluded(diff.deletedList, ext))
				return true;

			if (IsExtIncluded(diff.modifiedList, ext))
				return true;
		}

		return false;
	}
}

namespace git
{
	int GetNumberOfCommitsNotPushed(const std::wstring& wstrBaseFolderAbsPath)
	{
		Cherry cherry(wstrBaseFolderAbsPath);
		cherry.operator()();

		std::vector<std::wstring> wstrLineTextList;
		ConvertBufferToLineTexts(cherry.GetBuffer(), wstrLineTextList);

		return wstrLineTextList.size();
	}

	bool GetLogStatusDiff(const std::wstring& wstrBaseFolderAbsPath, const int range, LogStatusDiff& out)
	{
		Log_RangeStatus log(wstrBaseFolderAbsPath, range);
		log.operator()();

		std::vector<std::wstring> wstrLineTextList;
		ConvertBufferToLineTexts(log.GetBuffer(), wstrLineTextList);
		if (wstrLineTextList.empty())
			return false;

		// 0: commit SHA
		// 1. Author
		// 2. Date
		// 3. commit message
		// 4 ... : 파일 변경 정보.
		// 위 단계를 커밋별로 반복.

		using tok = boost::tokenizer < boost::char_separator<wchar_t>, std::wstring::const_iterator, std::wstring >;
		boost::char_separator<wchar_t> sep(L"\t");
		std::vector<std::wstring> tokenList;
		for (size_t i = 0; i < wstrLineTextList.size(); i++)
		{
			tokenList.clear();
			tok tokens(wstrLineTextList[i], sep);
			tok::const_iterator it = tokens.begin();
			for (; it != tokens.end(); it++)
				tokenList.push_back(*it);

			if (tokenList.front().find(L"Date:") != std::wstring::npos)
			{
				const int msgIdx = i + 1;
				if (msgIdx < int(wstrLineTextList.size()))
				{
					std::wstring wstrCommitMessage = wstrLineTextList[msgIdx];
					boost::algorithm::trim_left(wstrCommitMessage);
					out.commitMessageList.push_back(wstrCommitMessage);
				}
			}

			if (tokenList.front() == L"M")
				out.modifiedList.push_back(tokenList.back());

			if (tokenList.front() == L"A")
				out.addedList.push_back(tokenList.back());

			if (tokenList.front() == L"D")
				out.deletedList.push_back(tokenList.back());
		}

		RemoveDuplicates(out.modifiedList);
		RemoveDuplicates(out.addedList);
		RemoveDuplicates(out.deletedList);

		return true;
	}

	bool CommitSplittedCommits(const std::wstring& wstrBaseFolderAbsPath, const LogStatusDiff& origDiffs, const std::vector<std::wstring>& extList)
	{
		// 분할 커밋 생성.
		LogStatusDiff exceptDiffs, remainDiffs;
		SplitListByExt(origDiffs.addedList, exceptDiffs.addedList, remainDiffs.addedList, extList);
		SplitListByExt(origDiffs.deletedList, exceptDiffs.deletedList, remainDiffs.deletedList, extList);
		SplitListByExt(origDiffs.modifiedList, exceptDiffs.modifiedList, remainDiffs.modifiedList, extList);

		std::array<LogStatusDiff*, 2> arrDiffs = { &exceptDiffs, &remainDiffs };

		std::wstring wstrCommonCommitMsg;
		for (auto &wstrMessage : origDiffs.commitMessageList)
		{
			wstrCommonCommitMsg += wstrMessage;
			wstrCommonCommitMsg += L"\n";
		}

		std::vector<std::wstring> commitList;
		for (auto &diff : arrDiffs)
		{
			if (!diff->addedList.empty())
			{
				Add add(wstrBaseFolderAbsPath, diff->addedList);
				add.operator()();
			}

			if (!diff->deletedList.empty())
			{
				Remove rm(wstrBaseFolderAbsPath, diff->deletedList);
				rm.operator()();
			}

			commitList.clear();
			std::copy(diff->addedList.begin(), diff->addedList.end(), std::back_inserter(commitList));
			std::copy(diff->deletedList.begin(), diff->deletedList.end(), std::back_inserter(commitList));
			std::copy(diff->modifiedList.begin(), diff->modifiedList.end(), std::back_inserter(commitList));

			if (!commitList.empty())
			{
				Commit commit(wstrBaseFolderAbsPath, commitList, wstrCommonCommitMsg);
				commit.operator()();
			}
		}

		return true;
	}

	SplitCommit::SplitCommit(const std::wstring& wstrBaseFolderAbsPath, const std::vector<std::wstring>& extList)
		: m_wstrBaseFolderAbsPath(wstrBaseFolderAbsPath), m_extList(extList)
	{}

	SplitCommit::~SplitCommit()
	{}

	bool SplitCommit::operator()()
	{
		// 로컬에 커밋된 내용 저장 후 커밋들을 리셋시키고 저장된 내용을 가지고 다시 커밋을 생성하는데 확장자명에 따라 분할하여 커밋한다.

		const int numOfCommits = git::GetNumberOfCommitsNotPushed(m_wstrBaseFolderAbsPath);
		if (numOfCommits == 0)
			return false;

		LogStatusDiff diff;
		GetLogStatusDiff(m_wstrBaseFolderAbsPath, numOfCommits, diff);
		if (!IsExtIncludedInDiffs(diff, m_extList))
			return false;

		ResetCommits reset(m_wstrBaseFolderAbsPath, numOfCommits);
		reset.operator()();

		CommitSplittedCommits(m_wstrBaseFolderAbsPath, diff, m_extList);

		return true;
	}
}