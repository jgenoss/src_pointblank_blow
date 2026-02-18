// PaneEventnAction.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "Globalvariable.h"
#include "PaneEventnAction.h"
#include "EditDlgAction_MoveTo.h"
#include "EditDlgAction_ScaleTo.h"
#include "EditDlgAction_RotateTo.h"
#include "EditDlgAction_PlaySound.h"

static const char * szActionString[I3UI_EVT_COUNT] = 
{
	"ACTION_MOVE",
	"ACTION_ROTATE",
	"ACTION_SCALE",
	"ACTION_SOUND"
};

static const char * szStateString[ I3UI_EVT_MAXCOUNT] = 
{
	"I3UI_EVT_NORMAL",
	"I3UI_EVT_CREATE",
	"I3UI_EVT_DESTROY",
	"I3UI_EVT_SHOW",
	"I3UI_EVT_HIDE",
	"I3UI_EVT_ACTIVATE",
	"I3UI_EVT_INACTIVATE",
	"I3UI_EVT_ENABLE",
	"I3UI_EVT_DISABLE",
	"I3UI_EVT_SETFOCUS",
	"I3UI_EVT_KILLFOCUS",
	"I3UI_EVT_ENTERMOUSE",
	"I3UI_EVT_LEAVEMOUSE",
	"I3UI_EVT_CLICKED",
	"I3UI_EVT_DBL_CLICKED",
	"I3UI_EVT_R_CLICKED",
	"I3UI_EVT_R_DBL_CLICKED",
	"I3UI_EVT_M_CLICKED",
	"I3UI_EVT_M_DBL_CLICKED",
	"I3UI_EVT_DRAGSTART",
	"I3UI_EVT_DRAGGING",
	"I3UI_EVT_DRAGEND",
	"I3UI_EVT_DRAGCANCEL",

	"I3UI_EVT_SIZING",
	"I3UI_EVT_SIZE",

	"I3UI_EVT_MOVING",
	"I3UI_EVT_MOVE",

	"I3UI_EVT_BTN_PUSHED",
	"I3UI_EVT_IME",
	"I3UI_EVT_CHANGED",
	"I3UI_EVT_RESERVED3",
	"I3UI_EVT_RESERVED4",
	"I3UI_EVT_RESERVED5",
	"I3UI_EVT_RESERVED6",
	"I3UI_EVT_RESERVED7",
	"I3UI_EVT_RESERVED8",
	"I3UI_EVT_RESERVED9",

	"I3UI_EVT_RESERVED10",
	"I3UI_EVT_RESERVED11",
	"I3UI_EVT_RESERVED12",
	"I3UI_EVT_RESERVED13",
	"I3UI_EVT_RESERVED14",
	"I3UI_EVT_RESERVED15",
	"I3UI_EVT_RESERVED16",
	"I3UI_EVT_RESERVED17",
	"I3UI_EVT_RESERVED18",
	"I3UI_EVT_RESERVED19"
};

// CPaneEventnAction dialog

IMPLEMENT_DYNAMIC(CPaneEventnAction, i3TDKDialogBase)
CPaneEventnAction::CPaneEventnAction(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneEventnAction::IDD, pParent)
{
	m_pObj		= NULL;
	m_nState	= I3UI_EVT_NORMAL;
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

	I3UI_EVT_DATA * pAction = m_pObj->getEventData( m_nState);

	if( pAction == NULL)	
		return;

	INT32 nActionStyle = pAction->m_nEventStyle;
	
	INT32 i;
	for( i = 0; i< I3UI_EVT_COUNT; ++i)
	{
		if( nActionStyle & ( 1 << i))
		{
			m_ctrlActionList.InsertItem( m_ctrlActionList.GetItemCount(), szActionString[i] );
		}
	}
}

