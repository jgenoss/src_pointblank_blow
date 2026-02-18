#pragma once
#include "afxwin.h"


// CPanelProperty dialog

class CPanelProperty : public CDialog
{
	DECLARE_DYNAMIC(CPanelProperty)
protected:
	i3PackNode *			m_pPackNode;
	I3_PACK_FILE_REF *		m_pFile;

protected:
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void		setFile( I3_PACK_FILE_REF * pFile);

public:
	CPanelProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelProperty();

// Dialog Data
	enum { IDD = IDD_PANEL_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Security;
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedChkSecurity();
	afx_msg void OnBnClickedButton1();
	CEdit m_EdWorkDir;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CEdit m_EdLiveDir;
	CEdit m_EdScriptDir;
};
