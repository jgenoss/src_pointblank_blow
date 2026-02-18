// i3TDKProfilingViewerDialogDlg.cpp : 구현 파일
//

#include "stdafx.h"

#if !defined( I3_NO_PROFILE)

#include "i3TDKProfilingViewerDialog.h"
#include "resource.h"


// i3TDKProfilingViewerDialog 대화 상자

i3TDKProfilingViewerDialog::~i3TDKProfilingViewerDialog( void)
{
	if( m_pProfileCtrl != nullptr)
	{
		delete m_pProfileCtrl;
		m_pProfileCtrl = nullptr;
	}
}

void i3TDKProfilingViewerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(i3TDKProfilingViewerDialog, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// i3TDKProfilingViewerDialog 메시지 처리기

BOOL i3TDKProfilingViewerDialog::OnInitDialog()
{
	CDialog::OnInitDialog();


	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}
/*
BOOL i3TDKProfilingViewerDialog::Create( CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::Create( i3TDKProfilingViewerDialog::IDD, pParentWnd);
}*/


int i3TDKProfilingViewerDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	RECT rt;
	GetClientRect( &rt);

	if( m_pProfileCtrl == nullptr)
	{
		m_pProfileCtrl = new i3TDKProfilingViewerCtrl;
		m_pProfileCtrl->Create( WS_CHILD | WS_VISIBLE, rt, this, IDD_PROFILING_VIEWER);

		//	기본적으로 1초에 한번씩 업데이트 하도록
		m_pProfileCtrl->SetUpdateTime( 1000);
	}

	return 0;
}

void i3TDKProfilingViewerDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if( m_pProfileCtrl != nullptr)
	{
		if( ::IsWindow( m_pProfileCtrl->GetSafeHwnd()))
			m_pProfileCtrl->SetWindowPos( nullptr, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

LRESULT i3TDKProfilingViewerDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	LRESULT ret = CDialog::WindowProc(message, wParam, lParam);

	switch( message)
	{
	case WM_KEYDOWN:
		{
			if( wParam)
			{
				case VK_ESCAPE:
					{
						delete this;
					}
					break;
			}
		}
		break;
	case WM_DESTROY:
	case WM_CLOSE:
	case WM_QUIT:
		{
			delete this;
		}
		break;
	}

	return ret;
}

void i3TDKProfilingViewerDialog::OnOK()
{
	CDialog::OnOK();
}

#endif
