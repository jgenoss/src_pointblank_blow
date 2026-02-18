// DlgUpdate.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "ControlTool.h"
#include "DlgUpdate.h"


// CDlgUpdate 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgUpdate, CDialog)

CDlgUpdate::CDlgUpdate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUpdate::IDD, pParent)
{
	m_i32CurProgressInc			= 0;
	m_i32TotalProgressInc		= 0;
	m_bCreate					= FALSE;

	m_hThread					= INVALID_HANDLE_VALUE;
	m_ThreadID					= 0;
	m_bRunning					= TRUE;
	m_bUpdateDone				= FALSE;
}

CDlgUpdate::~CDlgUpdate()
{
	if(m_hThread != INVALID_HANDLE_VALUE)
	{
		/*DWORD dwExitCode;

		GetExitCodeThread(m_hThread, &dwExitCode);

		if(dwExitCode == STILL_ACTIVE)
			TerminateThread(m_hThread, 0);

		CloseHandle(m_hThread);*/

		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
}

void CDlgUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CURRENT_UPDATE, m_ctrlCurUpdate);
	DDX_Control(pDX, IDC_PROGRESS_CURRENT, m_ctrlCurProgress);
	DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_ctrlTotalProgress);
	DDX_Control(pDX, IDOK, m_btOk);
	DDX_Control(pDX, IDC_UPDATE_DISPLAY, m_ctrlDisplayRun);
	DDX_Control(pDX, IDC_TOTAL_UPDATE, m_ctrlTotalUpdate);
}


BEGIN_MESSAGE_MAP(CDlgUpdate, CDialog)
END_MESSAGE_MAP()


// CDlgUpdate 메시지 처리기입니다.

BOOL CDlgUpdate::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ctrlCurProgress.SetRange(0, MAX_PROGRESS_GAUGE);
	m_ctrlCurProgress.SetPos(0);
	m_ctrlTotalProgress.SetRange(0, MAX_PROGRESS_GAUGE);
	m_ctrlTotalProgress.SetPos(0);

	m_bCreate = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CDlgUpdate::StartUpdate()
{
	m_btOk.EnableWindow(FALSE);
	m_hThread = (HANDLE)CreateThread(0, 0, ProgressWorking, (LPVOID)this, 0, &m_ThreadID);
}


void CDlgUpdate::IncCurProgressBar(INT32 i32IncCurrent, INT32 i32IncTotal)
{
	m_i32CurProgressInc = i32IncCurrent;
	m_i32TotalProgressInc = i32IncTotal;
	RedrawWindow();
}



DWORD WINAPI CDlgUpdate::ProgressWorking(LPVOID lpParam)
{
	INT32	i32Cur = 0;
	INT32	i32Count = 0;

	INT32	i32CurProgressInc_old = 0;
	INT32	i32TotalProgressInc_old = 0;

	BOOL	bRunning = TRUE;

	CDlgUpdate* pUpdate =  (CDlgUpdate*)lpParam;

	TCHAR cDispRunning[MAX_RUN_DISPLAY][6] = 
	{
		{_T(">----")}, 
		{_T("->---")},
		{_T("-->--")},
		{_T("--->-")},
		{_T("---->")}
	};
	
	while(bRunning)
	{
		pUpdate->m_ctrlDisplayRun.SetWindowText( cDispRunning[i32Cur] );
		pUpdate->m_ctrlDisplayRun.RedrawWindow();
		i32Count++;

		if(i32Count == 10)
		{
			i32Cur++;

			if(MAX_RUN_DISPLAY == i32Cur)
				i32Cur = 0;

			i32Count = 0;
		}

		if(i32CurProgressInc_old != pUpdate->m_i32CurProgressInc)
		{
			i32CurProgressInc_old++;

			if(i32CurProgressInc_old == MAX_PROGRESS_GAUGE)
				i32CurProgressInc_old = 0;
		}

		if(i32TotalProgressInc_old != pUpdate->m_i32TotalProgressInc)
			i32TotalProgressInc_old++;

		if(i32TotalProgressInc_old == MAX_PROGRESS_GAUGE)
		{
			bRunning = FALSE;
			pUpdate->m_bUpdateDone = TRUE;
			pUpdate->m_btOk.EnableWindow(TRUE);

			ExitThread(pUpdate->m_ThreadID);
		}

		pUpdate->m_ctrlCurProgress.SetPos(i32CurProgressInc_old);
		pUpdate->m_ctrlTotalProgress.SetPos(i32TotalProgressInc_old);

		Sleep(5);
	}

	return 0;
}
