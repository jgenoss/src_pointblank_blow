#if defined( I3_WINDOWS)

#pragma once

class i3Profile
{
private:
	CWinApp *m_App;
	CWnd *m_Wnd;

	CString m_SectionName;
	CString m_PrevSectionName;

public:
	i3Profile(CWnd *wnd);
	virtual ~i3Profile();

	LPCTSTR SetSection(LPCTSTR Name);
	LPCTSTR GetSection();

	void LoadWindowPos(BOOL bNoSize = TRUE);
	void SaveWindowPos();

	UINT GetInt(LPCTSTR szEntry, int nDefault);
	CString GetString(LPCTSTR szEntry, LPCTSTR szDefault);
	
	void WriteInt(LPCTSTR szEntry, int nValue);
	void WriteString(LPCTSTR szEntry, LPCTSTR szValue);
};

#endif