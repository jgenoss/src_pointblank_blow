#pragma once


// CDragDropOptionDlg dialog
#include "resource.h"

class CDragDropOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CDragDropOptionDlg)

public:
	CDragDropOptionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDragDropOptionDlg();

// Dialog Data
	enum { IDD = IDD_DRAGDROP_OPTION };

	INT32 m_nSelectedItem;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_opOverWrite;
	int m_opAddSub;
	int m_opCancel;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;
public:
	virtual BOOL OnInitDialog() override;
};
