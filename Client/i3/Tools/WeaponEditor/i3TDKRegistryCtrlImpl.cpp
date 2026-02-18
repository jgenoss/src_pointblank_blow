// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKRegistryCtrlImpl.h"
#include "i3TDKUtilShell.h"
#include "i3Scene/i3RegINT32Ex.h"

static INT32	CompareNameProc( void* pData1, void* pData2 )
{
	i3RegKey* pRegKey1 =  static_cast<i3RegKey*>(pData1);
	i3RegKey* pRegKey2 =  static_cast<i3RegKey*>(pData2);

	if( pRegKey1->hasName() == false)
		return 1;

	if( pRegKey2->hasName() == false)
		return -1;

	return strcmp( pRegKey1->GetName(), pRegKey2->GetName() );
}

static INT32	CompareTimeProc( void* pData1, void* pData2 )
{
	i3RegKey* pRegKey1 =  static_cast<i3RegKey*>(pData1);
	i3RegKey* pRegKey2 =  static_cast<i3RegKey*>(pData2);

	if( pRegKey1->hasName() == false)
		return 1;

	if( pRegKey2->hasName() == false)
		return -1;

	//char* pszChar1 = NULL;
	//char* pszChar2 = NULL;
	i3RegString* pRegString1 = (i3RegString*)pRegKey1->FindDataByName( "__Make Time" );
	i3RegString* pRegString2 = (i3RegString*)pRegKey2->FindDataByName( "__Make Time" );
	
	//pRegKey1->GetData( "__Make Time", pszChar1 );
	//pRegKey2->GetData( "__Make Time", pszChar2 );

	if (pRegString1 == NULL || pRegString2 == NULL )//( pszChar1 == NULL || pszChar1 == NULL )
	{
		I3TRACE( "CompareTimeProc : NULL ");
		return 0;
	}

	return strcmp( pRegString2->getValue(), pRegString1->getValue() );
}


#define	REG_TREECTRL	101
#define REG_LISTCTRL	102
#define REG_LISTCTRL2	103
#define REG_LISTCTRL3	104
#define REG_IMAGE_VIEW	105

enum 
{
	ID_MENU_ADDKEY = 100,
	ID_MENU_DELETEKEY,
	ID_MENU_COPYKEY,
	ID_MENU_PASTEKEY,
	ID_MENU_RENAMEKEY,
	ID_MENU_FINDKEY,
	ID_MENU_DELETEDATA,
	ID_MENU_EDITDATA,
	ID_MENU_RENAMEDATA,
	ID_MENU_COPYDATA,
	ID_MENU_PASTEDATA,
	ID_MENU_COLLAPSENODE,
	ID_MENU_EXPANDNODE,
	ID_MENU_IMPORTKEY,
	ID_MENU_EXPORTKEY,
	ID_MENU_DTYPE_INT32,
	ID_MENU_DTYPE_REAL32,
	ID_MENU_DTYPE_STRING,
	ID_MENU_DTYPE_VEC2D,
	ID_MENU_DTYPE_VEC3D,
	ID_MENU_DTYPE_VEC4D,
	ID_MENU_DTYPE_COLOR,
	ID_MENU_DTYPE_MATRIX,
	ID_MENU_DTYPE_INT32EX,
	ID_MENU_EXPORT_SQLCREATE,
	ID_MENU_EXPORT_SQLUPDATE,
	ID_MENU_SORT_KEY,
	ID_MENU_SORT_TIME,
}; 

// i3RegistryCtrl
enum
{
	I3ICON_ANIM			= 0,
	I3ICON_ATTR			= 2,
	I3ICON_BONE			= 4,
	I3ICON_CAMERA		= 6,
	I3ICON_GEOMETRY		= 8,
	I3ICON_NODE			= 10,
	I3ICON_RESOURCE		= 12,
	I3ICON_SOUND		= 14,
	I3ICON_TEXTURE		= 16,
};

IMPLEMENT_DYNAMIC(i3TDKRegistryCtrlImpl, CWnd)
i3TDKRegistryCtrlImpl::i3TDKRegistryCtrlImpl()
{
	m_dwStyle = 0;

	m_pImageList			= NULL;
	m_nIndexOfFirstTDKImage	= 0;	//TDK res에서 add된 icon의 첫번째 index

	m_pRoot					= NULL;
	m_pCurItem				= NULL;
	m_nCurRow				= -1;

	m_SrcItem				= NULL;
	m_SrcData				= NULL;

	m_pCurWeaponKey			= NULL;
}

i3TDKRegistryCtrlImpl::~i3TDKRegistryCtrlImpl()
{
	I3_SAFE_RELEASE( m_pRoot);
	I3_SAFE_RELEASE( m_SrcData );

	I3_SAFE_DELETE( m_pImageList);
	I3_SAFE_RELEASE( m_pContext);
}

