#include "StdAfx.h"
#include "resource.h"
#include "i3UIShapeListCtrl.h"
#include "UIEShapePropertyDlg.h"

#include "MainFrm.h"
#include "i3UIEditorView_Template.h"

i3UIShapeListCtrl::i3UIShapeListCtrl(void)
{
	m_pCurrentTemplate = NULL;
}

i3UIShapeListCtrl::~i3UIShapeListCtrl(void)
{
	I3_SAFE_RELEASE( m_pCurrentTemplate);
}

BEGIN_MESSAGE_MAP(i3UIShapeListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, &i3UIShapeListCtrl::OnNMDblclk)
END_MESSAGE_MAP()

INT32 i3UIShapeListCtrl::getSelectedItem(bool bSelClick)
{
	POSITION pos;

	int nScrollPos = GetScrollPos(SB_VERT);

	pos = GetFirstSelectedItemPosition();
	
	CRect ref;
	GetItemRect( 0, ref, LVIR_BOUNDS );

	if( pos == NULL)
		return -1;

	int nItemPos = GetNextSelectedItem( pos);

	int nDelta = nItemPos - nScrollPos;

	CSize size(0,ref.Height()*nDelta);

	if (bSelClick)
		Scroll( size );	

	return nItemPos;
}

void i3UIShapeListCtrl::PopUpShapeMenu()
{
	if( m_pCurrentTemplate != NULL)// && i3::kind_of<i3UITemplate_User*>(m_pCurrentTemplate)) //->IsTypeOf( i3UITemplate_User::static_meta()) )
	{
		CMenu Menu, * pPopup;
		POINT pt;
		UINT32 cmd;
//		List MetaList;		//쓰이지 않아 주석..

		GetCursorPos( &pt);

		{
			Menu.LoadMenu( IDR_MENU_SHAPELIST);
		}

		pPopup = Menu.GetSubMenu( 0);

		INT32 idx = getSelectedItem();
		if( idx == -1)
		{
			pPopup->EnableMenuItem( ID_SHAPELIST_REMOVESHAPE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SHAPELIST_EDITSHAPEINFO, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SHAPELIST_MAKECLONE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}

		if( !i3::kind_of<i3UITemplate_User*>(m_pCurrentTemplate)) //->IsTypeOf( i3UITemplate_User::static_meta()) == FALSE)
		{
			pPopup->EnableMenuItem( ID_SHAPELIST_REMOVESHAPE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SHAPELIST_ADDSHAPE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SHAPELIST_MAKECLONE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}

		cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
					pt.x, pt.y, (CWnd *) this, NULL);

		switch( cmd)
		{
			case ID_SHAPELIST_REMOVESHAPE:		RemoveShape( idx);	break;
			case ID_SHAPELIST_EDITSHAPEINFO:	EditShape( idx);	break;
			case ID_SHAPELIST_ADDSHAPE:			AddShape();	break;
			case ID_SHAPELIST_MAKECLONE:		MakeClone(idx);	break;
		}
	}
}

INT32 i3UIShapeListCtrl::MakeClone( INT32 idx)
{
	I3_BOUNDCHK( idx, m_pCurrentTemplate->getShapeCount());

	I3UI_SHAPE_INFO	* pSrcInfo = ((i3UITemplate_User*)m_pCurrentTemplate)->getShapeInfo(idx);
	
	I3UI_SHAPE_INFO * pInfo = new I3UI_SHAPE_INFO; //(I3UI_SHAPE_INFO*)i3MemAlloc( sizeof( I3UI_SHAPE_INFO));
	pInfo->m_pShape = i3UIShape::new_object();

	{
		pSrcInfo->m_pShape->CopyTo( pInfo->m_pShape, I3_COPY_INSTANCE);
		pInfo->m_strInfoString = pSrcInfo->m_strInfoString;
	}

	INT32 ret = ((i3UITemplate_User*)m_pCurrentTemplate)->AddNewShape( pInfo);

	UpdateAllShape();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pInfo->m_pShape);

	return ret;
}

void	i3UIShapeListCtrl::UpdateAllShape(void)
{
	DeleteAllItems();

	if( m_pCurrentTemplate == NULL)
		return;

	INT32 i, idx, count = m_pCurrentTemplate->getShapeCount();
	i3UIShape * pShape;
	char conv[256];

	for( i = 0; i < count; i++)
	{
		pShape = m_pCurrentTemplate->getShape( i);		

		i3::snprintf( conv, sizeof(conv), "%d", i + 1);

		idx = InsertItem( i, conv);

		UpdateShape( idx, i, pShape);
	}
}

