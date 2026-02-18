#pragma once

#if defined( I3_DEBUG)
#include "resource.h"

// CDebugPanel dialog

class CDebugPanel : public CDialog
{
	DECLARE_DYNAMIC(CDebugPanel)

public:
	CDebugPanel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDebugPanel();

// Dialog Data
	enum { IDD = IDD_DEBUG_PANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnVram();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnRam();
	afx_msg void OnBnClickedBtnGamegraph();
};

#endif
