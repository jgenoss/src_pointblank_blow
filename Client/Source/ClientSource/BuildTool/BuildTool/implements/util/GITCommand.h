#pragma once

#include "IProcessCommand.h"
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/singleton.hpp>
#include <utility>

namespace git
{
	class GitExePath : public boost::serialization::singleton<GitExePath>
	{
		friend class boost::serialization::singleton<GitExePath>;
	public:
		GitExePath()
		{
			wchar_t wszInstallPath[MAX_PATH] = { 0, };
			if (RegFindFromKey(HKEY_LOCAL_MACHINE, L"SoftWare\\GitForWindows", L"InstallPath", wszInstallPath))
			{
				m_wstrExePath = boost::io::str(boost::wformat(L"%s\\bin\\git.exe") % wszInstallPath);
			}
		}

		static GitExePath& get() { return boost::serialization::singleton<GitExePath>::get_mutable_instance(); }

		const std::wstring& GetPath() const { return m_wstrExePath; }

	private:
		std::wstring m_wstrExePath;
	};

	// --------------------------------------
	// 브랜치 변경
	// git checkout [브랜치 이름]
	// --------------------------------------
	class CheckoutBranch : public ProcessCommand
	{
	public:
		CheckoutBranch(const std::wstring& wstrGitAbsPath, const std::wstring& wstrBranchName)
			: m_wstrGitAbsPath(wstrGitAbsPath), m_wstrBranchName(wstrBranchName) {}
		virtual ~CheckoutBranch() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s checkout %s") % GitExePath::get().GetPath() % m_wstrGitAbsPath % m_wstrBranchName);
		}

