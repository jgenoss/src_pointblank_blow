#pragma once


// COptimizeWnd dialog

class COptimizeWnd : public CDialog
{
	DECLARE_DYNAMIC(COptimizeWnd)

public:
	COptimizeWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptimizeWnd();

// Dialog Data
	enum { IDD = IDD_DIALOG_OPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
