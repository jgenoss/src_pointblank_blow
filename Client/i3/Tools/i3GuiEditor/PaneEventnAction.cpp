// PaneEventnAction.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiEditor.h"
#include "Globalvariable.h"
#include "PaneEventnAction.h"
#include "EditDlgAction_MoveTo.h"
#include "EditDlgAction_ScaleTo.h"
#include "EditDlgAction_RotateTo.h"
#include "EditDlgAction_PlaySound.h"

static const char * szActionString[I3GUI_EVENT_COUNT] = 
{
	"ACTION_MOVE",
	"ACTION_ROTATE",
	"ACTION_SCALE",
	"ACTION_SOUND"
};

static const char * szStateString[ MAX_I3GUI_CONTROL_STATE] = 
{
	"STATE_NORMAL",
	"STATE_CLICKED",
	"STATE_FOCUS",
	"STATE_ONMOUSE",
	"STATE_START",
	"STATE_TIMEOUT",
	"STATE_DRAG",
	"STATE_PROGRESS",
	"STATE_DISABLED",
	
	"STATE_OnEnable",
	"STATE_OnDisable",	
	"STATE_R_CLICKED",
	"RESERVED2",
	"RESERVED3",
	"RESERVED4",
	"RESERVED5",
	"RESERVED6",
	"RESERVED7",
	"RESERVED8",
	"RESERVED9"
};

// CPaneEventnAction dialog

IMPLEMENT_DYNAMIC(CPaneEventnAction, i3TDKDialogBase)
CPaneEventnAction::CPaneEventnAction(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneEventnAction::IDD, pParent)
{
	m_pObj		= NULL;
	m_nState	= I3GUI_CONTROL_STATE_NORMAL;
}

CPaneEventnAction::~CPaneEventnAction()
{
	m_pObj	= NULL;
}

void CPaneEventnAction::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ENA_STATE, m_ctrlStateList);
	DDX_Control(pDX, IDC_LIST_ENA_ACTION, m_ctrlActionList);
}

///////////////////////////////////////////////////////////////////////////////
void CPaneEventnAction::_RefreshActionList()
{
	m_ctrlActionList.DeleteAllItems();

	if( m_pObj == NULL)	return;

	I3GUI_EVENT_DATA * pAction = m_pObj->getEventData( m_nState);

	if( pAction == NULL)	
		return;

	INT32 nActionStyle = pAction->m_nEventStyle;
	
	INT32 i;
	for( i = 0; i< I3GUI_EVENT_COUNT; ++i)
	{
		if( nActionStyle & ( 1 << i))
		{
			m_ctrlActionList.InsertItem( m_ctrlActionList.GetItemCount(), szActionString[i] );
		}
	}
}

void CPaneEventnAction::_SetActionSettings( INT32 nIdx, i3GuiObjBase * pObj)
{
	m_ctrlActionList.DeleteAllItems();

	if( pObj == NULL)	return;
	
	m_nState	= (GUI_CONTROL_STATE)nIdx;
	
	I3GUI_EVENT_DATA *	pAction			= pObj->getEventData( m_nState);
	INT32				nActionStyle	= 0;

	if( pAction != NULL)
	{
		nActionStyle = pAction->m_nEventStyle;
	}

	INT32 i;
	for( i = 0; i< I3GUI_EVENT_COUNT; ++i)
	{
		if( nActionStyle & (1 << i))
		{
			nIdx = m_ctrlActionList.InsertItem( m_ctrlActionList.GetItemCount(), szActionString[i] );
		}
	}
}