BEGIN_MESSAGE_MAP(i3TDKRegistryCtrlImpl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( NM_RCLICK, REG_TREECTRL, OnNMRClickTree)
	ON_NOTIFY( TVN_SELCHANGED, REG_TREECTRL, OnTVNSelChanged)
	ON_NOTIFY( TVN_ENDLABELEDIT, REG_TREECTRL, OnTVNEndLabelEdit)
	ON_NOTIFY( NM_CLICK, REG_LISTCTRL, OnNMClickListCtrl)
	ON_NOTIFY( NM_RCLICK, REG_LISTCTRL, OnNMRClickListCtrl)
	ON_NOTIFY( NM_RCLICK, REG_LISTCTRL3, OnNMRClickListCtrl3)
END_MESSAGE_MAP()

// i3RegistryCtrl message handlers
BOOL i3TDKRegistryCtrlImpl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
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

int i3TDKRegistryCtrlImpl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rect;
	GetClientRect( &rect);

	INT32	CX = rect.Width()/4;
	rect.right -= CX;
	//rect.right = 100;

	{
		if( m_TreeCtrl.Create( m_dwStyle | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VISIBLE | 
								TVS_HASBUTTONS | TVS_HASLINES | TVS_EDITLABELS |
								TVS_INFOTIP | TVS_LINESATROOT,
								rect, this, REG_TREECTRL) == FALSE)
		{
			I3FATAL( "Could not Create TreeCtrl");
		}
	}

	{
		rect.left = rect.right;
		rect.right += CX;

		if( m_ListViewCtrl.Create( m_dwStyle | WS_BORDER | LVS_SORTASCENDING, rect, this, REG_LISTCTRL) == FALSE)
		{
			I3FATAL( "Could not Create ListView");
		}

		m_ListViewCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);


		if( m_ListViewCtrl2.Create( m_dwStyle | WS_BORDER | LVS_SORTASCENDING, rect, this, REG_LISTCTRL3) == FALSE)
		{
			I3FATAL( "Could not Create ListView");
		}

		m_ListViewCtrl2.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	if( m_ImageView.Create( m_dwStyle | WS_BORDER, this, REG_IMAGE_VIEW) == FALSE)
	{
		I3FATAL( "Could not Create ListView");
	}

	{
		i3VideoInfo video;

		video.m_Width = 32;
		video.m_Height = 32;

		m_pContext = i3RenderContext::NewObject();
		m_pContext->Create( &video, m_hWnd);
	}

	m_ImageView.LoadImageFile("D:\\PointBlank\\PointBlank\\trunk\\Media\\Gui\\WeaponShape\\Weapon_Select0.i3i");


	{
		rect.top	= rect.Height() / 2;
		if( m_ListDataCtrl.Create( m_dwStyle | WS_BORDER | LVS_SORTASCENDING, rect, this, REG_LISTCTRL2) == FALSE)
		{
			I3FATAL( "Could not Create EditView");
		}

		m_ListDataCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	{
		m_pImageList			= i3TDK::GetSystemImageList();
		m_nIndexOfFirstTDKImage = i3TDK::AddTDKImageListTo( m_pImageList);

		m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3RegData::GetClassMeta(), I3_TDK_UPDATE_EDIT);

	return 0;
}

void i3TDKRegistryCtrlImpl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	// INT32	CX = (INT32)(cx * 0.33f);
	INT32	CX = (INT32)(cx * 0.13f);
	INT32	CX2 = (INT32)(cx * 0.37f);
	
	if( ::IsWindow( m_TreeCtrl))
	{
		m_TreeCtrl.SetWindowPos( NULL, 1, 1, CX - 2, cy - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if( ::IsWindow( m_ListDataCtrl))
	{
		m_ListDataCtrl.SetWindowPos( NULL, CX , 1 , CX2 - 1, 126, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if( ::IsWindow( m_ListViewCtrl))
	{
		m_ListViewCtrl.SetWindowPos( NULL, CX, 128, CX2 - 1, cy - 129, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if( ::IsWindow( m_ListViewCtrl2))
	{
		m_ListViewCtrl2.SetWindowPos( NULL, CX + CX2, 128, cx - CX -CX2 - 1, cy - 129, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if( ::IsWindow( m_ImageView))
	{
		m_ImageView.SetWindowPos( NULL, CX + CX2, 1, cx - CX -CX2 - 1, 126, SWP_NOACTIVATE | SWP_NOZORDER);
	}

}

// CRegTreeCtrl message handlers
void	i3TDKRegistryCtrlImpl::SetRoot( i3RegKey * pRoot)
{
	if( m_pRoot == pRoot) return;

	I3_REF_CHANGE( m_pRoot, pRoot);

	UpdateAll();
}

void	i3TDKRegistryCtrlImpl::SetTreeInfo( TVITEM * pInfo, i3RegKey * pKey)
{
	memset( pInfo, 0, sizeof( TVITEM));

	pInfo->mask				= TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;
	pInfo->cChildren		= 0;

	if( pKey->getNodeStyle() & I3REG_STYLE_ROOT)
	{
		pInfo->iImage			= m_nIndexOfFirstTDKImage + I3ICON_NODE;
		pInfo->iSelectedImage	= m_nIndexOfFirstTDKImage + I3ICON_NODE + 1;
	}
	else
	{
		pInfo->iImage			= m_nIndexOfFirstTDKImage + I3ICON_GEOMETRY;
		pInfo->iSelectedImage	= m_nIndexOfFirstTDKImage + I3ICON_GEOMETRY + 1;
	}


	pInfo->lParam = ( LPARAM) pKey;
	pInfo->pszText= (LPSTR)pKey->GetName();
}

void	i3TDKRegistryCtrlImpl::UpdateTree( HTREEITEM hItem, i3RegKey * pKey)
{
	UpdateData( TRUE);
}

HTREEITEM	i3TDKRegistryCtrlImpl::AddTree( HTREEITEM hParent, i3RegKey * pKey)
{
	TVINSERTSTRUCT	data;
	HTREEITEM		hItem;

	data.hParent		= hParent;
	data.hInsertAfter	= TVI_LAST;

	SetTreeInfo( &data.item, pKey);

	hItem	= m_TreeCtrl.InsertItem( &data);

	if( hParent != TVI_ROOT)
	{
		TVITEM item;
		item.hItem = hParent;
		item.mask	= TVIF_CHILDREN;

		if( m_TreeCtrl.GetItem( &item))
		{
			item.cChildren = 1;
			m_TreeCtrl.SetItem(&item);
		}
	}

	return hItem;
}

void	i3TDKRegistryCtrlImpl::Rec_AddTree( HTREEITEM hParent, i3RegKey * pKey)
{
	HTREEITEM hItem;
	INT32		i;
	i3RegKey * pChild;

	hItem	= AddTree( hParent, pKey);

	for ( i =0; i < pKey->getChildCount(); i++)
	{
		pChild = (i3RegKey*)pKey->getChild( i);

		Rec_AddTree( hItem, pChild);
	}
}

//+
void	i3TDKRegistryCtrlImpl::Rec_SortTree( i3RegKey* pKey, REGISTRYCTRL_SORT_TYPE sort_type )
{
	i3RegKey* pChild = NULL;
  
	for ( int i =0; i < pKey->getChildCount(); i++ )
	{
		pChild = (i3RegKey*)pKey->getChild( i );
		
		if ( pChild->getChildCount() > 0 )
		{
			Rec_SortTree( pChild, sort_type );
		}
	}

	pChild = (i3RegKey*)pChild->getParent();

	if ( pChild != NULL )
	{
		i3List* pChildList = pChild->getChildList();
		if ( sort_type == SORT_TYPE_KEY )
		{
			pChildList->Sort( (COMPAREPROC)CompareNameProc );
		}
		else if( sort_type == SORT_TYPE_TIME )
		{
			pChildList->Sort( (COMPAREPROC)CompareTimeProc );						
		}
	}
}

//+
void	i3TDKRegistryCtrlImpl::Rec_CheckTimeStamp( i3RegKey* pKey )
{
	i3RegKey* pChild = NULL;
  
	for ( int i =0; i < pKey->getChildCount(); i++ )
	{
		pChild = (i3RegKey*)pKey->getChild( i );

		if ( pChild->getChildCount() > 0 )
		{
			Rec_CheckTimeStamp( pChild );
		}

		if ( pChild->FindDataByName( "__Make Time" ) == NULL || pChild->FindDataByName( "__Update Time" ) == NULL )
		{
			pChild->AddData( "__Make Time", "0" );
			pChild->AddData( "__Update Time", "0" );
		}
	}
}

void	i3TDKRegistryCtrlImpl::UpdateAll( void)
{
	m_ListViewCtrl.DeleteAllItems();
	m_ListViewCtrl2.DeleteAllItems();
	m_TreeCtrl.DeleteAllItems();

	if( m_pRoot != NULL)
	{
		Rec_AddTree( TVI_ROOT, m_pRoot);
	}

	UpdateData();
}


void	i3TDKRegistryCtrlImpl::OnDelete( HTREEITEM hItem)
{
	i3RegKey * pKey = (i3RegKey*) m_TreeCtrl.GetItemData( hItem);
	i3RegKey * pParent;

	I3NOTICE( "Delete Item");

	pParent = (i3RegKey*)pKey->getParent();
	pParent->RemoveChild( pKey);

	m_TreeCtrl.DeleteItem( hItem);
}

void i3TDKRegistryCtrlImpl::OnNMRClickTree( NMHDR * pNMHDR, LRESULT * pResult)
{
	UINT32	cmd;
	POINT	pt;
	HTREEITEM hSel = NULL;

	GetCursorPos( &pt);

	{
		POINT	cpt;
		UINT	flag;

		cpt = pt;
		m_TreeCtrl.ScreenToClient( &cpt);

		hSel = m_TreeCtrl.HitTest( cpt, &flag);

		if((hSel != NULL) && (TVHT_ONITEM & flag))
		{
			m_TreeCtrl.Select( hSel, TVGN_CARET);
		}
	}

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

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_COPYKEY, "복사");
			if( m_SrcItem != NULL)
			{
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_PASTEKEY, "키값 붙여넣기");
			}

			if( m_SrcData != NULL)
			{
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_PASTEDATA, "값 붙여넣기 &v");
			}

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DELETEKEY, "삭제");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_RENAMEKEY, "이름 바꾸기");

			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_IMPORTKEY, "키 가져오기");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORTKEY, "키 내보내기");

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORT_SQLCREATE, "SQL (CREATE) 내보내기");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORT_SQLUPDATE, "SQL (UPDATE) 내보내기");			

			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, "");
		}

		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_FINDKEY, "찾기..");
		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_COLLAPSENODE, "축소");
		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPANDNODE, "확장");
		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_SORT_KEY, "키 정렬");
		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_SORT_TIME, "시간 정렬");
	}

	{//Data Menu
		pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_ADDKEY, "새 키값	&N");

		if( hSel != NULL)
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
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_INT32EX, "무기확장형");
		}
	}

	cmd = pKeyMenu->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd*) this, NULL);

	delete( pDataMenu);
	delete( pKeyMenu);

	switch( cmd)
	{
	case 	ID_MENU_ADDKEY:			_AddNewKey(hSel);						break;
	case 	ID_MENU_DELETEKEY:		_DeleteKey(hSel);						break;
	case 	ID_MENU_RENAMEKEY:		_RenameKey(hSel);						break;

	case	ID_MENU_COPYKEY:		_CopyKey( hSel);						break;
	case	ID_MENU_PASTEKEY:		_PasteKey( hSel);						break;

	case 	ID_MENU_FINDKEY:		_FindKey();								break;
	case	ID_MENU_SORT_KEY:		_Sort( SORT_TYPE_KEY );					break;
	case	ID_MENU_SORT_TIME:		_Sort( SORT_TYPE_TIME );				break;

	case	ID_MENU_DTYPE_INT32:	_AddNewData(hSel,	I3REG_TYPE_INT32);	break;
	case	ID_MENU_DTYPE_REAL32:	_AddNewData(hSel,	I3REG_TYPE_REAL32);	break;
	case	ID_MENU_DTYPE_STRING:	_AddNewData(hSel,	I3REG_TYPE_STRING);	break;
	case	ID_MENU_DTYPE_VEC2D:	_AddNewData(hSel,	I3REG_TYPE_VEC2D);	break;
	case	ID_MENU_DTYPE_VEC3D:	_AddNewData(hSel,	I3REG_TYPE_VEC3D);	break;
	case	ID_MENU_DTYPE_VEC4D:	_AddNewData(hSel,	I3REG_TYPE_VEC4D);	break;
	case	ID_MENU_DTYPE_COLOR:	_AddNewData(hSel,	I3REG_TYPE_COLOR);	break;
	case	ID_MENU_DTYPE_MATRIX:	_AddNewData(hSel,	I3REG_TYPE_MATRIX);	break;
	case	ID_MENU_DTYPE_INT32EX:	_AddNewData(hSel,	I3REG_TYPE_INT32EX);	break;

	case 	ID_MENU_COLLAPSENODE:	_CollapseNode(hSel);	break;
	case 	ID_MENU_EXPANDNODE:		_ExpandNode(hSel);	break;

	case	ID_MENU_IMPORTKEY:		_ImportKey( hSel);	break;
	case	ID_MENU_EXPORTKEY:		_ExportKey( hSel);	break;
	case	ID_MENU_EXPORT_SQLCREATE: _ExprotSqlCreate(hSel); break; 
	case	ID_MENU_EXPORT_SQLUPDATE: _ExprotSqlUpdate(hSel); break; 

	case	ID_MENU_PASTEDATA:		_PasteData();	break;
	default:
		break;
	}
	//
	*pResult = 0;
}

