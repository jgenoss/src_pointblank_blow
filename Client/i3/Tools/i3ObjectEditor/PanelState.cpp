// PanelState.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "PanelState.h"
#include "GlobalVar.h"
#include "DlgBuildConvex.h"
#include ".\panelstate.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/ftoa.h"

// CPanelState dialog

IMPLEMENT_DYNAMIC(CPanelState, i3TDKDialogBase)
CPanelState::CPanelState(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelState::IDD, pParent)
{
}

CPanelState::~CPanelState()
{
}

void CPanelState::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATELIST, m_StateListCtrl);
}

void CPanelState::UpdateAllState(void)
{
	INT32 i, idx;
	char conv[128];
	CObjectState * pState;

	m_StateListCtrl.DeleteAllItems();

	for( i = 0; i < g_pObject->getStateCount(); i++)
	{
		pState = g_pObject->getState( i);

		i3::ftoa( pState->getHPRatio(), conv);

		idx = m_StateListCtrl.InsertItem( i, conv, 0);

		UpdateState( idx);
	}
}

void CPanelState::UpdateState( INT32 idx)
{
	char conv[128];
	CObjectState * pState = g_pObject->getState( idx);

	// HP
	i3::ftoa( pState->getHPRatio(), conv);
	m_StateListCtrl.SetItemText( idx, 0, conv);

	// Event
	if( pState->getTimeEvent() == NULL)
		strcpy( conv, "-");
	else
        sprintf( conv, "%d", pState->getTimeEvent()->getEventCount());
	m_StateListCtrl.SetItemText( idx, 1, conv);

	// Convex
	if( pState->isContainConvex())
		strcpy( conv, "¡Û");
	else
		strcpy( conv, "-");
	m_StateListCtrl.SetItemText( idx, 2, conv);

	// Scene-Graph
	if( pState->getSgResource() == NULL)
		strcpy( conv, "-");
	else
	{
//		i3String::SplitFileName( pState->getSgResource()->GetName(), conv, FALSE);
		i3::extract_filetitle(pState->getSgResource()->GetName(), conv);
	}
	m_StateListCtrl.SetItemText( idx, 3, conv);
}

void CPanelState::OnAddState( CObjectState * pState)
{
	INT32 idx = g_pObject->GetStateIndex( pState);

	m_StateListCtrl.InsertItem( idx, "", 0);

	UpdateState( idx);
}

void CPanelState::OnEditState( CObjectState * pState)
{
	INT32 idx = g_pObject->GetStateIndex( pState);

	UpdateState( idx);
}

BEGIN_MESSAGE_MAP(CPanelState, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_STATELIST, OnNMRclickStatelist)
	ON_NOTIFY(NM_CLICK, IDC_STATELIST, OnNMClickStatelist)
END_MESSAGE_MAP()


// CPanelState message handlers

BOOL CPanelState::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_StateListCtrl.InsertColumn( 0, "HP", LVCFMT_LEFT, 60);
		m_StateListCtrl.InsertColumn( 1, "Event", LVCFMT_LEFT, 60);
		m_StateListCtrl.InsertColumn( 2, "Convex", LVCFMT_LEFT, 60);
		m_StateListCtrl.InsertColumn( 3, "SceneGraph", LVCFMT_LEFT, 150);
		m_StateListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	i3TDK::RegisterUpdate( m_hWnd, CObjectTemplate::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, CObjectState::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelState::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_StateListCtrl.m_hWnd))
	{
		m_StateListCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void	CPanelState::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	switch( pInfo->m_Event)
	{
		case I3_TDK_UPDATE_SCENE :
			UpdateAllState();
			break;

		case I3_TDK_UPDATE_ADD :
			if( i3::kind_of<CObjectState*>(pInfo->m_pMeta)) //->IsTypeOf( CObjectState::static_meta()))
			{
				if( pInfo->m_pObject != NULL)
				{
					OnAddState( (CObjectState *) pInfo->m_pObject);
				}
				else
				{
					UpdateAllState();
				}
			}
			break;

		case I3_TDK_UPDATE_EDIT :
			if( i3::kind_of<CObjectState*>(pInfo->m_pMeta)) //->IsTypeOf( CObjectState::static_meta()))
			{
				OnEditState( (CObjectState*) pInfo->m_pObject);
			}
			break;
	}
}

LRESULT CPanelState::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPanelState::OnNMRclickStatelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt, pt2;
	UINT cmd;
	CMenu menu, * pPopup;
//	i3ElementBase * pObj = NULL;

	GetCursorPos( &pt);

	{
		menu.LoadMenu( IDR_POPUP_STATE);
		pPopup = menu.GetSubMenu( 0);
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_STATE_DELETE :
			break;

		case ID_STATE_BUILDCOLLISIONHULL :
			{
				INT32 idx = _GetSelectedState();

				if( idx != -1)
				{
					g_pObject->BuildConvex( idx);

					UpdateState( idx);
				}
			}
			break;

		case ID_STATE_NEWSTATE :
			{
				CObjectState * pState = CObjectState::new_object_ref();

				pState->setOwner( g_pObject);
				pState->setHPRatio( 0.0f);

				g_pObject->AddState( pState);

				UpdateAllState();
			}
			break;

	}

	*pResult = 0;
}

INT32 CPanelState::_GetSelectedState(void)
{
	POSITION pos = m_StateListCtrl.GetFirstSelectedItemPosition();

//	INT32 nRet = -1;

	if( pos == NULL)
	{
		//if( m_StateListCtrl.GetItemCount() > 0)	return 0;
		//else									return -1;
		return -1;
	}
	else
	{
		return m_StateListCtrl.GetNextSelectedItem( pos);
	}
}

void CPanelState::OnNMClickStatelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 idx;

	*pResult = 0;

	idx = _GetSelectedState();
	if( idx == -1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, g_pObject);
		return;
	}

	g_pObject->SetCurrentState( idx);

}

void CPanelState::OnOK()
{
}

void CPanelState::OnCancel()
{
}
