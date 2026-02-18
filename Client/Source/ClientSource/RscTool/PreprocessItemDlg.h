#pragma once


// PreprocessItemDlg 대화 상자입니다.
#include "resource.h"

class PreprocessItemDlg : public CDialog
{
	DECLARE_DYNAMIC(PreprocessItemDlg)

public:
	PreprocessItemDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~PreprocessItemDlg();

	void	SetText(const wchar_t* str);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PREPROCESS_ITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	CStatic		m_Text;

	DECLARE_MESSAGE_MAP()
};
