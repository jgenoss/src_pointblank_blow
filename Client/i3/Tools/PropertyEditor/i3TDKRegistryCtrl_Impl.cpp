// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "GlobalVariable.h"

#include "i3TDKRegistryCtrl_Impl.h"
#include "i3TDKUtilShell.h"

#include "MainFrm.h"
#include "resource.h"

#include "AckDialog.h"


IMPLEMENT_DYNAMIC(i3TDKRegistryCtrl_Impl, CWnd)

bool i3TDKRegistryCtrl_Impl::ms_useRenderContext = true;


i3TDKRegistryCtrl_Impl::i3TDKRegistryCtrl_Impl()
{
	m_dwStyle = 0;

	m_pImageList			= NULL;
	m_nIndexOfFirstTDKImage	= 0;	//TDK res에서 add된 icon의 첫번째 index

	m_pRoot = NULL;
	m_pClassTypeRoot = NULL;
	m_pItemListRoot = NULL;
	m_pWeaponRoot = NULL;

	m_pSubTreeFirstItem = NULL;
	m_pCurItem = NULL;
	m_pTreeCurItem = NULL;
	m_nCurRow = -1;

	m_SrcItem				= NULL;
	//m_SrcData				= NULL;

	m_FocusListCtrl = NULL;
	m_FocusTreeCtrl = NULL;

	m_CurrentClassTypeIndex = 0;
	m_MaxClassTypeIndex = 0;

	m_bItemWeaponPef = false;

	m_hAccel = LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_ACCEL_EDITING));
}

i3TDKRegistryCtrl_Impl::~i3TDKRegistryCtrl_Impl()
{
	_clearTemp();

	I3_SAFE_DELETE( m_pImageList);
	I3_SAFE_RELEASE( m_pContext);

	ClearWeaponDictionary();
}

BEGIN_MESSAGE_MAP(i3TDKRegistryCtrl_Impl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( NM_RCLICK, REG_TREECTRL, OnNMRClickTree)
	ON_NOTIFY( TVN_SELCHANGED, REG_TREECTRL, OnTVNSelChanged)
	ON_NOTIFY( TVN_ENDLABELEDIT, REG_TREECTRL, OnTVNEndLabelEdit)
END_MESSAGE_MAP()




// i3RegistryCtrl message handlers
BOOL i3TDKRegistryCtrl_Impl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR	hCursor;
	HBRUSH	hBack;

	m_dwStyle = dwStyle;

	hCursor = LoadCursor( NULL, IDC_ARROW);
	I3ASSERT( hCursor != NULL);

	hBack = (HBRUSH)GetStockObject( WHITE_BRUSH);
	I3ASSERT( hBack != NULL);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, NULL);
	I3ASSERT( pszClass != NULL);

	dwStyle	|= WS_CLIPCHILDREN;

	return CWnd::Create( pszClass, "", dwStyle, rect, pParentWnd, nID, NULL);
}

int i3TDKRegistryCtrl_Impl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect( &rect);

	{
		if( m_TreeCtrl.Create( m_dwStyle | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VISIBLE | 
								TVS_HASBUTTONS | TVS_HASLINES | TVS_EDITLABELS | TVS_SHOWSELALWAYS |
								TVS_INFOTIP | TVS_LINESATROOT,
								rect, this, REG_TREECTRL) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not Create TreeCtrl");
		}

		if( m_SubTree.Create( m_dwStyle | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VISIBLE | 
								TVS_HASBUTTONS | TVS_HASLINES | TVS_EDITLABELS | TVS_SHOWSELALWAYS |
								TVS_INFOTIP | TVS_LINESATROOT,
								CRect(0,0,0,0), this, REG_TREECTRL) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not Create SubTreeCtrl");
		}
		m_SubTree.ShowWindow( FALSE );
	}

	{
		if( m_ItemListViewCtrl.Create( m_dwStyle | WS_BORDER | WS_VISIBLE , rect, this, REG_ITEM_LISTCTRL) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not Create ItemListView");
		}
		m_ItemListViewCtrl.SetUseCellLClickList( true );
		m_ItemListViewCtrl.Set_Name("List1");
		m_ItemListViewCtrl.ShowWindow( TRUE );

		if( m_WeaponListViewCtrl.Create( m_dwStyle | WS_BORDER | WS_VISIBLE , CRect(0,0,0,0), this, REG_WEAPON_LISTCTRL) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not Create WeaponListView");
		}
		m_WeaponListViewCtrl.SetUseCellLClickList( false );
		m_WeaponListViewCtrl.Set_Name("List2");
		m_WeaponListViewCtrl.ShowWindow( FALSE );
	}

	{
		if( m_ImageViewCtrl.Create( m_dwStyle | WS_BORDER, this, REG_IMAGE_VIEW) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not Create ImageView");
		}
		m_ImageViewCtrl.ShowWindow( FALSE );
	}
	{
		i3VideoInfo video;

		video.m_Width = 32;
		video.m_Height = 32;

		::SetCurrentDirectory( g_pProfile->m_WorkDirectoryPath );
		
		if(ms_useRenderContext) 
		{
			// chunjong.song 2015-01-06 - 적어도 PropertyEditor에서 사용할일이없다 생각되어서
			m_pContext = i3RenderContext::new_object();
			m_pContext->Create( &video, m_hWnd);
		}
		else
		{
			m_pContext = NULL;
		}
	}

	{
//사용안함.
		if( m_ListDataCtrl.Create( m_dwStyle | WS_BORDER | LVS_SORTASCENDING, rect, this, REG_EDITCTRL) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not Create EditView");
		}
		m_ListDataCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_ListDataCtrl.ShowWindow( FALSE );
	}

	{
		m_pImageList			= i3TDK::GetSystemImageList();
		m_nIndexOfFirstTDKImage = i3TDK::AddTDKImageListTo( m_pImageList);

		m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3RegData::static_meta(), I3_TDK_UPDATE_EDIT);


	{
		if( m_SubTreeToolTip.Create(this) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not Create Subtree Tooltip");
		}
	}

	return 0;
}

