// FileTreeDialogDlg.h : header file
//



#if !defined(AFX_FILETREEDIALOGDLG_H__85D56C0C_751B_49F5_8A92_7815BCD7C20C__INCLUDED_)
#define AFX_FILETREEDIALOGDLG_H__85D56C0C_751B_49F5_8A92_7815BCD7C20C__INCLUDED_

//#if _MSC_VER > 1000
#pragma once
//#endif // _MSC_VER > 1000

#include "afxcmn.h"
#include "FileTreeCtrl.h"
#include "Resource.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CPanelTreeScript dialog

class CPanelTreeScript : public CDialog
{
	DECLARE_DYNAMIC(CPanelTreeScript)

// Construction
public:
	CPanelTreeScript(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPanelTreeScript();

// Dialog Data
	//{{AFX_DATA(CPanelTreeScript)
	enum { IDD = IDD_FILETREEDIALOG_DIALOG };
	CFileTreeCtrl	m_FileTree;
	CString	m_strText;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanelTreeScript)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
	virtual void OnOK() override {}
	virtual void OnCancel() override {}

	
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPanelTreeScript)
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual BOOL OnInitDialog() override;
	virtual void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRefresh();
	afx_msg void OnStateChanged(NMHDR* pNMHDR, LRESULT* pResult);
	
	UINT64 GetCommandLine();
	void GetWindowsize(RECT* prect, const RECT* pRect1, const RECT* pRect2, INT32 value);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EdWorkDir;
	CEdit m_EdLiveDir;
	CEdit m_EdScriptDir;
	CListBox m_LFileList;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnSetPack();
	afx_msg void OnBnClickedRadio1();
	void OnGetlist();
	afx_msg void OnGetlist2();
	CButton m_SelectScript;
	CProgressCtrl m_progressCtrl;
	
	afx_msg void OnBnClickedRadio2();
	CEdit m_PackLogEditbox;
	//afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);

	void SetPath(const char* chQAPath, const char* chLivePath);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETREEDIALOGDLG_H__85D56C0C_751B_49F5_8A92_7815BCD7C20C__INCLUDED_)
