// PanelTimeTrack.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "PanelTimeTrack.h"
#include ".\paneltimetrack.h"


// CPanelTimeTrack dialog

IMPLEMENT_DYNAMIC(CPanelTimeTrack, CDialog)
CPanelTimeTrack::CPanelTimeTrack(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelTimeTrack::IDD, pParent)
{
	m_pAIState = NULL;
}

CPanelTimeTrack::~CPanelTimeTrack()
{
}

void CPanelTimeTrack::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CPanelTimeTrack::SetAIState( GICShapeAI * pShape)
{
	m_pAIState = pShape;

	m_TimeSeqCtrl.SetShape( pShape);
}

BEGIN_MESSAGE_MAP(CPanelTimeTrack, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanelTimeTrack message handlers

BOOL CPanelTimeTrack::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rt;

		GetClientRect( &rt);

		m_TimeSeqCtrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 100);
	}

	i3TDK::RegisterUpdate( m_hWnd, GICShapeAI::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelTimeTrack::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TimeSeqCtrl.m_hWnd))
	{
		m_TimeSeqCtrl.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelTimeTrack::OnCancel()
{
}

void CPanelTimeTrack::OnOK()
{
}

void CPanelTimeTrack::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( i3::kind_of<GICShapeAI*>(pInfo->m_pMeta)) //->IsTypeOf( GICShapeAI::static_meta()))
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
		{
			SetAIState( (GICShapeAI *) pInfo->m_pObject);
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
		{
			if( pInfo->m_pObject != NULL)
			{
				if( pInfo->m_pObject == m_pAIState)
				{
					SetAIState( NULL);
				}
			}
			else
			{
				SetAIState( NULL);
			}
		}
	}
}

LRESULT CPanelTimeTrack::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