void i3TDKRegistryCtrl_Impl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( GetItemWeaponPef() )
	{
		g_pProfile->m_Image.right = cx;
		g_pProfile->m_List2.right = cx;

		g_pProfile->m_Tree1.bottom = cy;
		g_pProfile->m_List1.bottom = cy;
		g_pProfile->m_List2.bottom = cy;
	}
	else
	{
		g_pProfile->m_SimpleList.right = cx;
		g_pProfile->m_SimpleTree.bottom = cy;
		g_pProfile->m_SimpleList.bottom = cy;
	}

	ReSizeControl();
}

CString i3TDKRegistryCtrl_Impl::GetNewKeyName(i3RegKey * pRegKey)
{
	UINT NewKeyCount = 0;

	CString Ret;
	Ret.Format("New Key %d", NewKeyCount);

	i3RegKey* pData = i3Registry::FindKey( Ret.GetString(), pRegKey );

	while( pData )
	{
		++NewKeyCount;
		Ret.Format("New Key %d", NewKeyCount);
		pData = i3Registry::FindKey( Ret.GetString(), pRegKey );
	}

	return Ret;
}

void i3TDKRegistryCtrl_Impl::OnNMRClickTree( NMHDR * pNMHDR, LRESULT * pResult)
{
	m_FocusTreeCtrl = (CTreeCtrl *)FromHandle( pNMHDR->hwndFrom );

	UINT32	cmd;
	CRect rect;

	HTREEITEM hSel = NULL;
	POINT	pt;
	GetCursorPos( &pt);

	{
		POINT	cpt;
		UINT	flag;

		cpt = pt;
		m_FocusTreeCtrl->ScreenToClient( &cpt);

		hSel = m_FocusTreeCtrl->HitTest( cpt, &flag);
		if((hSel != NULL) && (TVHT_ONITEM & flag))
		{
			m_FocusTreeCtrl->Select( hSel, TVGN_CARET);
		}
	}

	bool ItemListMenu = false;

	if( GetItemWeaponPef() && *m_FocusTreeCtrl == m_TreeCtrl  )
	{
		ItemListMenu = true;
	}
	//	return;


	CMenu * pKeyMenu = new CMenu;
	CMenu * pDataMenu = new CMenu;

	{
		pKeyMenu->CreatePopupMenu();
		pDataMenu->CreatePopupMenu();
	}

	{//Key Menu
		if( hSel != NULL)
		{
			pKeyMenu->AppendMenu( MF_POPUP, (INT_PTR)pDataMenu->m_hMenu, "새로 만들기  &N");
			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_COPYKEY, "복사 &C");
			if( m_SrcItem != NULL)
			{
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_PASTEKEY, "키값 붙여넣기");
			}

			if( !m_conSelected.empty() )
			{
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_PASTEDATA, "값 붙여넣기 &V");
			}

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DELETEKEY, "삭제 &D");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_RENAMEKEY, "이름 바꾸기");

			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_IMPORTKEY, "키 가져오기");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORTKEY, "키 내보내기");
			
			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_REMOVEALL_SELKEYS,   "익스포트목록 모두삭제..");

			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_ADD_SELKEYS,   "익스포트목록에 현재 선택된키 추가..");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_SHOW_SELKEYS,   "익스포트목록 보기..");			
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_IMPORT_SELKEYS, "익스포트된 키파일들 로드하기..");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORT_SELKEYS, "익스포트목록의 키들 저장하기..");
			
			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORT_SQLCREATE, "SQL (CREATE) 내보내기");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORT_SQLUPDATE, "SQL (UPDATE) 내보내기");			

			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");
		}

//		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_FINDKEY, "찾기..");
//		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_COLLAPSENODE, "축소");
//		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPANDNODE, "확장");
		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_SORT_KEY, "키 정렬");
//		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_SORT_TIME, "시간 정렬");
	}

	{//Data Menu
		pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_ADDKEY, "새 키값	&N");
			
		
		i3TDKListCtrl_Impl& ctrl_Impl = (m_bItemWeaponPef) ? m_WeaponListViewCtrl : m_ItemListViewCtrl;

		if (ctrl_Impl.IsNationFileListEmpty())
		{
			ctrl_Impl.LookForNationFile();
		}

		if (ctrl_Impl.IsNationFileListEmpty() == false)
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_ADDKEY_MULTINATION, "새 키값 - 멀티국가 &N");

		if( hSel != NULL && !ItemListMenu)
		{
			pDataMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");

			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_INT32, "정수형");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_REAL32, "실수형");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_STRING, "문자열");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC2D, "벡터형(2D)");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC3D, "벡터형(3D)");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC4D, "벡터형(4D)");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_COLOR, "색상");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_MATRIX, "매트릭스형");
		}
	}

	cmd = pKeyMenu->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd*) this, NULL);

	delete( pDataMenu);
	delete( pKeyMenu);

	CommandRClickTree( cmd, hSel );

	*pResult = 0;
}

