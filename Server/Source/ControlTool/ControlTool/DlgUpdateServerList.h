#pragma once
#include "afxwin.h"
#include "afxcmn.h"


#define SERVICE_NAME_COUNT		8

// CUpdateServerList 대화 상자입니다.

class CDlgUpdateServerList : public CDialog
{
	DECLARE_DYNAMIC(CDlgUpdateServerList)

public:
	CDlgUpdateServerList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgUpdateServerList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_UPDATE_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	CEdit				m_strUpdateRun;
	INT32				m_i32ServerCount;

	INT32	*			m_i32UpdatePercent;

public:
	CButton				m_btStart;
	CButton				m_btFinish;
	CButton				m_btCancel;
	CListCtrl			m_ctrlServerList;

	HANDLE				m_hThread;
	DWORD				m_ThreadID;
	BOOL				m_bRunning;

	CProgressCtrl		m_pProg[SERVICE_NAME_COUNT];
	
	TCHAR				m_strServerServiceName[SERVICE_NAME_COUNT][SERVICE_NAME_SIZE];
	
	afx_msg void		OnBnClickedCancle();
	afx_msg void 		OnBnClickedUpdateStart();
	afx_msg void 		OnBnClickedButtonAllcheck();
	afx_msg void 		OnBnClickedButtonUncheck();

	virtual BOOL		OnInitDialog();
	
	static DWORD WINAPI		UpdateThread(LPVOID lpParam);
	
	CButton m_btAllCheck;
	CButton m_btUncheck;
};
