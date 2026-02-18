#pragma once


// CFolderDlg dialog

class CFolderDlg : public CDialog
{
	DECLARE_DYNAMIC(CFolderDlg)

public:
	CFolderDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFolderDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_FOLDER };

	CString m_Title;
	CString m_Dir;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	virtual BOOL OnInitDialog();
	void SetTitle( const char *szTitle ){ m_Title = szTitle; }
	void SetDir( const char * szDir ){ m_Dir = szDir; }
	const char* GetDir(){ return LPCTSTR( m_Dir ); }

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonSetFolder();
	afx_msg void OnBnClickedBtnAmbient();
};
