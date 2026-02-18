// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKRegistryCtrl.h"
#include "i3TDKUtilShell.h"
#include "i3Base/itl/algorithm/sort.h"
#include "i3Base/string/ext/itoa.h"


/*
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

	//char* pszChar1 = nullptr;
	//char* pszChar2 = nullptr;
	i3RegString* pRegString1 = (i3RegString*)pRegKey1->FindDataByName( "__Make Time" );
	i3RegString* pRegString2 = (i3RegString*)pRegKey2->FindDataByName( "__Make Time" );
	
	//pRegKey1->GetData( "__Make Time", pszChar1 );
	//pRegKey2->GetData( "__Make Time", pszChar2 );

	if (pRegString1 == nullptr || pRegString2 == nullptr )//( pszChar1 == nullptr || pszChar1 == nullptr )
	{
		I3TRACE( "CompareTimeProc : nullptr ");
		return 0;
	}

	return strcmp( pRegString2->getValue(), pRegString1->getValue() );
}
*/
namespace
{

	bool	CompareNameProc(const i3TreeNode* pData1, const i3TreeNode* pData2 )
	{
		const i3RegKey* pRegKey1 =  static_cast<const i3RegKey*>(pData1);
		const i3RegKey* pRegKey2 =  static_cast<const i3RegKey*>(pData2);

		if( pRegKey1->hasName() == false)
			return false;

		if( pRegKey2->hasName() == false)
			return true;

		return (strcmp( pRegKey1->GetName(), pRegKey2->GetName() ) < 0);
	}

	bool	CompareTimeProc( const i3TreeNode* pData1, const i3TreeNode* pData2 )
	{
		const i3RegKey* pRegKey1 =  static_cast<const i3RegKey*>(pData1);
		const i3RegKey* pRegKey2 =  static_cast<const i3RegKey*>(pData2);

		if( pRegKey1->hasName() == false)
			return false;

		if( pRegKey2->hasName() == false)
			return true;

		//char* pszChar1 = nullptr;
		//char* pszChar2 = nullptr;
		i3RegString* pRegString1 = (i3RegString*)pRegKey1->FindDataByName( "__Make Time" );
		i3RegString* pRegString2 = (i3RegString*)pRegKey2->FindDataByName( "__Make Time" );

		//pRegKey1->GetData( "__Make Time", pszChar1 );
		//pRegKey2->GetData( "__Make Time", pszChar2 );

		if (pRegString1 == nullptr || pRegString2 == nullptr )//( pszChar1 == nullptr || pszChar1 == nullptr )
		{
			I3TRACE( "CompareTimeProc : nullptr ");
			return false;								// 동등성 비교함수로 치환할 경우엔 같은것을 리턴하면 안되며, false로 처리한다.
		}

		return pRegString2->getValue() < pRegString1->getValue() ;
	}

}



#define	REG_TREECTRL	101
#define REG_LISTCTRL	102
#define REG_LISTCTRL2	103

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

IMPLEMENT_DYNAMIC(i3TDKRegistryCtrl, CWnd)

i3TDKRegistryCtrl::~i3TDKRegistryCtrl()
{
	I3_SAFE_RELEASE( m_pRoot);
	I3_SAFE_RELEASE( m_SrcData );

	I3_SAFE_DELETE( m_pImageList);
}

BEGIN_MESSAGE_MAP(i3TDKRegistryCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( NM_RCLICK, REG_TREECTRL, OnNMRClickTree)
	ON_NOTIFY( TVN_SELCHANGED, REG_TREECTRL, OnTVNSelChanged)
	ON_NOTIFY( TVN_ENDLABELEDIT, REG_TREECTRL, OnTVNEndLabelEdit)
	ON_NOTIFY( NM_CLICK, REG_LISTCTRL, OnNMClickListCtrl)
	ON_NOTIFY( NM_RCLICK, REG_LISTCTRL, OnNMRClickListCtrl)
END_MESSAGE_MAP()

// i3RegistryCtrl message handlers
BOOL i3TDKRegistryCtrl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR	hCursor;
	HBRUSH	hBack;

	m_dwStyle = dwStyle;

	hCursor = LoadCursor( nullptr, IDC_ARROW);
	I3ASSERT( hCursor != nullptr);

	hBack = (HBRUSH)GetStockObject( WHITE_BRUSH);
	I3ASSERT( hBack != nullptr);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, nullptr);
	I3ASSERT( pszClass != nullptr);

	dwStyle	|= WS_CLIPCHILDREN;

	return CWnd::Create( pszClass, _T(""), dwStyle, rect, pParentWnd, nID, nullptr);
}