		const std::wstring m_wstrBranchName;
		const std::wstring m_wstrGitAbsPath;
	};

	// --------------------------------------
	// 로컬 .git 폴더를 원격서버로부터 갱신받는다 : fetch + merge.
	// git pull
	// --------------------------------------
	class Pull : public ProcessCommand
	{
	public:
		Pull(const std::wstring& wstrGitAbsPath) : m_wstrGitAbsPath(wstrGitAbsPath) {}
		virtual ~Pull() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s pull") % GitExePath::get().GetPath() % m_wstrGitAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
	};

	// --------------------------------------
	// 파일 추가.
	// git clone /로컬/저장소/경로.
	// --------------------------------------
	class Add : public ProcessCommand
	{
	public:
		Add(const std::wstring& wstrBaseFolderAbsPath, std::vector<std::wstring>& addTargetRelPathList)
			: m_wstrBaseFolderAbsPath(wstrBaseFolderAbsPath)
		{
			std::copy(addTargetRelPathList.begin(), addTargetRelPathList.end(), std::back_inserter(m_addTargetRelPathList));
		}
		virtual ~Add() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			std::wstring wstrAddFileOption;
			if (!m_addTargetRelPathList.empty())
				wstrAddFileOption = L"-- ";

			for (auto &wstrCurrRelPath : m_addTargetRelPathList)
			{
				wstrAddFileOption += L" ";
				wstrAddFileOption += wstrCurrRelPath;
			}

			return boost::io::str(
				boost::wformat(L"%s -C %s add %s") % GitExePath::get().GetPath() % m_wstrBaseFolderAbsPath % wstrAddFileOption);
		}

		const std::wstring m_wstrBaseFolderAbsPath;
		std::vector<std::wstring> m_addTargetRelPathList;
	};

	// --------------------------------------
	// 변경 내역을 로컬에 저장.
	// git commit -m "설명"
	// --------------------------------------
	class Commit : public ProcessCommand
	{
	public:
		Commit(const std::wstring& wstrBaseFolderAbsPath, std::vector<std::wstring>& commitFileRelPathList, const std::wstring& wstrCommitMsg)
			: m_wstrBaseFolderAbsPath(wstrBaseFolderAbsPath), m_wstrCommitMsg(wstrCommitMsg)
		{
			std::copy(commitFileRelPathList.begin(), commitFileRelPathList.end(), std::back_inserter(m_commitFileRelPathList));
		}
		virtual ~Commit() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			std::wstring wstrCommitFileOption;
			if (!m_commitFileRelPathList.empty())
				wstrCommitFileOption = L"--";

			for (auto &wstrCurrRelPath : m_commitFileRelPathList)
			{
				wstrCommitFileOption += L" ";
				wstrCommitFileOption += wstrCurrRelPath;
			}

			return boost::io::str(
				boost::wformat(L"%s -C %s commit -m %s %s") % GitExePath::get().GetPath() % m_wstrBaseFolderAbsPath % m_wstrCommitMsg % wstrCommitFileOption);
		}

		const std::wstring m_wstrBaseFolderAbsPath;
		std::vector<std::wstring> m_commitFileRelPathList;
		const std::wstring m_wstrCommitMsg;
	};

	class CommitAll : public ProcessCommand
	{
	public:
		CommitAll(const std::wstring& wstrBaseFolderAbsPath, const std::wstring& wstrCommitMsg)
			: m_wstrBaseFolderAbsPath(wstrBaseFolderAbsPath), m_wstrCommitMsg(wstrCommitMsg)
		{}
		virtual ~CommitAll() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s commit -m %s") % GitExePath::get().GetPath() % m_wstrBaseFolderAbsPath % m_wstrCommitMsg);
		}

		const std::wstring m_wstrBaseFolderAbsPath;
		const std::wstring m_wstrCommitMsg;
	};

	// --------------------------------------
	// 변경 내역을 master 저장소에 저장
	// git push origin master
	// --------------------------------------
	class Push : public ProcessCommand
	{
	public:
		Push(const std::wstring& wstrGitAbsPath, const std::wstring& wstrCommitMsg)
			: m_wstrGitAbsPath(wstrGitAbsPath)
		{}
		virtual ~Push() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s push") % GitExePath::get().GetPath() % m_wstrGitAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
	};

	class Log : public PipeProcessCommand
	{
	public:
		Log(const std::wstring& wstrGitAbsPath) : m_wstrGitAbsPath(wstrGitAbsPath)
		{}
		virtual ~Log() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s log") % GitExePath::get().GetPath() % m_wstrGitAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
	};

	class Log_RangeStatus : public PipeProcessCommand
	{
	public:
		Log_RangeStatus(const std::wstring& wstrBaseFolderAbsPath, const int numOfCommits)
			: m_wstrBaseFolderAbsPath(wstrBaseFolderAbsPath), m_numOfCommits(numOfCommits)
		{}
		virtual ~Log_RangeStatus() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s log -%d --name-status") % GitExePath::get().GetPath() % m_wstrBaseFolderAbsPath % m_numOfCommits);
		}

		const std::wstring m_wstrBaseFolderAbsPath;
		const int m_numOfCommits;
	};

	class Status : public PipeProcessCommand
	{
	public:
		Status(const std::wstring& wstrGitAbsPath) : m_wstrGitAbsPath(wstrGitAbsPath)
		{}
		virtual ~Status() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s status") % GitExePath::get().GetPath() % m_wstrGitAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
	};

	class ResetLastCommit : public ProcessCommand
	{
	public:
		ResetLastCommit(const std::wstring& wstrBaseFolderAbsPath) : m_wstrBaseFolderAbsPath(wstrBaseFolderAbsPath)
		{}
		virtual ~ResetLastCommit() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s reset HEAD^") % GitExePath::get().GetPath() % m_wstrBaseFolderAbsPath);
		}

		const std::wstring m_wstrBaseFolderAbsPath;
	};

	class ResetCommits : public ProcessCommand
	{
	public:
		ResetCommits(const std::wstring& wstrBaseFolderAbsPath, const int numOfCommits)
			: m_wstrBaseFolderAbsPath(wstrBaseFolderAbsPath), m_numOfCommits(numOfCommits)
		{}
		virtual ~ResetCommits() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s reset HEAD~%d") % GitExePath::get().GetPath() % m_wstrBaseFolderAbsPath % m_numOfCommits);
		}

		const std::wstring m_wstrBaseFolderAbsPath;
		const int m_numOfCommits;
	};

	class Remove : public ProcessCommand
	{
	public:
		Remove(const std::wstring& wstrBaseFolderAbsPath, std::vector<std::wstring>& removeFileRelPathList)
			: m_wstrBaseFolderAbsPath(wstrBaseFolderAbsPath)
		{
			std::copy(removeFileRelPathList.begin(), removeFileRelPathList.end(), std::back_inserter(m_removeFileRelPathList));
		}
		virtual ~Remove() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			std::wstring wstrRemoveOption;
			if (!m_removeFileRelPathList.empty())
				wstrRemoveOption = L"-- ";

			for (auto &wstrCurrRelPath : m_removeFileRelPathList)
			{
				wstrRemoveOption += L" ";
				wstrRemoveOption += wstrCurrRelPath;
			}

			return boost::io::str(
				boost::wformat(L"%s -C %s rm %s") % GitExePath::get().GetPath() % m_wstrBaseFolderAbsPath % wstrRemoveOption);
		}

		const std::wstring m_wstrBaseFolderAbsPath;
		std::vector<std::wstring> m_removeFileRelPathList;
	};

	class Cherry : public PipeProcessCommand
	{
	public:
		Cherry(const std::wstring& wstrGitAbsPath) : m_wstrGitAbsPath(wstrGitAbsPath)
		{}
		virtual ~Cherry() {}

	private:
		virtual std::wstring MakeCommandLine() override
		{
			return boost::io::str(
				boost::wformat(L"%s -C %s cherry") % GitExePath::get().GetPath() % m_wstrGitAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
	};

	int GetNumberOfCommitsNotPushed(const std::wstring& wstrBaseFolderAbsPath);

	struct LogStatusDiff
	{
		std::vector<std::wstring> commitMessageList;
		std::vector<std::wstring> modifiedList;
		std::vector<std::wstring> addedList;
		std::vector<std::wstring> deletedList;
	};

	bool GetLogStatusDiff(const std::wstring& wstrBaseFolderAbsPath, const int range, LogStatusDiff& out);

	bool CommitSplittedCommits(const std::wstring& wstrBaseFolderAbsPath, const LogStatusDiff& diffs, const std::vector<std::wstring>& extList);

	class ICommand
	{
	public:
		virtual ~ICommand() {}
		virtual bool operator()() = 0;
	};

	class SplitCommit : public ICommand
	{
	public:
		SplitCommit(const std::wstring& wstrBaseFolderAbsPath, const std::vector<std::wstring>& extList);
		virtual ~SplitCommit();

		virtual bool operator()() override;

	private:
		const std::wstring m_wstrBaseFolderAbsPath;
		const std::vector<std::wstring> m_extList;
	};
}