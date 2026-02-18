#pragma once

#include "IProcessCommand.h"
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/singleton.hpp>

namespace tgit
{
	class TGitExePath : public boost::serialization::singleton<TGitExePath>
	{
		friend class boost::serialization::singleton<TGitExePath>;
	public:
		TGitExePath()
		{
			wchar_t wszDirPath[MAX_PATH] = { 0, };
			if (RegFindFromKey(HKEY_LOCAL_MACHINE, L"SoftWare\\TortoiseGit", L"Directory", wszDirPath))
			{
				m_wstrExePath = boost::io::str(boost::wformat(L"%sbin\\TortoiseGitProc.exe") % wszDirPath);
			}
		}

		static TGitExePath& get() { return boost::serialization::singleton<TGitExePath>::get_mutable_instance(); }

		const std::wstring& GetPath() const { return m_wstrExePath; }

	private:
		std::wstring m_wstrExePath;
	};

	// https://tortoisegit.org/docs/tortoisegit/tgit-automation.html

	// --------------------------------------
	// 브랜치 변경 다이얼로그.
	// --------------------------------------
	class OpenSwitchDlg : public ProcessCommand
	{
	public:
		OpenSwitchDlg(const std::wstring& wstrGitAbsPath) : m_wstrGitAbsPath(wstrGitAbsPath) {}
		virtual ~OpenSwitchDlg() {}

	private:
		virtual std::wstring MakeCommandLine()
		{
			return boost::io::str(
				boost::wformat(L"%s /command:switch /path:\"%s\" /closeonend:0") % TGitExePath::get().GetPath() % m_wstrGitAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
	};

	// --------------------------------------
	// Pull 다이얼로그.
	// --------------------------------------
	class OpenPullDlg : public ProcessCommand
	{
	public:
		OpenPullDlg(const std::wstring& wstrGitAbsPath) : m_wstrGitAbsPath(wstrGitAbsPath) {}
		virtual ~OpenPullDlg() {}

	private:
		virtual std::wstring MakeCommandLine()
		{
			return boost::io::str(
				boost::wformat(L"%s /command:pull /path:\"%s\" /closeonend:0") % TGitExePath::get().GetPath() % m_wstrGitAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
	};

	// --------------------------------------
	// Push 다이얼로그.
	// --------------------------------------
	class OpenPushDlg : public ProcessCommand
	{
	public:
		OpenPushDlg(const std::wstring& wstrGitAbsPath) : m_wstrGitAbsPath(wstrGitAbsPath) {}
		virtual ~OpenPushDlg() {}

	private:
		virtual std::wstring MakeCommandLine()
		{
			return boost::io::str(
				boost::wformat(L"%s /command:push /path:\"%s\" /closeonend:0") % TGitExePath::get().GetPath() % m_wstrGitAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
	};

	// --------------------------------------
	// Add
	// --------------------------------------
	class Add : public ProcessCommand
	{
	public:
		Add(const std::wstring& wstrGitAbsPath, const std::wstring& wstrAddTargetRelPath)
			: m_wstrGitAbsPath(wstrGitAbsPath) {}
		virtual ~Add() {}

	private:
		virtual std::wstring MakeCommandLine()
		{
			const std::wstring wstrAddTargetAbsPath =
				boost::io::str(boost::wformat(L"%s\\%s") % m_wstrGitAbsPath % m_wstrAddTargetRelPath);

			return boost::io::str(
				boost::wformat(L"%s /command:add /path:\"%s\"") % TGitExePath::get().GetPath() % m_wstrGitAbsPath % wstrAddTargetAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
		const std::wstring m_wstrAddTargetRelPath;
	};

	// --------------------------------------
	// Commit 다이얼로그.
	// --------------------------------------
	class OpenCommitDlg : public ProcessCommand
	{
	public:
		OpenCommitDlg(const std::wstring& wstrGitAbsPath) : m_wstrGitAbsPath(wstrGitAbsPath) {}
		virtual ~OpenCommitDlg() {}

	private:
		virtual std::wstring MakeCommandLine()
		{
			return boost::io::str(
				boost::wformat(L"%s /command:commit /path:\"%s\" /closeonend:0") % TGitExePath::get().GetPath() % m_wstrGitAbsPath);
		}

		const std::wstring m_wstrGitAbsPath;
	};
}