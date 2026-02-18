#pragma once
#include "afxwin.h"


// CPaneCommon dialog

class CPaneCommon : public CDialog
{
	DECLARE_DYNAMIC(CPaneCommon)

public:
	CPaneCommon(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneCommon();

// Dialog Data
	enum { IDD = IDD_PANECOMMON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_PathCtrl;
};
