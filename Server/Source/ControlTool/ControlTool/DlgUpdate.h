#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define MAX_RUN_DISPLAY			5
#define MAX_PROGRESS_GAUGE		100

// CDlgUpdate 대화 상자입니다.

class CDlgUpdate : public CDialog
{
	DECLARE_DYNAMIC(CDlgUpdate)

public:
	CDlgUpdate(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgUpdate();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_UPDATE };

protected:
	HANDLE			m_hThread;
	DWORD			m_ThreadID;
	BOOL			m_bRunning;

	INT32			m_i32CurProgressInc;
	INT32			m_i32TotalProgressInc;

	BOOL			m_bCreate;
	BOOL			m_bUpdateDone;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit					m_ctrlDisplayRun;
	CEdit					m_ctrlCurUpdate;
	CProgressCtrl			m_ctrlCurProgress;
	CProgressCtrl			m_ctrlTotalProgress;
	CButton					m_btOk;

	virtual BOOL			OnInitDialog();

	BOOL					IsCreate()			{ return m_bCreate; }
	BOOL					IsUpdated()			{ return m_bUpdateDone; }
	void					StartUpdate();

	void					IncCurProgressBar(INT32 i32IncCurrent, INT32 i32IncTotal);
	static DWORD WINAPI		ProgressWorking(LPVOID lpParam);
	
	CEdit m_ctrlTotalUpdate;
};
