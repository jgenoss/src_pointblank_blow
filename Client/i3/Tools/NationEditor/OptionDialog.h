#pragma once


// COptionDialog dialog

class COptionDialog : public CDialog
{
	DECLARE_DYNAMIC(COptionDialog)

public:
	COptionDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionDialog();

// Dialog Data
	enum { IDD = IDD_OPTION_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	CEdit m_EditCtrl_WorkMediaPath;

public:
	afx_msg void OnBnClickedPath();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
//	void SetPath(const CString& Path);
//	CString GetPath() const;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