void CPaneEventnAction::_SetEventSettings( i3GameNode * pObj)
{
	//Reset
	m_ctrlStateList.DeleteAllItems();
	m_ctrlActionList.DeleteAllItems();

	if( pObj == NULL)	return;

	//chg ref
	if( i3::kind_of<i3GuiObjBase*>(pObj)) //->IsTypeOf( i3GuiObjBase::static_meta()))
	{
		i3GuiObjBase * pBase = (i3GuiObjBase *)pObj;

		m_pObj = pBase;

		//Set State List
		INT32 i = 0;
		for( i = 0; i < MAX_I3GUI_CONTROL_STATE; ++i)
		{
			m_ctrlStateList.InsertItem( i, szStateString[i], 0);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPaneEventnAction, i3TDKDialogBase)
//	ON_WM_CREATE()
ON_WM_SIZE()
//ON_NOTIFY(NM_RCLICK, IDC_LIST_ENA_STATE, OnNMRclickListEnaState)
ON_NOTIFY(NM_CLICK, IDC_LIST_ENA_STATE, OnNMClickListEnaState)
ON_NOTIFY(NM_RCLICK, IDC_LIST_ENA_ACTION, OnNMRclickListEnaAction)
END_MESSAGE_MAP()


// CPaneEventnAction message handlers
BOOL CPaneEventnAction::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText("Event and Action");

	m_ctrlStateList.InsertColumn( 0, "State", LVCFMT_LEFT, 250);
	m_ctrlStateList.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	
	m_ctrlActionList.InsertColumn( 0, "Action", LVCFMT_LEFT, 250);
	m_ctrlActionList.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	i3TDK::RegisterUpdate( m_hWnd, i3GuiObjBase::static_meta(), I3_TDK_UPDATE_SELECT);
	i3TDK::RegisterUpdate( m_hWnd, i3GuiObjBase::static_meta(), I3_TDK_UPDATE_REMOVE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void	CPaneEventnAction::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_hwndFrom == this->m_hWnd )	
		return ;

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( i3::kind_of<i3GuiObjBase*>(pInfo->m_pMeta)) //->IsTypeOf( i3GuiObjBase::static_meta()))
		{		
			_SetEventSettings( (i3GuiObjBase *)pInfo->m_pObject);
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE && pInfo->m_Event & I3_TDK_USRWND_GAMEGRAPH)
	{
		if( i3::kind_of<i3GuiObjBase*>(pInfo->m_pMeta)) //->IsTypeOf( i3GuiObjBase::static_meta()))
		{
			_SetEventSettings( NULL);
		}
	}
}

LRESULT CPaneEventnAction::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPaneEventnAction::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	RECT rt;
	GetClientRect( &rt);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_ctrlStateList))
	{
		m_ctrlStateList.SetWindowPos( NULL, 0, 0 , (cx >>1) , cy - 1, SWP_NOMOVE | SWP_NOACTIVATE);
		m_ctrlStateList.SetColumnWidth( 0, (cx >> 1) - 1);
	}

	if( ::IsWindow( m_ctrlActionList))
	{
		m_ctrlActionList.SetWindowPos( NULL, cx >>1, 0, (cx >> 1), cy  - 1, SWP_NOACTIVATE);
		m_ctrlActionList.SetColumnWidth( 0, cx >> 1);
	}
}

void CPaneEventnAction::OnNMClickListEnaState(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if( m_pObj == NULL)	return;

	if( pNMListView->iItem >= 0 && pNMListView->iItem < MAX_I3GUI_CONTROL_STATE)
	{
		_SetActionSettings( pNMListView->iItem, m_pObj);
	}

	*pResult = 0;
}