void	i3UIShapeListCtrl::UpdateShape( INT32 idxItem, INT32 idxShape, i3UIShape * pShape)
{
	if( m_pCurrentTemplate == NULL)
		return;

	char conv[256];

	SetItemData( idxItem, (DWORD_PTR) pShape);

	// Name
	const char * pszName = m_pCurrentTemplate->getShapeInfoString( idxShape);
	SetItemText( idxItem, 1, pszName);

	// X
	i3::snprintf( conv, sizeof(conv), "%d", (INT32) pShape->getPosX());
	SetItemText( idxItem, 2, conv);

	// Y
	i3::snprintf( conv, sizeof(conv), "%d", (INT32) pShape->getPosY());
	SetItemText( idxItem, 3, conv);

	// W
	i3::snprintf( conv, sizeof(conv), "%d", (INT32) pShape->getWidth());
	SetItemText( idxItem, 4, conv);

	// H
	i3::snprintf( conv, sizeof(conv), "%d", (INT32) pShape->getHeight());
	SetItemText( idxItem, 5, conv);

	// Tex
	SetItemText( idxItem, 6, pShape->getTextureResPath());
}

INT32 i3UIShapeListCtrl::AddShape( void)
{
	I3ASSERT( m_pCurrentTemplate  != NULL);	// menu에 의해 호출 되는 부분이어서 절대로 template가 null이면 안된다.

	CUIEShapePropertyDlg dlg;

	REAL32 width = 100.f;
	REAL32 height = 100.f;

	if( m_pCurrentTemplate->getShapeCount() > 0)
	{
		i3UIShape * pShape = m_pCurrentTemplate->getShape( m_pCurrentTemplate->getShapeCount() -1);		

		width = pShape->getWidth();
		height = pShape->getHeight();
	}

	if( dlg.Execute( width, height) == true)
	{
		INT32 _addcount = dlg.GetShapeCount();
		i3UITemplate_User* pTempUser = (i3UITemplate_User*)m_pCurrentTemplate;
		

		for(INT32 i=0; i<_addcount; i++)
		{
			I3UI_SHAPE_INFO * pInfo = new I3UI_SHAPE_INFO;	//	(I3UI_SHAPE_INFO*)i3MemAlloc( sizeof( I3UI_SHAPE_INFO));

			pInfo->m_pShape = i3UIShape::new_object();

			dlg.GetShapeInfo( pInfo);
			pTempUser->AddNewShape( pInfo);

			UpdateAllShape();

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pInfo->m_pShape);
		}
		resortTemplateCount(pTempUser);
		UpdateAllShape();
		//for(INT32 i=0; i<_addcount; i++)
		//	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, pInfo->m_pShape);
	}

	return 1;
}

INT32 i3UIShapeListCtrl::RemoveShape( INT32 idx)
{
	I3ASSERT( m_pCurrentTemplate  != NULL);	// menu에 의해 호출 되는 부분이어서 절대로 template가 null이면 안된다.

	if( idx >= m_pCurrentTemplate->getShapeCount() || idx < 0)
		return -1;

	I3UI_SHAPE_INFO * pInfo = ((i3UITemplate_User*)m_pCurrentTemplate)->getShapeInfo(idx);

	//여기서 직접 호출 하지 않고, 받아 들이는 template view에서 처리한다.( 안그러면 메시지 순서 꼬여서 크래시 함)
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REMOVE, pInfo->m_pShape, NULL);
	
	return 1;
}

INT32 i3UIShapeListCtrl::EditShape( INT32 idx)
{
	I3ASSERT( m_pCurrentTemplate  != NULL);	// menu에 의해 호출 되는 부분이어서 절대로 template가 null이면 안된다.

	I3UI_SHAPE_INFO * pInfo = NULL;

	if( i3::kind_of<i3UITemplate_User*>(m_pCurrentTemplate)) //->IsTypeOf( i3UITemplate_User::static_meta()) )
	{
		pInfo = ((i3UITemplate_User*)m_pCurrentTemplate)->getShapeInfo(idx);
		if( pInfo != NULL)
		{
			CUIEShapePropertyDlg dlg;
			dlg.setShapeInfo( pInfo);
			if( dlg.DoModal() == IDOK)
			{
				// 버튼 이미지셋의 경우 데이타가 리셋되는 버그가 있어 예외 처리 추가
				if( i3::kind_of<i3UITemplate_ButtonImageSet*>(m_pCurrentTemplate)) //->IsTypeOf( i3UITemplate_ButtonImageSet::static_meta() ) )
				{
					CMainFrame *frame = (CMainFrame*)AfxGetMainWnd();
					CView* pView = frame->GetActiveView();

					if( pView )
					{
						if( pView->IsKindOf( RUNTIME_CLASS( Ci3UIEditorView_Template)) )
						{
							Ci3UIEditorView_Template* pTView = (Ci3UIEditorView_Template*)pView;

							m_pCurrentTemplate->AdjustLayout( pInfo->m_pShape->getWidth(), pInfo->m_pShape->getHeight() );
						}
					}	
				}

				UpdateAllShape();
				
				i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pInfo->m_pShape);
			}
		}
	}
	else
	{
		I3UI_SHAPE_INFO shapeInfo;
		shapeInfo.m_strInfoString.clear();
		shapeInfo.m_pShape			= m_pCurrentTemplate->getShape(idx);

		CUIEShapePropertyDlg dlg;
		dlg.setOnlyRectMode( true);
		dlg.setShapeInfo( &shapeInfo);
		if( dlg.DoModal() == IDOK)
		{
			UpdateAllShape();

			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, shapeInfo.m_pShape);
		}
	}

	return 1;
}