void i3TDKRegistryCtrlImpl::ProcessCommonRClickOnList(i3TDKRegKeyCtrl& listViewCtrl, NMHDR * pNMHDR, LRESULT * pResult)
{
	UINT32	cmd;
	POINT	pt;
	LVHITTESTINFO lvh;

	i3RegData * pData = NULL;

	GetCursorPos( &pt);

	{
		POINT	cpt;
		cpt = pt;
		listViewCtrl.ScreenToClient( &cpt);
		lvh.pt = cpt;

		listViewCtrl.SubItemHitTest( &lvh);
		if( lvh.flags & LVHT_ONITEM)
		{
			pData = (i3RegData*)m_ListViewCtrl.GetItemData( lvh.iItem);
		}
	}

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

			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_INT32, "정수형");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_REAL32, "실수형");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_STRING, "문자열");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC2D, "벡터형(2D)");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC3D, "벡터형(3D)");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC4D, "벡터형(4D)");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_COLOR, "색상");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_MATRIX, "매트릭스형");
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_INT32EX, "무기확장형");

			if( m_SrcData != NULL)
			{
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_PASTEDATA, "값 붙여넣기 &v");
			}
		}
	}
	{//Key Menu


		if( pData != NULL)
		{
			pKeyMenu->AppendMenu( MF_SEPARATOR, 0, "");


			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_COPYDATA, "값복사");

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_RENAMEDATA,"이름 바꾸기 &r");
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DELETEDATA,"삭제 &d");
		}
	}


	cmd = pKeyMenu->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd*) this, NULL);

	delete( pDataMenu);
	delete( pKeyMenu);

	switch( cmd)
	{
	case 	ID_MENU_DELETEDATA:		_DeleteData(lvh.iItem);		break;
	case 	ID_MENU_RENAMEDATA:		_RenameData(lvh.iItem);		break;
	case	ID_MENU_COPYDATA:		_CopyData( lvh.iItem);		break;
	case	ID_MENU_PASTEDATA:		_PasteData();				break;

	case	ID_MENU_DTYPE_INT32:	_AddNewData(listViewCtrl, m_pCurItem, I3REG_TYPE_INT32);	break;
	case	ID_MENU_DTYPE_REAL32:	_AddNewData(listViewCtrl, m_pCurItem, I3REG_TYPE_REAL32);	break;
	case	ID_MENU_DTYPE_STRING:	_AddNewData(listViewCtrl, m_pCurItem,	I3REG_TYPE_STRING);	break;
	case	ID_MENU_DTYPE_VEC2D:	_AddNewData(listViewCtrl, m_pCurItem,	I3REG_TYPE_VEC2D);	break;
	case	ID_MENU_DTYPE_VEC3D:	_AddNewData(listViewCtrl, m_pCurItem, I3REG_TYPE_VEC3D);	break;
	case	ID_MENU_DTYPE_VEC4D:	_AddNewData(listViewCtrl, m_pCurItem, I3REG_TYPE_VEC4D);	break;
	case	ID_MENU_DTYPE_COLOR:	_AddNewData(listViewCtrl, m_pCurItem, I3REG_TYPE_COLOR);	break;
	case	ID_MENU_DTYPE_MATRIX:	_AddNewData(listViewCtrl, m_pCurItem, I3REG_TYPE_MATRIX);	break;
	case	ID_MENU_DTYPE_INT32EX:	_AddNewData(listViewCtrl, m_pCurItem, I3REG_TYPE_INT32EX);	break;
	default:
		break;
	}
	//
	*pResult = 0;
}


