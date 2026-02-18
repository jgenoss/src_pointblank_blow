#pragma once
#include "afxwin.h"


// CProp_LvElm_PathLink dialog

class CProp_LvElm_PathLink : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvElm_PathLink)

protected:
	i3LevelPathLink *		m_pLink;

public:
	virtual void	SetObject( i3ElementBase * pObj) override;

public:
	CProp_LvElm_PathLink(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvElm_PathLink();

// Dialog Data
	enum { IDD = IDD_PROP_LVELM_PATHLINK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_BiDir;
	CEdit m_ED_Cost;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnBnClickedChkBidir();
	afx_msg void OnEnKillfocusEdCost();
};
