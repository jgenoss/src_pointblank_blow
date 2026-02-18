#pragma once
#include "resource.h"

// Dlg_Find dialog
class CScriptWnd;
class CFindDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindDlg)
	CScriptWnd* m_pWnd;
public:
	CFindDlg(CScriptWnd* pWnd, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFindDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_FIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();

public:
	void		SetFindWhat( const char* pszFindWhat);
	const char* GetFindWhat( void);

	bool		IsMatchCase( void)			{	return m_bMatchCase;	}
	bool		IsMatchWholeWord( void)		{	return m_bMatchWholeWord;	}
	bool		IsSearchUp( void)			{	return m_bSearchUp;		}

	void		SetMatchCase( bool bTrue)			{	m_bMatchCase = bTrue;	}
	void		SetMatchWholeWord( bool bTrue)		{	m_bMatchWholeWord = bTrue;	}
	void		SetSearchUp( bool bTrue)			{	m_bSearchUp = bTrue;	}
protected:
	CButton m_chk_MatchCase;
	CButton m_chk_MatchWholeWord;
	CButton m_chk_SearchUp;

protected:

	CComboBox		m_cb_FindWhat;
	char			m_pFindWhat[MAX_PATH];
	bool			m_bMatchCase;
	bool			m_bMatchWholeWord;
	bool			m_bSearchUp;


};
