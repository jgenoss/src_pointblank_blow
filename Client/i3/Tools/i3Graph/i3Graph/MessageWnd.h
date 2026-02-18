#pragma once
#include "afxcmn.h"

#define I3GRAP_LOG_MAX			100
//CMessageWnd dialog

class CMessageWnd : public CDialog
{
	DECLARE_DYNAMIC(CMessageWnd)

public:
	CMessageWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMessageWnd();

// Dialog Data
	enum { IDD = IDD_DIALOG_MESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); 

	void	UpdateLog(INT32 Idx, char * pString); 
	DECLARE_MESSAGE_MAP()
public:

	
	void	SetMessage(INT32 LogLevel, char * pString); 
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListCtrl m_ListControl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
