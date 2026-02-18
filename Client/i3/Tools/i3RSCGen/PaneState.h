#pragma once
#include "afxcmn.h"
#include "RSCNode.h"
#include "afxwin.h"

// CPaneState dialog

class CPaneState : public CDialog
{
	DECLARE_DYNAMIC(CPaneState)

public:
	CPaneState(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneState();

// Dialog Data
	enum { IDD = IDD_PANE_STATUS };

public:
	void	ClearLog( void);
	void	Log( char * pszMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;

public:
	i3TDKLogCtrl m_ctrStateLog;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
