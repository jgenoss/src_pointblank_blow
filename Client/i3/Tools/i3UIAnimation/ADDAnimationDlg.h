#pragma once
#include "resource.h"
class CADDAnimationDlg : public CDialog
{
	DECLARE_DYNAMIC(CADDAnimationDlg)

	CListBox m_Listbox;
public:
	CADDAnimationDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CADDAnimationDlg();

	// 대화 상자 데이터입니다.
	enum {
		IDD = IDD_ANIMATION_ADD
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP();

public:
	char		m_chTitle[MAX_PATH];
	REAL32		m_fLifeTime;
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;

	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
};

