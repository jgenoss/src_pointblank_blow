#pragma once
#include "afxcmn.h"


// CPanel_Palette dialog

class CPanel_Palette : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanel_Palette)

protected:
	HTREEITEM		m_hRoot[ i3SceneOptimizer::CLASS_MAX];

public:
	CPanel_Palette(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_Palette();

	void	StartProcessing(void);
	void	EndProcessing(void);

// Dialog Data
	enum { IDD = IDD_PANEL_OPT_PALETTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_TREE_OPT;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMDblclkTreeOpt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindragTreeOpt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnGetInfoTipTreeOpt(NMHDR *pNMHDR, LRESULT *pResult);
};