int i3TDKRegistryCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rect;
	GetClientRect( &rect);

	INT32	CX = rect.Width()/2;
	rect.right -= CX;

	{
		if( m_TreeCtrl.Create( m_dwStyle | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VISIBLE | 
								TVS_HASBUTTONS | TVS_HASLINES | TVS_EDITLABELS |
								TVS_INFOTIP | TVS_LINESATROOT,
								rect, this, REG_TREECTRL) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not Create TreeCtrl");
		}
	}

	{
		rect.left = rect.right;
		rect.right += CX;

		if( m_ListViewCtrl.Create( m_dwStyle | WS_BORDER | LVS_SORTASCENDING, rect, this, REG_LISTCTRL) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not Create ListView");
		}

		m_ListViewCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	{
		rect.top	= rect.Height() / 2;
		if( m_ListDataCtrl.Create( m_dwStyle | WS_BORDER | LVS_SORTASCENDING, rect, this, REG_LISTCTRL2) == FALSE)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not Create EditView");
		}

		m_ListDataCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	{
		m_pImageList			= i3TDK::GetSystemImageList();
		m_nIndexOfFirstTDKImage = i3TDK::AddTDKImageListTo( m_pImageList);

		m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3RegData::static_meta(), I3_TDK_UPDATE_EDIT);

	return 0;
}