void i3TDKRegistryCtrlImpl::OnNMRClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult)
{
	ProcessCommonRClickOnList(m_ListViewCtrl, pNMHDR, pResult);
}

void i3TDKRegistryCtrlImpl::OnNMRClickListCtrl3( NMHDR * pNMHDR, LRESULT * pResult)
{
	ProcessCommonRClickOnList(m_ListViewCtrl2, pNMHDR, pResult);
}

void i3TDKRegistryCtrlImpl::OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	i3RegKey * pKey = (i3RegKey*)m_TreeCtrl.GetItemData( pNMTreeView->itemNew.hItem);

	m_ListViewCtrl.SetRegKey( pKey);
	m_pCurItem	= pNMTreeView->itemNew.hItem;
}

void i3TDKRegistryCtrlImpl::OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTVDISPINFO *pDispInfo = reinterpret_cast<NMTVDISPINFO*>(pNMHDR);
    
	i3RegKey * pKey = (i3RegKey*)pDispInfo->item.lParam;

	if( pDispInfo->item.pszText != NULL)
	{
		pKey->SetName( pDispInfo->item.pszText);
		HTREEITEM item = pDispInfo->item.hItem;

		m_TreeCtrl.SetItemText( item, pDispInfo->item.pszText);
	}
    
	*pResult = 0;
}