void i3TDKRegistryCtrl_Impl::UpdateSelectedListCtrlItem(bool* rstStamp)
{
	if(!m_FocusListCtrl)
		return;

	bool bStamp = false;
	INT32 nSelected = m_FocusListCtrl->GetSelectedCount();

	if( 0 < nSelected )
	{
		m_conSelected.clear();

		POSITION pt = m_FocusListCtrl->GetFirstSelectedItemPosition();
		for( INT32 nItem = 0; nItem < nSelected; ++nItem )
		{
			INT32 nSel_Item =  m_FocusListCtrl->GetNextSelectedItem( pt );

 			i3RegData* pSrcData = (i3RegData*)m_FocusListCtrl->GetItemData( nSel_Item);
 			if( NULL != pSrcData )
 			{
				if( !bStamp )
					bStamp = m_FocusListCtrl->IsStampText( pSrcData->GetName() );
				
				m_conSelected.push_back( nSel_Item );
			}
		}
	}

	if(rstStamp)
		*rstStamp = bStamp;
}

void i3TDKRegistryCtrl_Impl::OnNMRClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult)
{
	m_FocusListCtrl = (i3TDKListCtrl_Impl *)FromHandle( pNMHDR->hwndFrom );

	UINT32	cmd;
	POINT	pt;
	LVHITTESTINFO lvh;
	
	//i3RegData * pData = NULL;
	GetCursorPos( &pt);

	bool bHit = false;
	bool bStamp = false;
	lvh.pt = pt;
	m_FocusListCtrl->ScreenToClient( &lvh.pt);

	m_FocusListCtrl->SubItemHitTest( &lvh);
	bHit = (lvh.flags & LVHT_ONITEM ? true : false);

	UpdateSelectedListCtrlItem(&bStamp);

	CMenu * pKeyMenu = new CMenu;
	CMenu * pDataMenu = new CMenu;

	{
		pKeyMenu->CreatePopupMenu();
		pDataMenu->CreatePopupMenu();
	}


	{//Data Menu
		if( m_pCurItem != NULL)
		{
			pKeyMenu->AppendMenu( MF_POPUP, (INT_PTR)pDataMenu->m_hMenu, "새로 만들기  &N");
			if( GetItemWeaponPef() && (m_FocusListCtrl == &m_ItemListViewCtrl) )
			{
				pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_WEAPON, "복합형");
			}
			else
			{
				pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_INT32, "정수형");
				pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_REAL32, "실수형");
				pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_STRING, "문자열");
				pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC2D, "벡터형(2D)");
				pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC3D, "벡터형(3D)");
				pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC4D, "벡터형(4D)");
				pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_COLOR, "색상");
				pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_MATRIX, "매트릭스형");
			}
			if( !m_conSelected.empty() )
			{
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_PASTEDATA, "값 붙여넣기 &V");
			}
		}
	}
	{//Key Menu
		if( bHit )
		{
			pKeyMenu->AppendMenu( MF_SEPARATOR, 0, "");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_COPYDATA, "값복사 &C");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DELETEDATA,"삭제 &D");

			if( m_FocusListCtrl->GetPefFileType() == PFT_MULTI 
				&& !bStamp 
				&& !(lvh.iSubItem < 2))
			{
				pKeyMenu->AppendMenu( MF_SEPARATOR, 0, "");
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DATA_COPY_LEFT, "Copy Left");
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DATA_COPY_RIGHT, "Copy Right");
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DATA_COPY_WHOLE, "Copy Whole");
				pKeyMenu->AppendMenu( MF_SEPARATOR, 0, "");
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DATA_COMPARE, "Compare Data");
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DATA_MATCH, "Match Data");
			}
		}
	}


	cmd = pKeyMenu->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd*) this, NULL);

	delete( pDataMenu);
	delete( pKeyMenu);

	CommandRClickList( cmd, lvh );

	*pResult = 0;
}

void i3TDKRegistryCtrl_Impl::SelectSubTreeFirstItem()
{
	m_SubTree.Select(m_pSubTreeFirstItem, TVGN_CARET);
}

void i3TDKRegistryCtrl_Impl::OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult)
{
	m_FocusTreeCtrl = (CTreeCtrl *)FromHandle( pNMHDR->hwndFrom );

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	i3RegKey * pKey = (i3RegKey*)m_FocusTreeCtrl->GetItemData( pNMTreeView->itemNew.hItem);

	GetFriendListPtr()->SetRegKey( pKey, CListSubCtrl::ACT_CHANGEKEY );

	if( GetItemWeaponPef() )
	{
		AfxGetMainFrame()->ResetStatusText2();

		if( m_FocusTreeCtrl == &m_TreeCtrl )
		{
			m_SubTree.DeleteAllItems();
			m_WeaponListViewCtrl.DeleteAllItems();
			m_ImageViewCtrl.ResetImage();

			m_CurrentWeaponType = "";

			HTREEITEM hCurSel = m_FocusTreeCtrl->GetNextItem(TVI_ROOT, TVGN_CARET);
			if (hCurSel != NULL)
				m_CurrentWeaponType = (LPCTSTR)m_FocusTreeCtrl->GetItemText(hCurSel);

			CString DisplayString;
			m_CurrentClassTypeIndex = GetClassTypeLookupIndex( m_CurrentWeaponType );

			if( m_CurrentClassTypeIndex == 0 )
				DisplayString = "Not registered";
			else
				DisplayString.Format( "_ClassType : %d", m_CurrentClassTypeIndex);

			AfxGetMainFrame()->SetStatusText1( DisplayString );

			m_pTreeCurItem = pNMTreeView->itemNew.hItem;
		}
		else if( m_FocusTreeCtrl == &m_SubTree )
		{
			DisplayWeaponImage(pKey);
			DisplayWeaponRegString(pKey);
		}
	}

	m_pCurItem	= pNMTreeView->itemNew.hItem;
}