void i3TDKRegistryCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	INT32	CX = (INT32)(cx * 0.33f);
	
	if( ::IsWindow( m_TreeCtrl))
	{
		m_TreeCtrl.SetWindowPos( nullptr, 1, 1, CX - 2, cy - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if( ::IsWindow( m_ListDataCtrl))
	{
		m_ListDataCtrl.SetWindowPos( nullptr, CX , 1 , cx - CX - 1, 126, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if( ::IsWindow( m_ListViewCtrl))
	{
		m_ListViewCtrl.SetWindowPos( nullptr, CX, 128, cx - CX - 1, cy - 129, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

// CRegTreeCtrl message handlers
void	i3TDKRegistryCtrl::SetRoot( i3RegKey * pRoot)
{
	if( m_pRoot == pRoot) return;

	I3_REF_CHANGE( m_pRoot, pRoot);

	UpdateAll();
}

void	i3TDKRegistryCtrl::SetTreeInfo( TVITEM * pInfo, i3RegKey * pKey, const TCHAR* szKeyName)
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
	pInfo->pszText= (LPTSTR)szKeyName;		// (LPSTR)pKey->GetName();
}

void	i3TDKRegistryCtrl::UpdateTree( HTREEITEM hItem, i3RegKey * pKey)
{
	UpdateData( TRUE);
}

HTREEITEM	i3TDKRegistryCtrl::AddTree( HTREEITEM hParent, i3RegKey * pKey)
{
	TVINSERTSTRUCT	data;
	HTREEITEM		hItem;

	data.hParent		= hParent;
	data.hInsertAfter	= TVI_LAST;

#ifdef _UNICODE
	i3::stack_wstring wstrKeyName;	i3::mb_to_utf16(pKey->GetName(), wstrKeyName);
	const wchar_t* szKeyName = wstrKeyName.c_str();
#else
	const char* szKeyName = pKey->GetName();
#endif

	SetTreeInfo( &data.item, pKey, szKeyName);

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

void	i3TDKRegistryCtrl::Rec_AddTree( HTREEITEM hParent, i3RegKey * pKey)
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
void	i3TDKRegistryCtrl::Rec_SortTree( i3RegKey* pKey, REGISTRYCTRL_SORT_TYPE sort_type )
{
	i3RegKey* pChild = nullptr;
  
	for ( int i =0; i < pKey->getChildCount(); i++ )
	{
		pChild = (i3RegKey*)pKey->getChild( i );
		
		if ( pChild->getChildCount() > 0 )
		{
			Rec_SortTree( pChild, sort_type );
		}
	}

	if( pChild != nullptr)
	{
		pChild = (i3RegKey*)pChild->getParent();

		if ( pChild != nullptr )
		{
			i3::vector<i3TreeNode*>& ChildList = pChild->getChildList();
			if ( sort_type == SORT_TYPE_KEY )
			{
//				pChildList->Sort( (COMPAREPROC)CompareNameProc );
				i3::sort(ChildList.begin(), ChildList.end(), CompareNameProc);

			}
			else if( sort_type == SORT_TYPE_TIME )
			{
//				pChildList->Sort( (COMPAREPROC)CompareTimeProc );		
				i3::sort(ChildList.begin(), ChildList.end(), CompareTimeProc);
			}
		}
	}
}

//+
void	i3TDKRegistryCtrl::Rec_CheckTimeStamp( i3RegKey* pKey )
{
	i3RegKey* pChild = nullptr;
  
	for ( int i =0; i < pKey->getChildCount(); i++ )
	{
		pChild = (i3RegKey*)pKey->getChild( i );

		if ( pChild->getChildCount() > 0 )
		{
			Rec_CheckTimeStamp( pChild );
		}

		if ( pChild->FindDataByName( "__Make Time" ) == nullptr || pChild->FindDataByName( "__Update Time" ) == nullptr )
		{
			pChild->AddData( "__Make Time", L"0" );
			pChild->AddData( "__Update Time", L"0" );
		}
	}
}

void	i3TDKRegistryCtrl::UpdateAll( void)
{
	m_ListViewCtrl.DeleteAllItems();
	m_TreeCtrl.DeleteAllItems();

	if( m_pRoot != nullptr)
	{
		Rec_AddTree( TVI_ROOT, m_pRoot);
	}

	UpdateData();
}


void	i3TDKRegistryCtrl::OnDelete( HTREEITEM hItem)
{
	i3RegKey * pKey = (i3RegKey*) m_TreeCtrl.GetItemData( hItem);
	i3RegKey * pParent;

	I3PRINTLOG(I3LOG_NOTICE,  "Delete Item");

	pParent = (i3RegKey*)pKey->getParent();
	pParent->RemoveChild( pKey);

	m_TreeCtrl.DeleteItem( hItem);
}

void i3TDKRegistryCtrl::OnNMRClickTree( NMHDR * pNMHDR, LRESULT * pResult)
{
	UINT32	cmd;
	POINT	pt;
	HTREEITEM hSel = nullptr;

	GetCursorPos( &pt);

	{
		POINT	cpt;
		UINT	flag;

		cpt = pt;
		m_TreeCtrl.ScreenToClient( &cpt);

		hSel = m_TreeCtrl.HitTest( cpt, &flag);

		if((hSel != nullptr) && (TVHT_ONITEM & flag))
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
		if( hSel != nullptr)
		{
			pKeyMenu->AppendMenu( MF_POPUP, (INT_PTR)pDataMenu->m_hMenu, _T("새로 만들기  &N") );
			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, _T("") );

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_COPYKEY, _T("복사") );
			if( m_SrcItem != nullptr)
			{
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_PASTEKEY, _T("키값 붙여넣기") );
			}

			if( m_SrcData != nullptr)
			{
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_PASTEDATA, _T("값 붙여넣기 &v"));
			}

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DELETEKEY, _T("삭제"));
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_RENAMEKEY, _T("이름 바꾸기"));

			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, _T("") );

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_IMPORTKEY, _T("키 가져오기"));
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORTKEY, _T("키 내보내기"));

			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORT_SQLCREATE, _T("SQL (CREATE) 내보내기"));
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPORT_SQLUPDATE, _T("SQL (UPDATE) 내보내기"));			

			pKeyMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, _T(""));
		}

		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_FINDKEY, _T("찾기.."));
		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_COLLAPSENODE, _T("축소"));
		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_EXPANDNODE, _T("확장"));
		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_SORT_KEY, _T("키 정렬"));
		pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_SORT_TIME, _T("시간 정렬"));
	}

	{//Data Menu
		pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_ADDKEY, _T("새 키값	&N"));

		if( hSel != nullptr)
		{
			pDataMenu->AppendMenu( MF_SEPARATOR, (INT_PTR)0, _T(""));

			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_INT32, _T("정수형"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_REAL32, _T("실수형"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_STRING, _T("문자열"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC2D, _T("벡터형(2D)"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC3D, _T("벡터형(3D)"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC4D, _T("벡터형(4D)"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_COLOR, _T("색상"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_MATRIX, _T("매트릭스형"));
		}
	}

	cmd = pKeyMenu->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd*) this, nullptr);

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

void i3TDKRegistryCtrl::OnNMRClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult)
{
	UINT32	cmd;
	POINT	pt;
	LVHITTESTINFO lvh;
	
	i3RegData * pData = nullptr;

	GetCursorPos( &pt);

	{
		POINT	cpt;
		cpt = pt;
		m_ListViewCtrl.ScreenToClient( &cpt);
		lvh.pt = cpt;
        
		m_ListViewCtrl.SubItemHitTest( &lvh);
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
		if( m_pCurItem != nullptr)
		{
			pKeyMenu->AppendMenu( MF_POPUP, (INT_PTR)pDataMenu->m_hMenu, _T("새로 만들기  &N") );

			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_INT32, _T("정수형"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_REAL32, _T("실수형"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_STRING, _T("문자열"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC2D, _T("벡터형(2D)"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC3D, _T("벡터형(3D)"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_VEC4D, _T("벡터형(4D)"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_COLOR, _T("색상"));
			pDataMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DTYPE_MATRIX, _T("매트릭스형"));

			if( m_SrcData != nullptr)
			{
				pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_PASTEDATA, _T("값 붙여넣기 &v"));
			}
		}
	}
	{//Key Menu

		
		if( pData != nullptr)
		{
			pKeyMenu->AppendMenu( MF_SEPARATOR, 0, _T("") );

			
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_COPYDATA, _T("값복사"));
			
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_RENAMEDATA, _T("이름 바꾸기 &r"));
			pKeyMenu->AppendMenu( MF_STRING, (INT_PTR)ID_MENU_DELETEDATA, _T("삭제 &d"));
		}
	}


	cmd = pKeyMenu->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd*) this, nullptr);

	delete( pDataMenu);
	delete( pKeyMenu);

	switch( cmd)
	{
	case 	ID_MENU_DELETEDATA:		_DeleteData(lvh.iItem);		break;
	case 	ID_MENU_RENAMEDATA:		_RenameData(lvh.iItem);		break;
	case	ID_MENU_COPYDATA:		_CopyData( lvh.iItem);		break;
	case	ID_MENU_PASTEDATA:		_PasteData();				break;

	case	ID_MENU_DTYPE_INT32:	_AddNewData(m_pCurItem, I3REG_TYPE_INT32);	break;
	case	ID_MENU_DTYPE_REAL32:	_AddNewData(m_pCurItem, I3REG_TYPE_REAL32);	break;
	case	ID_MENU_DTYPE_STRING:	_AddNewData(m_pCurItem,	I3REG_TYPE_STRING);	break;
	case	ID_MENU_DTYPE_VEC2D:	_AddNewData(m_pCurItem,	I3REG_TYPE_VEC2D);	break;
	case	ID_MENU_DTYPE_VEC3D:	_AddNewData(m_pCurItem, I3REG_TYPE_VEC3D);	break;
	case	ID_MENU_DTYPE_VEC4D:	_AddNewData(m_pCurItem, I3REG_TYPE_VEC4D);	break;
	case	ID_MENU_DTYPE_COLOR:	_AddNewData(m_pCurItem, I3REG_TYPE_COLOR);	break;
	case	ID_MENU_DTYPE_MATRIX:	_AddNewData(m_pCurItem, I3REG_TYPE_MATRIX);	break;
	default:
		break;
	}
	//
	*pResult = 0;
}

void i3TDKRegistryCtrl::OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	i3RegKey * pKey = (i3RegKey*)m_TreeCtrl.GetItemData( pNMTreeView->itemNew.hItem);

	m_ListViewCtrl.SetRegKey( pKey);
	m_pCurItem	= pNMTreeView->itemNew.hItem;
}

