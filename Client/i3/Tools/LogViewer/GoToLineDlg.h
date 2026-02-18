#pragma once
#include "resource.h"

class CGoToLineDlg : public CDialog
{
	DECLARE_DYNAMIC(CGoToLineDlg)

public:
	CGoToLineDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGoToLineDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_GOTOLINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CStatic m_st_GoToLine;
	CEdit m_ed_GoToLine;
	char	pszStatic[64];
	char	pszEdit[64];

public:
	void			SetStaticText( const char* pszText);
	void			SetEditText( const char* pszText);
	const char*		GetEditText( void);
	
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