void	i3TDKRegistryCtrlImpl::_AddNewKey(HTREEITEM hParentItem)
{
	i3RegKey * pParent = (i3RegKey *)m_TreeCtrl.GetItemData( hParentItem);
	I3ASSERT( pParent != NULL);

	i3RegKey * pNew = i3RegKey::NewObjectRef();
	pNew->SetName("New Key");

	pParent->AddChild( pNew);

	HTREEITEM hCurItem = AddTree( hParentItem, pNew);

	// 생성시간 추가
	_AddTimeStampData( hCurItem );
}

void	i3TDKRegistryCtrlImpl::_DeleteKey(HTREEITEM hItem)
{
	i3RegKey * pKey = (i3RegKey *)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != NULL);

	if( pKey->getNodeStyle() & I3REG_STYLE_ROOT)
	{
		AfxMessageBox( "제거할수 없는 노드입니다.", MB_OK, 0);
		return;
	}

	i3RegKey * pParent = (i3RegKey*)pKey->getParent();
	I3ASSERT( pParent != NULL);

	pParent->RemoveChild( pKey);
	m_TreeCtrl.DeleteItem( hItem);
}

void	i3TDKRegistryCtrlImpl::_RenameKey(HTREEITEM hItem)
{
	i3RegKey * pKey = (i3RegKey *)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != NULL);

	::SendMessage( m_TreeCtrl.m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem);
}

