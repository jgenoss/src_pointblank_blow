#pragma once

#include "resource.h"
#include "afxcmn.h"
// cDlgProgress 대화 상자입니다.

class cDlgProgress : public CDialog
{
	DECLARE_DYNAMIC(cDlgProgress)

public:
	cDlgProgress(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~cDlgProgress();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_PROGRESS };

	void InitProgress(const INT32& nMaxCnt);
	INT32 m_nMaxCnt;
	INT32 m_nReadCnt;
	HANDLE	m_hExit;
	CWinThread*		m_pLoadingThread;

	void SetMaxCnt(const INT32& nMaxCnt) { m_nMaxCnt = nMaxCnt; }
	void SetCurrCnt(const INT32& nReadCnt) { m_nReadCnt = nReadCnt; }

	bool	StartLoading();
	bool	LoopLoading();
	bool	EndLoading();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	CProgressCtrl m_progress_loading;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
