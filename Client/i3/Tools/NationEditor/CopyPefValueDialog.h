#pragma once
#include "afxwin.h"


// CClonePefValueDialog 대화 상자입니다.

class CClonePefValueDialog : public CDialog
{
	DECLARE_DYNAMIC(CClonePefValueDialog)

public:
	CClonePefValueDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CClonePefValueDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_COPY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CHListBox m_PefList;
	CHListBox m_SrcList;
	CHListBox m_DestList;
	CHListBox m_CloneItemList;
	CEdit m_CloneItem;

	virtual BOOL OnInitDialog() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

public:
	void SetPefList(PefContextBank* pPefContextBank)	{	m_pPefContextBank = pPefContextBank;	}
	void SetNationList(StringVector* pNationBank)	{	m_pNationBank = pNationBank;	}

private:
	PefContextBank* m_pPefContextBank;
	StringVector* m_pNationBank;

private:
	void InitPefList();
	void InitNationList();
public:
	afx_msg void OnBnClickedOk();
};