void	i3TDKRegistryCtrlImpl::_CopyKey( HTREEITEM hItem)
{
	m_SrcItem = hItem;
}

void i3TDKRegistryCtrlImpl::_UpdateDataTimeStamp(i3RegKey* pKey)
{
	INT32		i;
	i3RegKey * pChild;
	i3RegData *Data;

	//만든 시간을 검색해서 있는 경우 수정한다.
	Data = pKey->FindDataByName("__Make Time");
	if( Data )
	{
		UpdateTimeStamp();
		pKey->RemoveData( Data );
		pKey->AddData("__Make Time", m_szTimeStamp);

		Data = pKey->FindDataByName("__Update Time");
		if( Data )
		{
			pKey->RemoveData( Data );
			pKey->AddData("__Update Time", m_szTimeStamp);
		}
	}

	for ( i =0; i < pKey->getChildCount(); i++)
	{
		pChild = (i3RegKey*)pKey->getChild( i);
		//다시 호출한다.
		_UpdateDataTimeStamp(pChild);
	}
}

void	i3TDKRegistryCtrlImpl::_PasteKey( HTREEITEM hItem)
{
	I3ASSERT( m_SrcItem != NULL);
	I3ASSERT( hItem != NULL);

	i3RegKey * pKey = (i3RegKey *)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != NULL);

	i3RegKey * pSrcKey = ( i3RegKey*)m_TreeCtrl.GetItemData( m_SrcItem);
	I3ASSERT( pSrcKey != NULL);

	i3RegKey * pNew = (i3RegKey*)i3RegKey::CreateClone( pSrcKey, I3_COPY_INSTANCE);

	_UpdateDataTimeStamp(pNew);

	pKey->AddChild( pNew);

	Rec_AddTree( hItem, pNew);
}

void	i3TDKRegistryCtrlImpl::_FindKey()
{

}

void	i3TDKRegistryCtrlImpl::_Sort( REGISTRYCTRL_SORT_TYPE sorttype )
{
	Rec_SortTree( m_pRoot, sorttype );
	UpdateAll();
}

void	i3TDKRegistryCtrlImpl::_GenerateTimeStamp()
{
	Rec_CheckTimeStamp( m_pRoot );
}

void	i3TDKRegistryCtrlImpl::_AddNewData(i3TDKRegKeyCtrl& listViewCtrl, HTREEITEM hParentItem, INT32 nNewDataType)
{
	if(&m_ListViewCtrl2 == &listViewCtrl) 
	{
		listViewCtrl.SetRegKey( m_pCurWeaponKey);
		if(m_pCurWeaponKey == NULL)
		{
			return;
		}
	}
	else if(&m_ListViewCtrl == &listViewCtrl)
	{
		i3RegKey * pParent = (i3RegKey *)m_TreeCtrl.GetItemData( hParentItem);
		I3ASSERT( pParent != NULL);

		listViewCtrl.SetRegKey( pParent);

	}
	else
	{
		I3ASSERT_0;
	}

	
	listViewCtrl.AddNewData( nNewDataType);

	//+
	UpdateTimeStamp();
	listViewCtrl.UpdateTimeStampData( "__Update Time", m_szTimeStamp, m_szTimeStampString );
}

void	i3TDKRegistryCtrlImpl::_AddNewData(HTREEITEM hParentItem, INT32 nNewDataType)
{
	i3RegKey * pParent = (i3RegKey *)m_TreeCtrl.GetItemData( hParentItem);
	I3ASSERT( pParent != NULL);

	m_ListViewCtrl.SetRegKey( pParent);
	m_ListViewCtrl.AddNewData( nNewDataType);

	//+
	UpdateTimeStamp();
	m_ListViewCtrl.UpdateTimeStampData( "__Update Time", m_szTimeStamp, m_szTimeStampString );
}