void i3TDKRegistryCtrl::OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTVDISPINFO *pDispInfo = reinterpret_cast<NMTVDISPINFO*>(pNMHDR);
    
	i3RegKey * pKey = (i3RegKey*)pDispInfo->item.lParam;

	if( pDispInfo->item.pszText != nullptr)
	{
#ifdef _UNICODE
		i3::stack_string strText;	i3::utf16_to_mb(pDispInfo->item.pszText, strText);
		const char* szText = strText.c_str();
#else
		const char* szText = pDispInfo->item.pszText;
#endif
		pKey->SetName( szText);

		HTREEITEM item = pDispInfo->item.hItem;

		m_TreeCtrl.SetItemText( item, pDispInfo->item.pszText);
	}
    
	*pResult = 0;
}

void	i3TDKRegistryCtrl::_AddNewKey(HTREEITEM hParentItem)
{
	i3RegKey * pParent = (i3RegKey *)m_TreeCtrl.GetItemData( hParentItem);
	I3ASSERT( pParent != nullptr);

	i3RegKey * pNew = i3RegKey::new_object_ref();
	pNew->SetName("New Key");

	pParent->AddChild( pNew);

	HTREEITEM hCurItem = AddTree( hParentItem, pNew);

	// 생성시간 추가
	_AddTimeStampData( hCurItem );
}

