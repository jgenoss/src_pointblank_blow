// i3AnimOptimizerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3AnimOptimizer.h"
#include "i3AnimOptimizerDlg.h"
#include ".\i3animoptimizerdlg.h"
#include "GlobVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3AnimOptimizerDlg 대화 상자



Ci3AnimOptimizerDlg::Ci3AnimOptimizerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3AnimOptimizerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pViewer = NULL;
}

void Ci3AnimOptimizerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Ci3AnimOptimizerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// Ci3AnimOptimizerDlg 메시지 처리기

BOOL Ci3AnimOptimizerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	{
		CRect rect;

		GetClientRect( &rect);

		m_Splitter0.Create( I3_SPLS_EX_VERT, WS_VISIBLE | WS_CHILD, rect, 10, this, 100);
		m_Splitter1.Create( 0, WS_VISIBLE | WS_CHILD, rect, 10, &m_Splitter0, 101);
		m_Splitter2.Create( 0, WS_VISIBLE | WS_CHILD, rect, 10, &m_Splitter0, 102);
		m_Splitter3.Create( 0, WS_VISIBLE | WS_CHILD, rect, 10, &m_Splitter2, 103);

		m_PaneView.Create( CPaneView::IDD, &m_Splitter1);
		m_PaneBone.Create( CPaneBone::IDD, &m_Splitter1);
		m_PaneTrack.Create( CPaneTrack::IDD, &m_Splitter3);
		m_PaneSpec.Create( CPaneSpec::IDD, &m_Splitter3);
		m_PaneAnim.Create( CPaneAnim::IDD, &m_Splitter2);

		m_Splitter0.AddPane( &m_Splitter1);
		m_Splitter0.AddPane( &m_Splitter2);

		m_Splitter1.AddPane( &m_PaneView);
		m_Splitter1.AddPane( &m_PaneBone);

		m_Splitter2.AddPane( &m_PaneAnim);
		m_Splitter2.AddPane( &m_Splitter3);
		
		m_Splitter3.AddPane( &m_PaneTrack);
		m_Splitter3.AddPane( &m_PaneSpec);

		m_Splitter0.LoadWindowInfo( "i3AnimOptimizer.CFG");
		m_Splitter0.Rearrange();
	}
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void Ci3AnimOptimizerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR Ci3AnimOptimizerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3AnimOptimizerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_Splitter0.m_hWnd))
	{
		m_Splitter0.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Ci3AnimOptimizerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	{
		m_Splitter0.SaveWindowInfo( "i3AnimOptimizer.CFG");
	}

	I3_SAFE_RELEASE( m_pViewer);
}

void Ci3AnimOptimizerDlg::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1], Ext[32];
	char szTempPath[MAX_PATH], * pPath;
	char szName[MAX_PATH];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		i3String::SplitFileExt( TempName, Ext, sizeof(Ext) - 1);
		i3String::Copy( szTempPath, TempName);

		pPath = i3String::SplitFileName( szTempPath, szName, FALSE);
		*pPath = 0;

		if( i3String::Compare( Ext, "I3S") == 0)
		{
			g_Project.LoadScene( TempName);
		}
		else if( i3String::Compare( Ext, "I3A") == 0)
		{
			g_Project.LoadAnim( TempName);
		}
	}

	DragFinish( hDropInfo);
}