void i3TDKRegistryCtrl_Impl::OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult)
{
	m_FocusTreeCtrl = (CTreeCtrl *)FromHandle( pNMHDR->hwndFrom );

	NMTVDISPINFO *pDispInfo = reinterpret_cast<NMTVDISPINFO*>(pNMHDR);
    
	i3RegKey * pKey = (i3RegKey*)pDispInfo->item.lParam;

	if( pDispInfo->item.pszText != NULL)
	{
		pKey->SetName( pDispInfo->item.pszText);
		HTREEITEM item = pDispInfo->item.hItem;

		m_FocusTreeCtrl->SetItemText( item, pDispInfo->item.pszText);

		if( GetItemWeaponPef() && m_FocusTreeCtrl == &m_TreeCtrl )
		{
			std::string NewName = pDispInfo->item.pszText;
			RenameClassType( m_CurrentWeaponType, NewName );

			m_CurrentWeaponType = NewName;
		}
	}
    
	*pResult = 0;
}


LRESULT i3TDKRegistryCtrl_Impl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO*)wParam);
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void i3TDKRegistryCtrl_Impl::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event == I3_TDK_UPDATE_EDIT)
	{
		if( i3::kind_of<i3RegData*>(pInfo->m_pMeta)) //->IsTypeOf( i3RegData::static_meta()))
		{
			if(( m_nCurRow >= 0) && (m_ItemListViewCtrl.GetItemCount()) > m_nCurRow)
			{
				m_ItemListViewCtrl.UpdateItem( m_nCurRow);
			}
		}
	}
}










void i3TDKRegistryCtrl_Impl::ReSizeControl()
{
	if( !GetItemWeaponPef() )
	{
		m_TreeCtrl.MoveWindow( g_pProfile->m_SimpleTree );
		m_ItemListViewCtrl.MoveWindow( g_pProfile->m_SimpleList );
	}
	else
	{
		m_TreeCtrl.MoveWindow( g_pProfile->m_Tree1 );
		m_ItemListViewCtrl.MoveWindow( g_pProfile->m_List1 );
		m_SubTree.MoveWindow( g_pProfile->m_Tree2 );

		m_ImageViewCtrl.MoveWindow( g_pProfile->m_Image );
		
		m_WeaponListViewCtrl.MoveWindow( g_pProfile->m_List2 );
	}
}
















// CRegTreeCtrl message handlers
void	i3TDKRegistryCtrl_Impl::SetRoot( i3RegKey * pRoot)
{
	//if( m_pRoot == pRoot)
	//	return;
//

	//I3_REF_CHANGE( m_pRoot, pRoot);
	m_pRoot = pRoot;

	UpdateAll();
}

i3TDKListCtrl_Impl* i3TDKRegistryCtrl_Impl::GetFriendListPtr()
{
	if( m_FocusTreeCtrl && *m_FocusTreeCtrl == m_SubTree )
		return &m_WeaponListViewCtrl;
	
	return &m_ItemListViewCtrl;
}

CTreeCtrl* i3TDKRegistryCtrl_Impl::GetFriendTreePtr()
{
	if( m_FocusListCtrl && *m_FocusListCtrl == m_WeaponListViewCtrl )
		return &m_SubTree;
	
	return &m_TreeCtrl;
}

void	i3TDKRegistryCtrl_Impl::UpdateAll( void)
{
	m_ItemListViewCtrl.DeleteAllItems();
	m_WeaponListViewCtrl.DeleteAllItems();
	m_TreeCtrl.DeleteAllItems();
	m_SubTree.DeleteAllItems();
	m_mHitemList.clear();

	m_selectedTreeItem.clear();

	ClearWeaponDictionary();

	if( m_pRoot != NULL)
	{
		Rec_AddTree( m_TreeCtrl, TVI_ROOT, m_pRoot);
	}

	UpdateData();
}



//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//

typedef void (*SpreadProc)(i3RegKey * pKey, size_t Index1, size_t Index2);

void _SpreadRegKey(i3RegKey * pKey, size_t Index1, size_t Index2, SpreadProc Proc, bool spread = true);
void _AddRegProc(i3RegKey * pKey, size_t Index1, size_t Index2);	//Index1, Index2는 사용하지 않는다.
void _RemoveRegProc(i3RegKey * pKey, size_t Index1, size_t Index2);	//Index2는 사용하지 않는다.
void _CopyRegProc(i3RegKey * pKey, size_t Index1, size_t Index2);	//Index1(From), Index2(To)
void _InsertCopyRegProc(i3RegKey * pKey, size_t Index1, size_t Index2);	//Index1(From), Index2(To)
void _ResetRegProc(i3RegKey * pKey, size_t Index1, size_t Index2);	//Index2는 사용하지 않는다.
void _SortRegProc(i3RegKey * pKey, size_t Index1, size_t Index2);	//Index1, Index2는 사용하지 않는다.


void i3TDKRegistryCtrl_Impl::NotifyRegFileNation()
{
	m_ItemListViewCtrl.NotifyRegFileNation();
}

void i3TDKRegistryCtrl_Impl::AddRegNation(const std::string& NationName)
{
	_SpreadRegKey( m_pRoot, 0, 0, _AddRegProc );

	NationBank* List = m_ItemListViewCtrl.GetNationListRef();

	List->push_back( NationName );
}

void i3TDKRegistryCtrl_Impl::DelRegNation(const std::string& NationName)
{
	NationBank* List = m_ItemListViewCtrl.GetNationListRef();

	NationBank::iterator It = List->begin();
	NationBank::iterator End = List->end();

	size_t Index = 0;
	for(; It != End; ++It, ++Index )
	{
		if( *It == NationName )
			break;
	}

	List->erase( It );

	_SpreadRegKey( m_pRoot, Index, 0, _RemoveRegProc );
}