void	i3TDKRegistryCtrl::_DeleteKey(HTREEITEM hItem)
{
	i3RegKey * pKey = (i3RegKey *)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != nullptr);

	if( pKey->getNodeStyle() & I3REG_STYLE_ROOT)
	{
		AfxMessageBox( _T("제거할수 없는 노드입니다."), MB_OK, 0);
		return;
	}

	i3RegKey * pParent = (i3RegKey*)pKey->getParent();
	I3ASSERT( pParent != nullptr);

	pParent->RemoveChild( pKey);
	m_TreeCtrl.DeleteItem( hItem);
}

void	i3TDKRegistryCtrl::_RenameKey(HTREEITEM hItem)
{
	i3RegKey * pKey = (i3RegKey *)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != nullptr);

	::SendMessage( m_TreeCtrl.m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem);
}

void	i3TDKRegistryCtrl::_CopyKey( HTREEITEM hItem)
{
	m_SrcItem = hItem;
}

void	i3TDKRegistryCtrl::_PasteKey( HTREEITEM hItem)
{
	I3ASSERT( m_SrcItem != nullptr);
	I3ASSERT( hItem != nullptr);

	i3RegKey * pKey = (i3RegKey *)m_TreeCtrl.GetItemData( hItem);
	I3ASSERT( pKey != nullptr);

	i3RegKey * pSrcKey = ( i3RegKey*)m_TreeCtrl.GetItemData( m_SrcItem);
	I3ASSERT( pSrcKey != nullptr);

	i3RegKey * pNew = (i3RegKey*)i3RegKey::CreateClone( pSrcKey, I3_COPY_INSTANCE);

	pKey->AddChild( pNew);

	Rec_AddTree( hItem, pNew);
}

void	i3TDKRegistryCtrl::_FindKey()
{

}

void	i3TDKRegistryCtrl::_Sort( REGISTRYCTRL_SORT_TYPE sorttype )
{
	Rec_SortTree( m_pRoot, sorttype );
	UpdateAll();
}

void	i3TDKRegistryCtrl::_GenerateTimeStamp()
{
	Rec_CheckTimeStamp( m_pRoot );
}

void	i3TDKRegistryCtrl::_AddNewData(HTREEITEM hParentItem, INT32 nNewDataType)
{
	i3RegKey * pParent = (i3RegKey *)m_TreeCtrl.GetItemData( hParentItem);
	I3ASSERT( pParent != nullptr);

	m_ListViewCtrl.SetRegKey( pParent);
	m_ListViewCtrl.AddNewData( nNewDataType);

	//+
	UpdateTimeStamp();
	m_ListViewCtrl.UpdateTimeStampData( _T("__Update Time"), m_szTimeStamp, m_szTimeStampString );
}

