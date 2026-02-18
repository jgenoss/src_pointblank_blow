// DlgProgress.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgProgress.h"


// cDlgProgress 대화 상자입니다.

IMPLEMENT_DYNAMIC(cDlgProgress, CDialog)

cDlgProgress::cDlgProgress(CWnd* pParent /*=NULL*/)
	: CDialog(cDlgProgress::IDD, pParent)
	, m_nMaxCnt(0)
	, m_nReadCnt(0)
{

}

cDlgProgress::~cDlgProgress()
{
}

void cDlgProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress_loading);
}


BEGIN_MESSAGE_MAP(cDlgProgress, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// cDlgProgress 메시지 처리기입니다.
UINT _ProgressThread(void* obj) 
{
	CProgressCtrl* pProgress = (CProgressCtrl*)obj;
	if (pProgress)
	{
		pProgress->SetPos(0);
	}
	return 0;	
}
void cDlgProgress::InitProgress(const INT32& nMaxCnt)
{
	//SetTimer(0, 100, NULL); // 1/10 second
	m_nMaxCnt = nMaxCnt;
	m_progress_loading.SetRange(0,(short)nMaxCnt);
	m_pLoadingThread = AfxBeginThread( _ProgressThread, (void*)&m_progress_loading );
}
BOOL cDlgProgress::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}


bool	cDlgProgress::StartLoading()
{
	m_hExit = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	return true;
}

bool	cDlgProgress::LoopLoading()
{
	if( WAIT_TIMEOUT != ::WaitForSingleObject( m_hExit, 0 ) )
	{
		return false;
	}
	//i3UIManager* pUI = i3UI::getManager();
	//INT32 nReadCnt = pUI->GetReadCnt();

	m_progress_loading.SetPos(0);

	return true;
}


bool	cDlgProgress::EndLoading()
{
	m_pLoadingThread = NULL;
	RedrawWindow();
	return true;
}

void cDlgProgress::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}


LRESULT cDlgProgress::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CDialog::WindowProc(message, wParam, lParam);
}