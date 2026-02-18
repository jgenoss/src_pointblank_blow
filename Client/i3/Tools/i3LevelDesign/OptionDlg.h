#if !defined( _COPTION_DIALOG_H__)
#define _COPTION_DIALOG_H__

// COptionDlg dialog
#include "afxwin.h"

class COptionDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionDlg)

public:
	COptionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionDlg();

// Dialog Data
	enum { IDD = IDD_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	CEdit m_edWorkDir;
	CButton m_btnWorkDir;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnBnClickedBtnWorkingdir();
	CEdit m_ResPathCtrl;
	afx_msg void OnBnClickedBtnRespath();
	CEdit m_ED_SnapPos;
	CEdit m_ED_SnapRotate;
	CEdit m_ED_SnapScale;
};

#endif