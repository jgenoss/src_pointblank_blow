#pragma once
#include "afxwin.h"
#include "resource.h"

// Dlg_Find dialog

class Dlg_Find : public CDialog
{
	DECLARE_DYNAMIC(Dlg_Find)

public:
	Dlg_Find(CWnd* pParent = NULL);   // standard constructor
	virtual ~Dlg_Find();

// Dialog Data
	enum { IDD = IDD_DLG_FIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;

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
	i3::vector<i3::rc_string> m_FoundList;
	char			m_pFindWhat[MAX_PATH];
	bool			m_bMatchCase;
	bool			m_bMatchWholeWord;
	bool			m_bSearchUp;

};