void i3TDKRegistryCtrl_Impl::CopyRegNation(const std::string& FromNation, 
										   const std::string& ToNation, bool BeginRoot)
{
	NationBank* List = m_ItemListViewCtrl.GetNationListRef();

	NationBank::iterator It = List->begin();
	NationBank::iterator End = List->end();

	size_t FromIndex = 0, ToIndex = 0;
	bool EndedFrom = false, EndedTo = false;
	for(; It != End; ++It )
	{
		if( !EndedFrom && *It != FromNation )
			++FromIndex;
		else
			EndedFrom = true;

		if( !EndedTo && *It != ToNation )
			++ToIndex;
		else
			EndedTo = true;
	}

	i3RegKey* pRegKey = NULL;

	if( BeginRoot )
		pRegKey = m_pRoot;
	else
	{
		if( !m_pCurItem )
			return;
		pRegKey = (i3RegKey *)m_TreeCtrl.GetItemData( m_pCurItem );
		if( !pRegKey )
			return;
	}

	_SpreadRegKey( pRegKey, FromIndex, ToIndex, _CopyRegProc, BeginRoot ? true : false  );
}

void i3TDKRegistryCtrl_Impl::CopyRegNation(size_t FromNation, size_t ToNation, bool BeginRoot)
{
	i3RegKey* pRegKey = NULL;

	if( BeginRoot )
		pRegKey = m_pRoot;
	else
	{
		if( !m_pCurItem )
			return;
		pRegKey = (i3RegKey *)m_TreeCtrl.GetItemData( m_pCurItem );
		if( !pRegKey )
			return;
	}

	_SpreadRegKey( pRegKey, FromNation, ToNation, _CopyRegProc, BeginRoot ? true : false  );
}

void i3TDKRegistryCtrl_Impl::InsertRegNation(const std::string& AddNation, const std::string& BaseNation)
{
	NationBank* List = m_ItemListViewCtrl.GetNationListRef();

	NationBank::iterator It = List->begin();
	NationBank::iterator End = List->end();

	size_t BaseIndex = 0;
	bool EndedBase = false;
	for(; It != End; ++It )
	{
		if( !EndedBase && *It != BaseNation )
			++BaseIndex;
		else
			EndedBase = true;
	}

	_SpreadRegKey( m_pRoot, BaseIndex, List->size(), _InsertCopyRegProc );

	List->push_back( AddNation );
}

void i3TDKRegistryCtrl_Impl::ResetRegNation(const std::string& NationName, bool BeginRoot)
{
	NationBank* List = m_ItemListViewCtrl.GetNationListRef();

	size_t size = List->size();
	size_t Index = 0;
	for(; Index<size; ++Index)
	{
		if( (*List)[Index] == NationName )
			break;
	}

	i3RegKey* pRegKey = NULL;

	if( BeginRoot )
		pRegKey = m_pRoot;
	else
	{
		if( !m_pCurItem )
			return;
		pRegKey = (i3RegKey *)m_TreeCtrl.GetItemData( m_pCurItem );
		if( !pRegKey )
			return;
	}
	
	_SpreadRegKey( pRegKey, Index, 0, _ResetRegProc, BeginRoot ? true : false );
}

int	i3TDKRegistryCtrl_Impl::GetRegNation(const std::string& NationName)
{
	NationBank* List = m_ItemListViewCtrl.GetNationListRef();

	size_t size = List->size();
	size_t Index = 0;
	int find_Index = -1;
	for(; Index<size; ++Index)
	{
		if( (*List)[Index] == NationName )
		{
			find_Index = Index;
			break;
		}
			
	}

	return find_Index;
}

void i3TDKRegistryCtrl_Impl::SortReg()
{
	_SpreadRegKey( m_pRoot, 0, 0, _SortRegProc );
}

void i3TDKRegistryCtrl_Impl::DirectCallCommandRClickList(i3TDKListCtrl_Impl* listCtrl, 
	UINT32 Cmd, LVHITTESTINFO* lvh) 
{ 
	if(!listCtrl)
		return;

	m_FocusListCtrl = listCtrl;

	UpdateSelectedListCtrlItem();

	LVHITTESTINFO tmplvh;
	memset(&tmplvh, 0, sizeof(LVHITTESTINFO));
	
	CommandRClickList(Cmd, lvh ? *lvh : tmplvh); 
}

void _SpreadRegKey(i3RegKey * pKey, size_t Index1, size_t Index2 , SpreadProc Proc, bool spread)
{
	(*Proc)(pKey, Index1, Index2);

	for(INT32 i=0; i<pKey->getChildCount(); i++)
	{
		if( spread )
			_SpreadRegKey( (i3RegKey*)pKey->getChild(i), Index1, Index2, Proc, spread);
	}
}

void _AddRegProc(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	if( pKey != NULL)
	{
		INT32	nItemCount = pKey->GetDataCount();
		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegArray* pArrayReg = (i3RegArray*)pKey->GetData(i);
			pArrayReg->addRegData( pArrayReg->createRegData() );
		}
	}
}

void _RemoveRegProc(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	if( pKey != NULL)
	{
		INT32	nItemCount = pKey->GetDataCount();
		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegArray* pArrayReg = (i3RegArray*)pKey->GetData(i);
			pArrayReg->removeRegData(Index1);
		}
	}
}