//+
void	i3TDKRegistryCtrlImpl::_AddTimeStampData(HTREEITEM hParentItem )
{
	UpdateTimeStamp();

	i3RegKey * pParent = (i3RegKey *)m_TreeCtrl.GetItemData( hParentItem);
	I3ASSERT( pParent != NULL);

	m_ListViewCtrl.SetRegKey( pParent);
	m_ListViewCtrl.AddTimeStampData( "__Make Time", m_szTimeStamp, m_szTimeStampString );
	m_ListViewCtrl.AddTimeStampData( "__Update Time", m_szTimeStamp, m_szTimeStampString );
}

void i3TDKRegistryCtrlImpl::_CopyData( INT32 nRow)
{
	I3_SAFE_RELEASE( m_SrcData );

	i3RegData * pSrcData = (i3RegData *)m_ListViewCtrl.GetItemData( nRow);
	if( pSrcData != NULL)
	{
		m_SrcData = (i3RegData*)CREATEINSTANCE( pSrcData->GetMeta());
		pSrcData->CopyTo( m_SrcData, I3_COPY_INSTANCE);
		m_SrcData->AddRef();
	}
}

void i3TDKRegistryCtrlImpl::_PasteData()
{
	if( m_SrcData != NULL && m_pCurItem != NULL)
	{
		m_ListViewCtrl.PasteData( m_SrcData);
	}
}

void	i3TDKRegistryCtrlImpl::_RenameData( INT32 nRow)
{
	::SendMessage( m_ListViewCtrl.m_hWnd, LVM_EDITLABEL, (WPARAM)nRow, 0);
}

void	i3TDKRegistryCtrlImpl::_DeleteData(INT32 nRow)
{
	m_ListViewCtrl.DeleteData( nRow);	
}

void	i3TDKRegistryCtrlImpl::_CollapseNode( HTREEITEM item)
{
	m_TreeCtrl.Expand( item, TVE_COLLAPSE);
}

void	i3TDKRegistryCtrlImpl::_ExpandNode(HTREEITEM item)
{
	m_TreeCtrl.Expand( item, TVE_EXPAND);
}

void	i3TDKRegistryCtrlImpl::_ExprotSqlUpdate( HTREEITEM item )
{
	if( item == NULL)	return;

	CFileDialog	Dlg( FALSE, "sql", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"sql File(*.sql)|*.sql||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3RegKey * pKey = (i3RegKey*)m_TreeCtrl.GetItemData( item);
	I3ASSERT( pKey != NULL);

	i3RegKeyFile RegKeyFile;
	RegKeyFile.setRoot( pKey);
	RegKeyFile.OnExportSQLUpdate( FileName ); 

	return; 
}

void	i3TDKRegistryCtrlImpl::_ExprotSqlCreate( HTREEITEM item )
{
	if( item == NULL)	return;

	CFileDialog	Dlg( FALSE, "sql", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"sql File(*.sql)|*.sql||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3RegKey * pKey = (i3RegKey*)m_TreeCtrl.GetItemData( item);
	I3ASSERT( pKey != NULL);

	i3RegKeyFile RegKeyFile;
	RegKeyFile.setRoot( pKey);
	RegKeyFile.OnExportSQLCreate( FileName ); 

	return; 
}

void	i3TDKRegistryCtrlImpl::_ExportKey( HTREEITEM item)
{
	if( item == NULL)	return;

	CFileDialog	Dlg( FALSE, "i3Rkf", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		"i3RegKey File(*.i3Rkf)|*.i3Rkf||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3RegKey * pKey = (i3RegKey*)m_TreeCtrl.GetItemData( item);
	I3ASSERT( pKey != NULL);

	i3RegKeyFile RegKeyFile;
	RegKeyFile.setRoot( pKey);

	i3ResourceFile File;
	i3List list;
	list.Add( &RegKeyFile);

	File.SetObjects( &list);
	if( File.Save( FileName) == STREAM_ERR)
	{
		I3TRACE("Could not save Regkeyfile\n");
		return;
	}
}

void	i3TDKRegistryCtrlImpl::_ImportKey( HTREEITEM item)
{
	CFileDialog	Dlg( TRUE, "i3Rkf", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3RegKey File(*.i3Rkf)|*.i3Rkf||", this);
	
	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3ResourceFile File;
	if( File.Load( FileName) == STREAM_ERR)
	{
		I3TRACE("Can't Load RegkeyFile\n");
		return;
	}

	INT32 nCount = File.GetObjectCount();
	for( INT32 i = 0;i < nCount; ++i)
	{
		i3PersistantElement * pElem = File.GetObject( i);
		if( pElem->IsExactTypeOf( i3RegKeyFile::GetClassMeta()) == TRUE)
		{
			i3RegKeyFile * pRegKeyFile = (i3RegKeyFile*)pElem;
			i3RegKey * pRoot = pRegKeyFile->getRoot();
			if( pRoot != NULL)
			{
				i3RegKey * pParentKey = (i3RegKey*)m_TreeCtrl.GetItemData( item);
				I3ASSERT( pParentKey != NULL);

				pParentKey->AddChild( pRoot);
				Rec_AddTree( item, pRoot);

				pRegKeyFile->AddRef();
				I3_SAFE_RELEASE( pRegKeyFile);
			}
		}
	}
}

void	i3TDKRegistryCtrlImpl::OnNMClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult)
{
	//	선택된 아이템 처리
	CPoint pt;
	
	//	클릭된 좌표 변환
	GetCursorPos( &pt);
	m_ListViewCtrl.ScreenToClient( &pt);

	LVHITTESTINFO lvh;
	lvh.pt = pt;
	m_ListViewCtrl.SubItemHitTest( &lvh);

	if( lvh.flags & LVHT_ONITEM)
	{
		i3RegData * pData = (i3RegData*)m_ListViewCtrl.GetItemData( lvh.iItem);
		I3ASSERT( pData != NULL);

		if(pData->IsExactTypeOf( i3RegINT32Ex::GetClassMeta())) 
		{
			m_pCurWeaponKey = ((i3RegINT32Ex*)pData)->GetRegKey();
		}
		else 
		{
			m_pCurWeaponKey = NULL;
		}
		m_ListViewCtrl2.SetRegKey(m_pCurWeaponKey);
		m_ListDataCtrl.SetData( pData);
		m_nCurRow = lvh.iItem;
	}
	
}

void i3TDKRegistryCtrlImpl::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event == I3_TDK_UPDATE_EDIT)
	{
		if( pInfo->m_pMeta->IsTypeOf( i3RegData::GetClassMeta()))
		{
			if(( m_nCurRow >= 0) && (m_ListViewCtrl.GetItemCount()) > m_nCurRow)
			{
				m_ListViewCtrl.UpdateItem( m_nCurRow);
			}
		}
	}
}

void i3TDKRegistryCtrlImpl::UpdateTimeStamp()
{
	time_t osBinaryTime;  // C run-time time (defined in <time.h>)
	time( &osBinaryTime );

	CTime ct(osBinaryTime);
	_i64toa( osBinaryTime, m_szTimeStamp, 10 );

	int y,m,d,h,min,s;
	
	y=ct.GetYear();
	m=ct.GetMonth();
	d=ct.GetDay();
	h=ct.GetHour();
	min=ct.GetMinute();
	s=ct.GetSecond();

	sprintf( m_szTimeStampString, "%04d-%02d-%02d %02d:%02d:%02d", y, m, d, h, min, s );
}

LRESULT i3TDKRegistryCtrlImpl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO*)wParam);
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void CImageViewCtrl::SetScroll()
{


}

