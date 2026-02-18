#pragma once
// OptionDlg dialog

class OptionDlg : public CDialog
{
	DECLARE_DYNAMIC(OptionDlg)

public:
	OptionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~OptionDlg();

// Dialog Data
	enum { IDD = IDD_OPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrlWorkDir;
	CEdit m_CtrlExceptRule;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedBtnSeldir();
protected:
	virtual void OnOK() override;
};