void i3UIShapeListCtrl::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	INT32 i;

	if( m_pCurrentTemplate == NULL)
		return;

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		// find shape
		bool bFind = false;
		INT32 findIdx = -1;
		for( i = 0; i < m_pCurrentTemplate->getShapeCount(); i++)
		{
			if( GetItemData( i) == (DWORD) pInfo->m_pObject)
			{
				bFind = true;
				findIdx = i;
				break;
			}
		}

		if( bFind)
		{
			//SetFocus();
			for( i = 0; i < m_pCurrentTemplate->getShapeCount(); i++)
			{
				SetItemState( i, 0, 0xFFFFFFFF);
			}

			SetItemState( findIdx, LVIS_FOCUSED | LVIS_SELECTED, 0xFFFFFFFF);
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_UNSELECT)
	{
		for( i = 0; i < m_pCurrentTemplate->getShapeCount(); i++)
		{
			SetItemState( i, 0, 0xFFFFFFFF);
		}
	}
	else if (pInfo->m_Event & I3_TDK_UPDATE_ALL)
	{
		UpdateAllShape();
	}
}

LRESULT i3UIShapeListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	if( message == WM_KEYDOWN )
	{
		const INT32 nSelected = getSelectedItem();

		switch( LOWORD(wParam) )
		{
		case	VK_HOME:
				{
					if( 0 < nSelected )
					{
						MoveShape( nSelected, nSelected - 1 );
					}
				}
				return 0;

		case	VK_END:
				{
					if( nSelected < (GetItemCount() -1) )
					{
						MoveShape( nSelected, nSelected + 1 );
					}
				}
				return 0;

		case	VK_DELETE:
				{
					//
				}
				return 0;
		}
	}

	return CListCtrl::WindowProc(message, wParam, lParam);
}

int i3UIShapeListCtrl::OnCreate( LPCREATESTRUCT lpCreateStruct)
{
	i3TDK::RegisterUpdate( m_hWnd, i3UIShape::static_meta(), I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_UNSELECT | I3_TDK_UPDATE_ALL);
	
	return CListCtrl::OnCreate( lpCreateStruct);
}

void i3UIShapeListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	pNMItemActivate;
	
	// TODO: Add your control notification handler code here
	INT32 idx = getSelectedItem();

	if( idx != -1)
	{
		EditShape(idx);
	}

	*pResult = 0;
}

INT32 i3UIShapeListCtrl::MoveShape( INT32 nSel, INT32 nMove )
{
	I3ASSERT( m_pCurrentTemplate != NULL );

	if( NULL == m_pCurrentTemplate )
	{
		return 0; 
	}

	const int nCnt = m_pCurrentTemplate->getShapeCount();

	if( nSel < 0 || nCnt <= nSel )
	{
		return 0; 
	}

	if( nMove < 0 || nCnt <= nMove )
	{
		return 0; 
	}

	I3UI_SHAPE_INFO* prev = ((i3UITemplate_User*)m_pCurrentTemplate)->getShapeInfo( nSel );
	I3UI_SHAPE_INFO* move = ((i3UITemplate_User*)m_pCurrentTemplate)->getShapeInfo( nMove );

	if( NULL == prev || NULL == move )
	{
		return 0; 
	}

	// data swap
	{
		I3UI_SHAPE_INFO tmp;

		tmp.m_pShape = prev->m_pShape;
		tmp.m_strInfoString = prev->m_strInfoString;
	
		prev->m_pShape = move->m_pShape;
		prev->m_strInfoString = move->m_strInfoString;
	
		move->m_pShape = tmp.m_pShape;
		move->m_strInfoString = tmp.m_strInfoString;
	}

	SetItemText( nSel, 0,	prev->m_strInfoString.c_str() );
	UpdateShape( nSel, nSel, prev->m_pShape );

	SetItemText( nMove, 0,	move->m_strInfoString.c_str() );
	UpdateShape( nMove, nMove, move->m_pShape );	

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, move->m_pShape );	
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, move->m_pShape );
	return 1;
}

void i3UIShapeListCtrl::resortTemplateCount(i3UITemplate_User * pUser)
{
	INT32 _count = pUser->getShapeCount();
	for(INT32 i=0; i<_count; i++)
	{
		I3UI_SHAPE_INFO * pInfo = pUser->getShapeInfo(i);
		char conv[MAX_PATH] = {0,};
		::itoa(i+1, conv, 10);
		pInfo->m_strInfoString = conv;
	}
}
