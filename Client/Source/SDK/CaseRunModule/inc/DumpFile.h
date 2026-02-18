#pragma once

namespace CaseRun
{
	class DumpFile
	{
	protected:
		wstring		m_strPath;
		wstring		m_strDate;

		wstring		m_DumpFile;
		wstring		m_ReportFile;
		wstring		m_CaptureFile;

		wstring		m_ZipFile;

	protected:
		bool	CreateDump(struct _EXCEPTION_POINTERS * pInfo);
		bool	Analyze(void);
		bool	CaptureScreen(void);

		bool	Zip(void);

	public:
		DumpFile(void);

		const wstring &		getZipFile(void) const { return m_ZipFile; }

		bool	Create(struct _EXCEPTION_POINTERS * pInfo);
		bool	Analyze(const wstring & path);
	};
}