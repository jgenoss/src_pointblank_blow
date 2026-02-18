#pragma once
#include "afxcmn.h"
#include "ListAIDriver.h"
#include "AIDriverManager.h"
#include "afxwin.h"
// cDlgAIEnumList 대화 상자입니다.

class cDlgAIEnumList : public CDialog
{
	DECLARE_DYNAMIC(cDlgAIEnumList)

public:
	cDlgAIEnumList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~cDlgAIEnumList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AI_ENUM_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

private:
	mAIDriverCon	m_mAIDriverData;
	vAIEnumCon		m_vAIEnumData;

	int m_nSelectIndex;
public:
	void SetAIDriverData(mAIDriverCon* pAIDriverCon);
	void SetAIEnum(vAIEnumCon* pvAIEnum);

public:
	cListAIDriver m_list_ai_enum;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListAiEnumList(NMHDR *pNMHDR, LRESULT *pResult);
	
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CEdit m_edit_enum_explain;
	cListAIDriver m_list_enum_current_weapon;
	afx_msg void OnBnClickedButtonAddEnum();
};