//+
void	i3TDKRegistryCtrl::_AddTimeStampData(HTREEITEM hParentItem )
{
	UpdateTimeStamp();

	i3RegKey * pParent = (i3RegKey *)m_TreeCtrl.GetItemData( hParentItem);
	I3ASSERT( pParent != nullptr);

	m_ListViewCtrl.SetRegKey( pParent);
	m_ListViewCtrl.AddTimeStampData( _T("__Make Time"), m_szTimeStamp, m_szTimeStampString );
	m_ListViewCtrl.AddTimeStampData( _T("__Update Time"), m_szTimeStamp, m_szTimeStampString );

}

void i3TDKRegistryCtrl::_CopyData( INT32 nRow)
{
	I3_SAFE_RELEASE( m_SrcData );

	i3RegData * pSrcData = (i3RegData *)m_ListViewCtrl.GetItemData( nRow);
	if( pSrcData != nullptr)
	{
		m_SrcData =	pSrcData->create_instance(); //	(i3RegData*)CREATEINSTANCE( pSrcData->GetMeta());
		pSrcData->CopyTo( m_SrcData, I3_COPY_INSTANCE);
		I3_MUST_ADDREF( m_SrcData);
	}
}

void i3TDKRegistryCtrl::_PasteData()
{
	if( m_SrcData != nullptr && m_pCurItem != nullptr)
	{
		m_ListViewCtrl.PasteData( m_SrcData);
	}
}

void	i3TDKRegistryCtrl::_RenameData( INT32 nRow)
{
	::SendMessage( m_ListViewCtrl.m_hWnd, LVM_EDITLABEL, (WPARAM)nRow, 0);
}

void	i3TDKRegistryCtrl::_DeleteData(INT32 nRow)
{
	m_ListViewCtrl.DeleteData( nRow);	
}

void	i3TDKRegistryCtrl::_CollapseNode( HTREEITEM item)
{
	m_TreeCtrl.Expand( item, TVE_COLLAPSE);
}

void	i3TDKRegistryCtrl::_ExpandNode(HTREEITEM item)
{
	m_TreeCtrl.Expand( item, TVE_EXPAND);
}

