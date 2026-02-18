#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// i3TDKDlgEnvSet dialog

class I3_EXPORT_TDK i3TDKDlgEnvSet : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlgEnvSet)

protected:
	INT32	m_idxSelProject = -1;

protected:
	void	_UpdateAll(void);
	void	_UpdateProject( INT32 idx);

	INT32	_getSelectedIndex(void);
	void	_SelectProject( INT32 idx);

	void	OnNewProject(void);
	void	OnDelete(void);
	void	OnSetActiveProject(void);

public:
	i3TDKDlgEnvSet(CWnd* pParent = nullptr) : CDialog(i3TDKDlgEnvSet::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlgEnvSet() {}

// Dialog Data
	enum { IDD = IDD_TDK_ENVSET };

	bool	Execute(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_LV_Project;
	CEdit m_ED_WorkDir;
	CRichEditCtrl m_ED_Desc;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMRclickTdkProjectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTdkProjectList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusTdkEdWorkdir();
	afx_msg void OnEnKillfocusTdkRichedDesc();
	afx_msg void OnNMDblclkTdkProjectList(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnOK();
};