void _CopyRegProc(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	if( pKey != NULL)
	{
		INT32	nItemCount = pKey->GetDataCount();
		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegArray* pArrayReg = (i3RegArray*)pKey->GetData(i);

			switch( pArrayReg->getElementType() )
			{
			case I3REG_TYPE_INT32:	
				{
					INT32 val;
					(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_REAL32:	
				{
					REAL32 val;
					(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_STRING:	
				{
					i3::rc_wstring wstr;
					(*pArrayReg)[Index1]->GetValue( wstr );
					 pArrayReg->replaceRegData( Index2, wstr );
				}
				break;
			case I3REG_TYPE_VEC2D:	
				{
					VEC2D val;
					(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_VEC3D:
				{
					VEC3D val;
					(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_VEC4D:
				{
					VEC4D val;
					(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_COLOR:
				{
					I3COLOR val;
					(*pArrayReg)[Index1]->GetValue( &val );
					 pArrayReg->replaceRegData( Index2, val );
				}
				break;
			case I3REG_TYPE_MATRIX:	
				{
					//INT32 val;
					//(*pArrayReg)[Index1]->GetValue( &val );
					// pArrayReg->replaceRegData( Index2, val );
				}
				break;
			}
		}
	}
}

void _InsertCopyRegProc(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	_AddRegProc(pKey, 0, 0);
	_CopyRegProc(pKey, Index1, Index2);
}

void _ResetRegProc(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	if( pKey != NULL)
	{
		INT32	nItemCount = pKey->GetDataCount();
		for( INT32 i = 0; i < nItemCount; ++i)
		{
			i3RegArray* pArrayReg = (i3RegArray*)pKey->GetData(i);

			switch( pArrayReg->getElementType() )
			{
			case I3REG_TYPE_INT32:	
				{
					 pArrayReg->replaceRegData( Index1, 0 );
				}
				break;
			case I3REG_TYPE_REAL32:	
				{
					 pArrayReg->replaceRegData( Index1, 0.0f );
				}
				break;
			case I3REG_TYPE_STRING:	
				{
					 pArrayReg->replaceRegData( Index1, L"" );
				}
				break;
			case I3REG_TYPE_VEC2D:	
				{
					VEC2D val;
					val.x = 0.0f;
					val.y = 0.0f;
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_VEC3D:
				{
					VEC3D val;
					val.x = 0.0f;
					val.y = 0.0f;
					val.z = 0.0f;
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_VEC4D:
				{
					VEC4D val;
					val.x = 0.0f;
					val.y = 0.0f;
					val.z = 0.0f;
					val.w = 0.0f;
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_COLOR:
				{
					I3COLOR val;
					val.r = 0;
					val.g = 0;
					val.b = 0;
					val.a = 0;
					 pArrayReg->replaceRegData( Index1, val );
				}
				break;
			case I3REG_TYPE_MATRIX:	
				{
					//INT32 val;
					//(*pArrayReg)[Index1]->GetValue( &val );
					// pArrayReg->replaceRegData( Index2, val );
				}
				break;
			}
		}
	}
}


i3RegKey * _pSortKey;

int _Sort_GetItemCount()
{
	return _pSortKey->GetDataCount();
}

std::string _Sort_GetItemText(int inx)
{
	i3RegData *pData = _pSortKey->GetData(inx);

	return std::string( pData->GetName() );
}
void _Sort_Swap(int low, int high)
{
	i3::vector<i3RegData*>* List = _pSortKey->GetValueList();
	i3::swap( List->at( low ), List->at( high ) );
}

bool _Sort_IsStampText(const std::string & Str)
{
	if( Str.find("__") == 0 && Str.find( "Time" ) == Str.size() - 4 )
		return true;

	return false;
}

bool _Sort_Compare(const std::string& Str1, const std::string& Str2, BOOL bAscending)
{
	bool Stamp1, Stamp2;

	Stamp1 = _Sort_IsStampText(Str1);
	Stamp2 = _Sort_IsStampText(Str2);

	if( !Stamp1 && !Stamp2 )
	{
		return Str1 < Str2;
	}

	if( bAscending )
	{
		if( (Stamp1 && Stamp2) )
		{
			if( Str2 == "__Make Time" )
				return true;

			return false;
		}

		if( Stamp2 )
			return true;

		return false;
	}
	else
	{
		if( Stamp1 && Stamp2 )
		{
			if( Str1 == "__Make Time" )
				return true;

			return false;
		}

		if( Stamp1 )
			return true;

		return false;
	}

//	return true;
}

BOOL _Sort_Reg( int nCol, BOOL bAscending, int low /*= 0*/, int high /*= -1*/ )
{
	if( nCol >= _Sort_GetItemCount() )
		return FALSE;

	if( high == -1 ) high = _Sort_GetItemCount() - 1;
	int lo = low;
	int hi = high;
	std::string midItem, textItem;
	if( hi <= lo ) return FALSE;
	midItem = _Sort_GetItemText( low );
	// 인덱스들이 교차될때까지 리스트를 돕니다(Loop).
	while( lo <= hi )
	{
		if( bAscending )
		{
			while( lo < high )
			{
				textItem = _Sort_GetItemText( lo );
				if( textItem == midItem )		break;
				if( _Sort_Compare(textItem, midItem, bAscending) )	break;
				++lo;
			}
			
			while( hi > low )
			{
				textItem = _Sort_GetItemText( hi );
				if( textItem == midItem )		break;
				if( !_Sort_Compare(textItem, midItem, bAscending) )	break;
				--hi;
			}
		}
		else
		{
			while( lo < high )
			{
				textItem = _Sort_GetItemText( lo );
				if( textItem == midItem )		break;
				if( !_Sort_Compare(textItem, midItem, bAscending) )	break;
				++lo;
			}
			
			while( hi > low )
			{
				textItem = _Sort_GetItemText( hi );
				if( textItem == midItem )		break;
				if( _Sort_Compare(textItem, midItem, bAscending) )	break;
				--hi;
			}
		}

		// 만약 인덱스가 교차되지 않았다면 교환하고, 만약 아이템이 같지않다면,
		if( lo <= hi )
		{
			// 아이템이 같지 않을때만 교환한다.
			if( _Sort_GetItemText( lo ) != _Sort_GetItemText( hi ) )
			{
				_Sort_Swap(lo, hi);
			}
			++lo;
			--hi;
		}
	}

	// 만약 오른쪽 인덱스가 배열의 왼쪽 끝에 닿지 않았다면 왼쪽 구역을 
	// 정렬해야 한다.
	if( low < hi )
		_Sort_Reg( nCol, bAscending , low, hi);

	// 만약 왼쪽 인덱스가 배열의 오른쪽 끝에 닿지 않았다면 오른쪽 구역을
	// 정렬해야 한다.
	if( lo < high )
		_Sort_Reg( nCol, bAscending , lo, high );

	return TRUE;
}

void _SortRegProc(i3RegKey * pKey, size_t Index1, size_t Index2)
{
	_pSortKey = pKey;
	_Sort_Reg(0, FALSE, 0, -1);
}

BOOL i3TDKRegistryCtrl_Impl::PreTranslateMessage(MSG* pMsg)
{
	m_SubTreeToolTip.RelayEvent( pMsg );

	switch(pMsg->message)
	{
	case WM_KEYDOWN:

		switch(pMsg->wParam)
		{
		case VK_F2:
			{
				m_FocusTreeCtrl = (CTreeCtrl *)FromHandle(pMsg->hwnd);
				if(m_FocusTreeCtrl != NULL)
				{
					HTREEITEM hItem = m_FocusTreeCtrl->GetSelectedItem(); 
					if (hItem == NULL)break;

					_RenameKey(*m_FocusTreeCtrl, hItem);
				}
			}
			break;

		default: 
			break;
		}

		if (::GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			CEdit* edit = (CEdit *)FromHandle(pMsg->hwnd);

			if (edit)
			{
				if (pMsg->wParam == 'C')
				{
					edit->Copy();
				}
				else if (pMsg->wParam == 'V')
				{
					edit->Paste();
				}
				else if (pMsg->wParam == 'X')
				{
					edit->Cut();
				}
			}
		}

		break;
	}

	if(WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		if(GetFocus() == &m_TreeCtrl && m_hAccel && ::TranslateAccelerator(GetSafeHwnd(), m_hAccel, pMsg))
			return TRUE;
	}

	return CWnd::PreTranslateMessage(pMsg);
}

#define CON_REV_SIZE (64)

void i3TDKRegistryCtrl_Impl::_initSelected()
{
	m_conSelected.clear();
	m_conSelected.reserve(CON_REV_SIZE);
}

void i3TDKRegistryCtrl_Impl::_initTemp()
{
	_clearTemp();
	m_conTempRegData.reserve(CON_REV_SIZE);

}

void i3TDKRegistryCtrl_Impl::_clearTemp()
{
	ITR_REG_DATA _iter = m_conTempRegData.begin();
	while( _iter != m_conTempRegData.end() )
	{
		I3_SAFE_RELEASE( (*_iter) );
		(*_iter) = NULL;
		++_iter;
	}
	m_conTempRegData.clear();
}


static bool FindInStr(const char* cmp1, const char* cmp2, bool casesensitve=false)
{
	if(!cmp1 || !cmp2)
		return false;

	const char* cur = cmp1;

	while(cur && *cur)
	{		
		const char* cmpsrc = cur;
		const char* cmpdst = cmp2;

		bool innerDiff = false;
		while(*cmpdst)
		{
			if(!*cmpsrc) 
			{ 
				innerDiff = true; 
				break; 
			}

			char s = *cmpsrc++;
			char d = *cmpdst++;

			if(casesensitve)
			{
				if(s != d) 
				{ 
					innerDiff = true; 
					break; 
				}
			}
			else
			{
				if(toupper(s) != toupper(d)) 
				{ 
					innerDiff = true; 
					break; 
				}
			}
		}

		if(!innerDiff)
			return true;

		cur++;
	}

	return false;
}

static void RecursiveMakeTreeItemPathStr(char* bufTreePath, CTreeCtrl& treeCtrl, HTREEITEM hCurItem)
{
	if(hCurItem == NULL)
		return;

	char* curSt = bufTreePath;

	while(*curSt)
		curSt++;

	CString itemText = treeCtrl.GetItemText(hCurItem);
	INT32 numLen = itemText.GetLength();

	strncpy(curSt, itemText.GetBuffer(), numLen);
	curSt += numLen;

	*curSt++ = '|';
	*curSt = NULL;

	RecursiveMakeTreeItemPathStr(curSt, treeCtrl, treeCtrl.GetParentItem(hCurItem));
}

static void RecursiveAdjustRegArrayItemCount(CTreeCtrl& treeCtrl, HTREEITEM hCurItem, INT32 numAdjustCount, 
	std::vector<std::string>* rstAdjustedLog)
{
	if(numAdjustCount < 3)
		return;

	if(!hCurItem)
		return;

	if(hCurItem == TVI_ROOT)
		hCurItem = treeCtrl.GetChildItem(hCurItem);
	
	HTREEITEM hChild = hCurItem; 
	while(hChild)
	{
		char buf[MAX_PATH] = {0};

		TVITEM item;
		item.mask  = TVIF_TEXT | TVIF_HANDLE | TVIF_PARAM;
		item.hItem = hChild;
		item.pszText = buf;
		item.cchTextMax = MAX_PATH;

		if(treeCtrl.GetItem(&item))
		{
			i3RegKey* pKey = (i3RegKey*)item.lParam;
			
			if(pKey)
			{
				INT32 numCurKeyData = pKey->GetDataCount();

				for(INT32 i=0; i<numCurKeyData;i++)
				{
					i3RegData* pDat = pKey->GetData(i);
					char buf2[MAX_PATH] = {0};
					char bufTreePath[1024];

					if(pDat)
					{
						if(pDat->getDataType() == I3REG_TYPE_ARRAY)
						{
							i3RegArray* pRegAryDat = (i3RegArray*)pDat;

							INT32 numCurAry = pRegAryDat->getSize();
							bool adjusted = false;
							if(numCurAry > 2 && numCurAry > numAdjustCount)
							{
								INT32 delta = numCurAry - numAdjustCount;
								for(INT32 m=0; m<delta; m++)
									pRegAryDat->removeRegData(pRegAryDat->getSize()-1);

								I3ASSERT(pRegAryDat->getSize() == (size_t)numAdjustCount);
								adjusted = true;
							}

							if(rstAdjustedLog && adjusted)
							{
								bufTreePath[0] = NULL;
								RecursiveMakeTreeItemPathStr(bufTreePath, treeCtrl, hChild);

								sprintf_s(buf2, MAX_PATH, "[%s] [%s-%s] [cur=%d to %d]", bufTreePath, pKey->GetName(), pDat->GetName(), numCurAry, numAdjustCount);
								rstAdjustedLog->push_back(std::string(buf2));
							}
						}
					}
				}
			}
		}

		RecursiveAdjustRegArrayItemCount(treeCtrl, treeCtrl.GetChildItem(hChild), numAdjustCount, rstAdjustedLog);
		hChild = treeCtrl.GetNextItem(hChild, TVGN_NEXT);
	}
}


static void RecursiveCollectTreeItemByStr(std::vector<i3TDKRegistryCtrl_Impl::StrTreeItem>& rst, 
	CTreeCtrl& treeCtrl, HTREEITEM hCurItem, const char* str, bool findInPrefixOnly)
{
	if(!hCurItem)
		return;

	if(hCurItem == TVI_ROOT)
		hCurItem = treeCtrl.GetChildItem(hCurItem);

	HTREEITEM hChild = hCurItem; 
	while(hChild)
	{
		char buf[MAX_PATH] = {0};

		TVITEM item;
		item.mask  = TVIF_TEXT | TVIF_HANDLE;
		item.hItem = hChild;
		item.pszText = buf;
		item.cchTextMax = MAX_PATH;

		if(treeCtrl.GetItem(&item))
		{
			bool found = false;

			if(findInPrefixOnly)
			{
				size_t nLen = strlen(str);
				if(!strnicmp(item.pszText, str, nLen))
					found = true;
			}
			else
			{
				found = FindInStr(item.pszText, str);
			}

			if(found)
			{
				i3TDKRegistryCtrl_Impl::StrTreeItem rstItem;

				rstItem.m_str = item.pszText;
				rstItem.m_item = hChild;

				rst.push_back(rstItem);
			}
		}

		RecursiveCollectTreeItemByStr(rst, treeCtrl, treeCtrl.GetChildItem(hChild), str, findInPrefixOnly);
		hChild = treeCtrl.GetNextItem(hChild, TVGN_NEXT);
	}
}

void i3TDKRegistryCtrl_Impl::CollectTreeItemByStr(std::vector<i3TDKRegistryCtrl_Impl::StrTreeItem>& rst, 
	const char* str, bool findInPrefixOnly, HTREEITEM findFrom)
{
	RecursiveCollectTreeItemByStr(rst, m_TreeCtrl, findFrom, str, findInPrefixOnly);
}

void i3TDKRegistryCtrl_Impl::AdjustKeyRegDataCntToNationCount(std::vector<std::string>* rstAdjustedLog)
{	
	INT32 numNation = GetFriendListPtr()->GetNationListRef()->size();
	RecursiveAdjustRegArrayItemCount(m_TreeCtrl, TVI_ROOT, numNation, rstAdjustedLog);
}

void i3TDKRegistryCtrl_Impl::SelectItemInTreeCtrl(HTREEITEM hTreeItem)
{
	if(!hTreeItem)
		return;

	m_TreeCtrl.SelectItem(hTreeItem);
}

HTREEITEM i3TDKRegistryCtrl_Impl::GetSelectdTreeItem()
{
	return m_TreeCtrl.GetSelectedItem();
}

BOOL i3TDKRegistryCtrl_Impl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(HIWORD(wParam) == 1 && m_FocusTreeCtrl) // by AccelatorKey
	{
		WORD accelKeyID = LOWORD(wParam);
		HTREEITEM hSel = m_FocusTreeCtrl->GetSelectedItem();
		UINT32 cmd = 0;

		if(GetFocus() == &m_TreeCtrl)
		{
			switch(accelKeyID)
			{
			case ID_ACCEL_EDITING_VAL_COPY  : if(hSel) cmd = ID_MENU_COPYKEY; break;
			case ID_ACCEL_EDITING_VAL_PASTE : cmd = ID_MENU_PASTEKEY; break;
			case ID_ACCEL_EDITING_VAL_DEL   : if(hSel) cmd = ID_MENU_DELETEKEY;	break;
			default: break;
			}
		}

		if(cmd)
		{
			CommandRClickTree(cmd, hSel);
			return TRUE;
		}
	}

	return CWnd::OnCommand(wParam, lParam);
}