void CPaneEventnAction::OnNMRclickListEnaAction(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	LPNMLISTVIEW pNMListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	CRect rt;

	m_ctrlActionList.GetWindowRect(&rt);

	CMenu Menu, * pPopup;
	UINT32	cmd;

	{
		Menu.LoadMenu( IDR_POPUP_ACTION );
	}

	pPopup = Menu.GetSubMenu(0);

	if( m_pObj != 0)
	{
		if(pNMListView->iItem > -1 && pNMListView->iItem < m_ctrlActionList.GetItemCount())
		{
			pPopup->EnableMenuItem( ID_ACTION_ADDMOVE,		MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_ACTION_ADDSCALE,		MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_ACTION_ADDROTATE,	MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_ACTION_ADDSOUND,		MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}
		else
		{
			pPopup->EnableMenuItem( ID_POPUP_REMOVEACTION, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_POPUP_EDITACTION, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
		}
	}
	else
	{
		pPopup->EnableMenuItem( ID_ACTION_ADDMOVE,		MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_ACTION_ADDSCALE,		MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_ACTION_ADDROTATE,	MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_ACTION_ADDSOUND,		MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);

		pPopup->EnableMenuItem( ID_POPUP_REMOVEACTION, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_POPUP_EDITACTION, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED );
	}
	

	cmd = pPopup->TrackPopupMenuEx( 
		TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pNMListView->ptAction.x + rt.left,
		pNMListView->ptAction.y + rt.top,
		(CWnd *)this, NULL);

	switch( cmd )
	{
	case	ID_ACTION_ADDMOVE:
		{
			_AddAction( I3GUI_EVENT_MOVE);
		}
		break;
	case	ID_ACTION_ADDSCALE:
		{
			_AddAction( I3GUI_EVENT_SCALE);
		}
		break;
	case	ID_ACTION_ADDROTATE:
		{
			_AddAction( I3GUI_EVENT_ROTATE);
		}
		break;
	case	ID_ACTION_ADDSOUND:
		{
			_AddAction( I3GUI_EVENT_SOUND);
		}
		break;
	case	ID_POPUP_REMOVEACTION:
		{
			_Remove_Action( pNMListView->iItem);
		}
		break;
	case	ID_POPUP_EDITACTION:
		{
			_EditAction( pNMListView->iItem);
		}
		break;

	default:
		break;
	}

	*pResult = 0;
}

void CPaneEventnAction::_AddAction(UINT32 nAction)
{
	I3ASSERT( m_pObj != NULL);

	I3GUI_EVENT_DATA * pAction = m_pObj->getEventData( m_nState);

	INT32 nEventStyle = 0;

	if( pAction != NULL)	
	{
		nEventStyle = pAction->m_nEventStyle;
	}
	
	if( nEventStyle  & nAction)
	{
		AfxMessageBox("동일한 종류의 액션은 1개를 초과할수 없습니다", MB_OK, 0);
		return;
	}

	if( pAction == NULL)
	{
		pAction = I3GUI_EVENT_DATA::new_object_ref();
		pAction->Init();
	}
	else
	{
		pAction = m_pObj->getEventData( m_nState);
		I3ASSERT( pAction != NULL);
	}

	void * p;

	switch( nAction)
	{
	case I3GUI_EVENT_MOVE:
		{
			p = i3MemAlloc( sizeof( I3GUI_EVENT_MOVE_DATA));
			((I3GUI_EVENT_MOVE_DATA*)p)->Init();
			pAction->m_pMoveData = (I3GUI_EVENT_MOVE_DATA*)p;
		}	
		break;


	case I3GUI_EVENT_ROTATE:
		{
			p = i3MemAlloc( sizeof( I3GUI_EVENT_ROTATE_DATA));
			((I3GUI_EVENT_ROTATE_DATA*)p)->Init();
			pAction->m_pRotateData = (I3GUI_EVENT_ROTATE_DATA*)p;
		}
		break;


	case I3GUI_EVENT_SCALE:
		{
			p = i3MemAlloc( sizeof( I3GUI_EVENT_SCALE_DATA));
			((I3GUI_EVENT_SCALE_DATA*)p)->Init();
			pAction->m_pScaleData = (I3GUI_EVENT_SCALE_DATA*)p;
		}	
		break;


	case I3GUI_EVENT_SOUND:
		{
			i3SoundPlayInfo * pInfo			= i3SoundPlayInfo::new_object_ref();
			i3SoundNode		* pSoundNode	= i3SoundNode::new_object();

			pSoundNode->setSoundPlayInfo( pInfo);

			pAction->m_pSoundNode = pSoundNode;
		}	
		break;
	}

	if( nEventStyle == 0)
	{
		m_pObj->SetEventData( m_nState, pAction);
	}

	pAction->m_nEventStyle |= nAction;

	_RefreshActionList();
}

void CPaneEventnAction::_Remove_Action( INT32	nItem)
{
	if( nItem >= m_ctrlActionList.GetItemCount())
		return;

	I3ASSERT( m_pObj != NULL);
	I3ASSERT( m_pObj->getEventData( m_nState) != NULL);

	char szString[MAX_PATH];
	m_ctrlActionList.GetItemText( nItem, 0, szString, MAX_PATH - 1);

	//
	INT32 i, nStyle = 0;
	for( i = 0; i< I3GUI_EVENT_COUNT; ++i)
	{
		if( i3::generic_is_iequal(szString, szActionString[i]) )
		{
			nStyle = ( 1 << i);
			break;
		}
	}

	INT32				nEventStyle	= 0;
	I3GUI_EVENT_DATA *	pAction		= m_pObj->getEventData( m_nState);

	if( pAction != NULL)
	{
		nEventStyle = pAction->m_nEventStyle;
	}
	
	I3ASSERT( (nEventStyle & nStyle) != 0);

	switch( nStyle)
	{
	case I3GUI_EVENT_MOVE:
		{
			I3MEM_SAFE_FREE( (I3GUI_EVENT_MOVE_DATA *)pAction->m_pMoveData);
			break;
		}
	case I3GUI_EVENT_ROTATE:
		{
			I3MEM_SAFE_FREE( (I3GUI_EVENT_ROTATE_DATA *)pAction->m_pRotateData);
			break;
		}
	case I3GUI_EVENT_SCALE:
		{
			I3MEM_SAFE_FREE( (I3GUI_EVENT_SCALE_DATA *)pAction->m_pScaleData);
			break;
		}
	case I3GUI_EVENT_SOUND:	
		{
			I3_SAFE_RELEASE( pAction->m_pSoundNode);
			break;
		}
	default:
		break;
	}

	nEventStyle &= ~nStyle;
	if( nEventStyle == 0)
	{
		m_pObj->SetEventData( m_nState, NULL);
	}
	else
	{
		pAction->m_nEventStyle = nEventStyle;
	}

	_RefreshActionList();
}

void CPaneEventnAction::_EditAction( INT32 nItem)
{
	if( nItem >= m_ctrlActionList.GetItemCount())
		return;

	I3ASSERT( m_pObj != NULL);
	I3ASSERT( m_pObj->getEventData( m_nState) != NULL);

	char szString[MAX_PATH];
	m_ctrlActionList.GetItemText( nItem, 0, szString, MAX_PATH - 1);

	//
	INT32 i, nStyle = 0;
	for( i = 0; i< I3GUI_EVENT_COUNT; ++i)
	{
		if( i3::generic_is_iequal(szString, szActionString[i]) )
		{
			nStyle = ( 1 << i);
			break;
		}
	}

	I3GUI_EVENT_DATA *	pAction		= m_pObj->getEventData( m_nState);
	INT32				nEventStyle	= pAction->m_nEventStyle;

	
	I3ASSERT( (nEventStyle & nStyle) != 0);

	switch( nStyle)
	{
	case I3GUI_EVENT_MOVE:
		{
			CEditDlgAction_MoveTo	Dlg;
			Dlg.SetMoveEventData( pAction->m_pMoveData);
			Dlg.DoModal();
			break;
		}
	case I3GUI_EVENT_ROTATE:
		{
			CEditDlgAction_RotateTo	Dlg;
			Dlg.SetRotateEventData( pAction->m_pRotateData);
			Dlg.DoModal();
			break;
		}
	case I3GUI_EVENT_SCALE:
		{
			CEditDlgAction_ScaleTo	Dlg;
			Dlg.SetScaleEventData( pAction->m_pScaleData);
			Dlg.DoModal();
			break;
		}
	case I3GUI_EVENT_SOUND:	
		{
			CEditDlgAction_PlaySound Dlg;
			Dlg.SetSoundEventData( pAction->m_pSoundNode);
			Dlg.DoModal();
			break;
		}
	default:
		break;
	}
}
