#pragma once
#include "resource.h"
class CDlgProject : public CDialog
{
	DECLARE_DYNAMIC(CDlgProject)
	DECLARE_MESSAGE_MAP()

	CEdit m_EditQA;
	CEdit m_EditLive;
	CEdit m_EditName;
	CButton m_ButtonQA;
	CButton m_ButtonLive;

	char m_chProjectName[MAX_PATH];
	char m_chQA[MAX_PATH];
	char m_chLive[MAX_PATH];
public:
	CDlgProject(const char* chName, const char* chQAPath, const char* chLivePath, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProject();

	// Dialog Data
	enum { IDD = IDD_DLG_NEW_PROJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnInitDialog() override;

public:
	afx_msg void OnBnClickedButtonQa();
	afx_msg void OnBnClickedButtonLive();
	afx_msg void OnBnClickedOk();

	void SetName(const char* str) { m_EditName.SetWindowText(str); }
	void SetQAPath(const char* str) { m_EditQA.SetWindowText(str); }
	void SetLivePath(const char* str) { m_EditLive.SetWindowText(str); }

	const char* GetQAPath();
	const char* GetLivePath();
	const char* GetName();

};