void CPaneEventnAction::_SetActionSettings( INT32 nIdx, i3UIControl * pObj)
{
	m_ctrlActionList.DeleteAllItems();

	if( pObj == NULL)	return;
	
	m_nState	= (I3UI_EVT)nIdx;
	
	I3UI_EVT_DATA	*	pAction			= pObj->getEventData( m_nState);
	INT32				nActionStyle	= 0;

	if( pAction != NULL)
	{
		nActionStyle = pAction->m_nEventStyle;
	}

	INT32 i;
	for( i = 0; i< I3UI_EVT_MAXCOUNT; ++i)
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
	if( pObj->IsTypeOf( i3UIControl::GetClassMeta()))
	{
		i3UIControl * pBase = (i3UIControl *)pObj;

		m_pObj = pBase;

		//Set State List
		INT32 i = 0;
		for( i = 0; i < I3UI_EVT_MAXCOUNT; ++i)
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

	i3TDK::RegisterUpdate( m_hWnd, i3GameObjBase::GetClassMeta(), I3_TDK_UPDATE_SELECT);
	i3TDK::RegisterUpdate( m_hWnd, i3GameObjBase::GetClassMeta(), I3_TDK_UPDATE_REMOVE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void	CPaneEventnAction::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_hwndFrom == this->m_hWnd )	
		return ;

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( pInfo->m_pMeta->IsTypeOf( i3GameObjBase::GetClassMeta()))
		{		
			_SetEventSettings( (i3GameObjBase *)pInfo->m_pObject);
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE && pInfo->m_Event & I3_TDK_USRWND_GAMEGRAPH)
	{
		if( pInfo->m_pMeta->IsTypeOf( i3GameObjBase::GetClassMeta()))
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

	if( pNMListView->iItem >= 0 && pNMListView->iItem < I3UI_EVT_MAXCOUNT)
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
			_AddAction( I3UI_EVT_MOVE);
		}
		break;
	case	ID_ACTION_ADDSCALE:
		{
			_AddAction( I3UI_EVT_SCALE);
		}
		break;
	case	ID_ACTION_ADDROTATE:
		{
			_AddAction( I3UI_EVT_ROTATE);
		}
		break;
	case	ID_ACTION_ADDSOUND:
		{
			_AddAction( I3UI_EVT_SOUND);
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

	I3UI_EVT_DATA * pAction = m_pObj->getEventData( m_nState);

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
		pAction = I3UI_EVT_DATA::NewObjectRef();
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
	case I3UI_EVT_MOVE:
		{
			p = i3MemAlloc( sizeof( I3UI_EVT_MOVE_DATA));
			((I3UI_EVT_MOVE_DATA*)p)->Init();
			pAction->m_pMoveData = (I3UI_EVT_MOVE_DATA*)p;
		}	
		break;


	case I3UI_EVT_ROTATE:
		{
			p = i3MemAlloc( sizeof( I3UI_EVT_ROTATE_DATA));
			((I3UI_EVT_ROTATE_DATA*)p)->Init();
			pAction->m_pRotateData = (I3UI_EVT_ROTATE_DATA*)p;
		}
		break;


	case I3UI_EVT_SCALE:
		{
			p = i3MemAlloc( sizeof( I3UI_EVT_SCALE_DATA));
			((I3UI_EVT_SCALE_DATA*)p)->Init();
			pAction->m_pScaleData = (I3UI_EVT_SCALE_DATA*)p;
		}	
		break;


	case I3UI_EVT_SOUND:
		{
			i3SoundPlayInfo * pInfo			= i3SoundPlayInfo::NewObjectRef();
			i3SoundNode		* pSoundNode	= i3SoundNode::NewObject();

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
	for( i = 0; i< I3UI_EVT_MAXCOUNT; ++i)
	{
		if( i3String::Compare(szString, szActionString[i]) == 0)
		{
			nStyle = ( 1 << i);
			break;
		}
	}

	INT32						nEventStyle	= 0;
	I3UI_EVT_DATA *	pAction		= m_pObj->getEventData( m_nState);

	if( pAction != NULL)
	{
		nEventStyle = pAction->m_nEventStyle;
	}
	
	I3ASSERT( (nEventStyle & nStyle) != 0);

	switch( nStyle)
	{
	case I3UI_EVT_MOVE:
		{
			I3MEM_SAFE_FREE( (I3UI_EVT_MOVE_DATA *)pAction->m_pMoveData);
			break;
		}
	case I3UI_EVT_ROTATE:
		{
			I3MEM_SAFE_FREE( (I3UI_EVT_ROTATE_DATA *)pAction->m_pRotateData);
			break;
		}
	case I3UI_EVT_SCALE:
		{
			I3MEM_SAFE_FREE( (I3UI_EVT_SCALE_DATA *)pAction->m_pScaleData);
			break;
		}
	case I3UI_EVT_SOUND:	
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
	for( i = 0; i< I3UI_EVT_MAXCOUNT; ++i)
	{
		if( i3String::Compare(szString, szActionString[i]) == 0)
		{
			nStyle = ( 1 << i);
			break;
		}
	}

	I3UI_EVT_DATA *	pAction		= m_pObj->getEventData( m_nState);
	INT32			nEventStyle	= pAction->m_nEventStyle;

	
	I3ASSERT( (nEventStyle & nStyle) != 0);

	switch( nStyle)
	{
	case I3UI_EVT_MOVE:
		{
			CEditDlgAction_MoveTo	Dlg;
			Dlg.SetMoveEventData( pAction->m_pMoveData);
			Dlg.DoModal();
			break;
		}
	case I3UI_EVT_ROTATE:
		{
			CEditDlgAction_RotateTo	Dlg;
			Dlg.SetRotateEventData( pAction->m_pRotateData);
			Dlg.DoModal();
			break;
		}
	case I3UI_EVT_SCALE:
		{
			CEditDlgAction_ScaleTo	Dlg;
			Dlg.SetScaleEventData( pAction->m_pScaleData);
			Dlg.DoModal();
			break;
		}
	case I3UI_EVT_SOUND:	
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