void CImageViewCtrl::OnDraw( HDC dc)
{
	CRect rt ; 

	//이미지의 크기가 윈도의 크기보다 작은경우 남는영역을 브러쉬로 채워준다.
	//ERASEBKGND사용시 깜빡거림을 해소하기위한 방편 ;;
	::SelectObject( dc, GetStockObject( NULL_PEN));
	::SelectObject( dc, GetStockObject( LTGRAY_BRUSH));

	if(m_pTexture == NULL)
	{
		Rectangle( dc, 0, 0, m_WndSize.cx + 1, m_WndSize.cy + 1);
		return;
	}


	if(m_Zoom * m_pTexture->GetWidth() < (REAL32)m_WndSize.cx)
	{
		rt.left = (int)( m_Zoom * m_pTexture->GetWidth());
		rt.right = m_WndSize.cx + 1;
		rt.top = 0 ; 
		rt.bottom = m_WndSize.cy + 1;
	}
	Rectangle( dc, rt.left, rt.top, rt.right, rt.bottom);

	if(m_Zoom * m_pTexture->GetHeight() < (REAL32)m_WndSize.cy)
	{
		rt.left = 0 ; 
		rt.right = m_WndSize.cy + 1;
		rt.top = (int)( m_Zoom * m_pTexture->GetHeight());
		rt.bottom = m_WndSize.cy + 1;
	}
	Rectangle( dc, rt.left, rt.top, rt.right, rt.bottom);


	//이미지 출력, 스크롤된 위치에 따라서 소스이미지의 스타트 포지션을 변경하여 이미지를 스크롤 시킴
	/*StretchBlt( dc, 0 , 0,
		m_WndSize.cx,
		m_WndSize.cy,
		m_MemDC ,
		(int)(m_posScroll.x / m_Zoom),
		(int)(m_posScroll.y / m_Zoom),
		(int)(m_WndSize.cx	/ m_Zoom),
		(int)(m_WndSize.cy	/ m_Zoom), 
		SRCCOPY);*/

	StretchBlt( dc, 0 , 0,
		m_WndSize.cx,
		m_WndSize.cy,
		m_MemDC ,
		0,
		0,
		1,
		1, 
		SRCCOPY);

	StretchBlt( dc, 0 , 0,
		330,
		130,
		m_MemDC ,
		0,
		0,
		330,
		130, 
		SRCCOPY);
}