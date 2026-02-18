#pragma once
#include "afxcmn.h"
#include "i3TDKOptSpecBase.h"
#include "afxwin.h"
#include "resource.h"

// i3TDKDlgRename dialog

class I3_EXPORT_TDK i3TDKDlgRename : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKDlgRename)

public:
	i3TDKDlgRename(CWnd* pParent = nullptr) : i3TDKOptSpecBase(IDD_TDK_DLG_RENAME, pParent) {}   // standard constructor
	virtual ~i3TDKDlgRename() {}

	enum { IDD = IDD_TDK_DLG_RENAME };

protected:
	CEdit			m_NameCtrl;
	const TCHAR*	m_pszHelpText = nullptr;

#ifdef _UNICODE
	i3::rc_wstring	m_strName;
#else
	i3::rc_string	m_strName;
#endif

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	const TCHAR *			getName(void) const			{ return m_strName.c_str(); }

#ifdef _UNICODE
	const i3::rc_wstring&	getNameString() const		{ return m_strName; }
	void setName(const i3::rc_wstring& strName) { m_strName = strName; }
#else
	const i3::rc_string&	getNameString() const		{ return m_strName; }
	void setName(const i3::rc_string& strName) { m_strName = strName; }
#endif
	void			setName( const TCHAR* pszName)	{ m_strName = pszName; }
	
	BOOL			Execute( const TCHAR* pszHelp, const TCHAR* pszInitName);

	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CStatic m_HelpTextCtrl;
};
