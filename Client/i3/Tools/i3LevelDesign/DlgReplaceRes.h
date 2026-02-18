#pragma once
#include "afxwin.h"


// CDlgReplaceRes dialog

class CDlgReplaceRes : public CDialog
{
	DECLARE_DYNAMIC(CDlgReplaceRes)
protected:
	i3LevelRes *		m_pDestRes;
	i3LevelRes *		m_pSrcRes;

public:
	CDlgReplaceRes(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReplaceRes();

	bool	Execute( i3LevelRes * pSrcRes);

	i3LevelRes *	getDestRes(void)			{ return m_pDestRes; }

// Dialog Data
	enum { IDD = IDD_DLG_REPLACE_RES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Src;
	CEdit m_ED_Dest;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
