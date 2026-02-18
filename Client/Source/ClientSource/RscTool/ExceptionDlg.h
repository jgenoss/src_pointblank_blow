#pragma once


// ExceptionDlg 대화 상자입니다.
#include "PackScriptContents.h"

class ExceptionDlg : public CDialog
{
	DECLARE_DYNAMIC(ExceptionDlg)

public:
	ExceptionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ExceptionDlg();
	
	void SetExceptionFileString(const PackScriptExceptionString& str);
	const PackScriptExceptionString& GetExceptionFileString() const { return m_exception_copied_string; }
	

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EXCEPTIONDLG };

private:

	CEdit			m_edit_exception;
	CEdit			m_edit_force_copy_to_qapath;
	
	PackScriptExceptionString	m_exception_copied_string;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
	
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
