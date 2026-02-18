// PanelTimeEvent.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "PanelTimeEvent.h"
#include "ObjectState.h"
#include ".\paneltimeevent.h"


// CPanelTimeEvent dialog

IMPLEMENT_DYNAMIC(CPanelTimeEvent, i3TDKDialogBase)
CPanelTimeEvent::CPanelTimeEvent(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelTimeEvent::IDD, pParent)
{
}

CPanelTimeEvent::~CPanelTimeEvent()
{
}

void CPanelTimeEvent::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}

void CPanelTimeEvent::OnStartPlay(void)
{
	m_TimeEventCtrl.setAutoRefreshEnable( true);
}

void CPanelTimeEvent::OnStartPause(void)
{
	m_TimeEventCtrl.setAutoRefreshEnable( false);
}

BEGIN_MESSAGE_MAP(CPanelTimeEvent, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanelTimeEvent message handlers

BOOL CPanelTimeEvent::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rect;

		GetClientRect( &rect);

		rect.left += 3;
		rect.top += 3;
		rect.right -= 3;
		rect.bottom -= 3;

		m_TimeEventCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);
	}

	i3TDK::RegisterUpdate( m_hWnd, CObjectState::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelTimeEvent::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TimeEventCtrl.m_hWnd))
	{
		m_TimeEventCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void	CPanelTimeEvent::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( i3::same_of<CObjectState*>(pInfo->m_pMeta)) //->IsExactTypeOf( CObjectState::static_meta()))
	{
		switch( pInfo->m_Event)
		{
			case I3_TDK_UPDATE_SELECT :
				if( pInfo->m_pObject != NULL)
				{
					CObjectState * pState = (CObjectState *) pInfo->m_pObject;

					m_TimeEventCtrl.SetTimeSequence( pState->getTimeEvent());
				}
				else
				{
					m_TimeEventCtrl.SetTimeSequence( NULL);
				}
				break;

		}
	}
}

LRESULT CPanelTimeEvent::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}


void CPanelTimeEvent::OnOK()
{
}

void CPanelTimeEvent::OnCancel()
{
}