void	i3TDKRegistryCtrl::_ExprotSqlUpdate( HTREEITEM item )
{
	if( item == nullptr)	return;

	CFileDialog	Dlg( FALSE, _T("sql"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		_T("sql File(*.sql)|*.sql||"), this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3RegKey * pKey = (i3RegKey*)m_TreeCtrl.GetItemData( item);
	I3ASSERT( pKey != nullptr);

	i3RegKeyFile RegKeyFile;
	RegKeyFile.setRoot( pKey);

#ifdef _UNICODE
	i3::stack_string strFileName;	i3::utf16_to_mb(FileName, strFileName);
	const char* szFileName = strFileName.c_str();
#else
	const char* szFileName = FileName;
#endif

	RegKeyFile.OnExportSQLUpdate( szFileName ); 

	return; 
}

void	i3TDKRegistryCtrl::_ExprotSqlCreate( HTREEITEM item )
{
	if( item == nullptr)	return;

	CFileDialog	Dlg( FALSE, _T("sql"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		_T("sql File(*.sql)|*.sql||"), this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3RegKey * pKey = (i3RegKey*)m_TreeCtrl.GetItemData( item);
	I3ASSERT( pKey != nullptr);

	i3RegKeyFile RegKeyFile;
	RegKeyFile.setRoot( pKey);

#ifdef _UNICODE
	i3::stack_string strFileName;	i3::utf16_to_mb(FileName, strFileName);
	const char* szFileName = strFileName.c_str();
#else
	const char* szFileName = FileName;
#endif

	RegKeyFile.OnExportSQLCreate( szFileName ); 

	return; 
}

void	i3TDKRegistryCtrl::_ExportKey( HTREEITEM item)
{
	if( item == nullptr)	return;

	CFileDialog	Dlg( FALSE, _T("i3Rkf"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,  
		_T("i3RegKey File(*.i3Rkf)|*.i3Rkf||"), this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

	i3RegKey * pKey = (i3RegKey*)m_TreeCtrl.GetItemData( item);
	I3ASSERT( pKey != nullptr);

	i3RegKeyFile RegKeyFile;
	RegKeyFile.setRoot( pKey);

	i3ResourceFile File;
//	List list; 
//	list.Add( &RegKeyFile);
	i3::vector<i3PersistantElement*>	list(1, &RegKeyFile);

	File.SetObjects( list);

#ifdef _UNICODE
	i3::stack_string strFileName;	i3::utf16_to_mb(FileName, strFileName);
	const char* szFileName = strFileName.c_str();
#else
	const char* szFileName = FileName;
#endif

	if( File.Save( szFileName) == STREAM_ERR)
	{
		I3TRACE("Could not save Regkeyfile\n");
		return;
	}
}

void	i3TDKRegistryCtrl::_ImportKey( HTREEITEM item)
{
	CFileDialog	Dlg( TRUE, _T("i3Rkf"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		_T("i3RegKey File(*.i3Rkf)|*.i3Rkf||"), this);
	
	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	CString FileName = Dlg.GetPathName();

#ifdef _UNICODE
	i3::stack_string strFileName;	i3::utf16_to_mb(FileName, strFileName);
	const char* szFileName = strFileName.c_str();
#else
	const char* szFileName = FileName;
#endif

	i3ResourceFile File;

	if( File.Load( szFileName) == STREAM_ERR)
	{
		I3TRACE("Can't Load RegkeyFile\n");
		return;
	}

	INT32 nCount = File.GetObjectCount();
	for( INT32 i = 0;i < nCount; ++i)
	{
		i3PersistantElement * pElem = File.GetObject( i);
		if( i3::same_of<i3RegKeyFile*>(pElem))
		{
			i3RegKeyFile * pRegKeyFile = (i3RegKeyFile*)pElem;
			i3RegKey * pRoot = pRegKeyFile->getRoot();
			if( pRoot != nullptr)
			{
				i3RegKey * pParentKey = (i3RegKey*)m_TreeCtrl.GetItemData( item);
				I3ASSERT( pParentKey != nullptr);

				pParentKey->AddChild( pRoot);
				Rec_AddTree( item, pRoot);

				I3_SAFE_ADDREF( pRegKeyFile);
				I3_SAFE_RELEASE( pRegKeyFile);
			}
		}
	}
}

void	i3TDKRegistryCtrl::OnNMClickListCtrl( NMHDR * pNMHDR, LRESULT * pResult)
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
		I3ASSERT( pData != nullptr);

		m_ListDataCtrl.SetData( pData);
		m_nCurRow = lvh.iItem;
	}
	
}

void i3TDKRegistryCtrl::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event == I3_TDK_UPDATE_EDIT)
	{
		if( i3::kind_of<i3RegData*>(pInfo->m_pMeta))
		{
			if(( m_nCurRow >= 0) && (m_ListViewCtrl.GetItemCount()) > m_nCurRow)
			{
				m_ListViewCtrl.UpdateItem( m_nCurRow);
			}
		}
	}
}

void i3TDKRegistryCtrl::UpdateTimeStamp()
{
	time_t osBinaryTime;  // C run-time time (defined in <time.h>)
	time( &osBinaryTime );

	CTime ct(osBinaryTime);
	i3::itoa( osBinaryTime, m_szTimeStamp, _countof(m_szTimeStamp) );

	int y,m,d,h,min,s;
	
	y=ct.GetYear();
	m=ct.GetMonth();
	d=ct.GetDay();
	h=ct.GetHour();
	min=ct.GetMinute();
	s=ct.GetSecond();

	i3::snprintf( m_szTimeStampString, _countof(m_szTimeStampString), 
					_T("%04d-%02d-%02d %02d:%02d:%02d"), y, m, d, h, min, s );
}

LRESULT i3TDKRegistryCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO*)wParam);
	}

	return CWnd::WindowProc(message, wParam, lParam);
}
