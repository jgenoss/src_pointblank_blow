#pragma once
#include "afxwin.h"


// Dlg_GoToLine dialog

class Dlg_GoToLine : public CDialog
{
	DECLARE_DYNAMIC(Dlg_GoToLine)

public:
	Dlg_GoToLine(CWnd* pParent = NULL);   // standard constructor
	virtual ~Dlg_GoToLine();

// Dialog Data
	enum { IDD = IDD_DLG_GOTOLINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

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
	
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
