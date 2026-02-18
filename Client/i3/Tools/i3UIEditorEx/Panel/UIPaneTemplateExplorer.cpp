#include "stdafx.h"
#include "../resource.h"
#include "../i3UIEditor.h"
#include "../MainFrm.h"
#include "../i3UIEditorView.h"

#include "UIPaneTemplateExplorer.h"

#include "../Dlg_SelTemplateType.h"

#include "../UIPaneDefines.h"
#include "../ScriptManager.h"

#include "i3Base/string/compare/generic_icompare.h"
#include "i3Base/string/compare/generic_is_iequal.h"

#define TOOLBAR_BUTTON_COUNT 12
#define TOOLBAR_HEIGHT	30
#define STATUSBAR_HEIGHT 75
#define FIND_HEIGHT		19

static CListCtrl* s_pListView = NULL;		//for CompareProc
static bool s_bAsc = true;

struct TBBUTTON_INFO
{
	TBBUTTON		tbButton;
	const char*		pszTooltip;
	UINT32			strID;
	const char*		pszName;
};	//not TBBUTTONINFO

static TBBUTTON_INFO s_tbButtons[TOOLBAR_BUTTON_COUNT] = 
{
	{ { 0, ID_TBBTN_TREEVIEW, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0, 0, 0, 0}, "Tree View", IDS_TREEVIEW, "Tree"},
	{ { 1, ID_TBBTN_ICONVIEW, TBSTATE_ENABLED|TBSTATE_CHECKED, TBSTYLE_CHECKGROUP, 0, 0, 0, 1}, "Icon View", IDS_ICONVIEW, "Icon"},
	{ { 2, ID_TBBTN_REPORTVIEW, TBSTATE_ENABLED, TBSTYLE_CHECKGROUP, 0, 0, 0, 2}, "Report View", IDS_REPORTVIEW, "Report"},
	{ { 5,	0, 0, BTNS_SEP/*=TBSTYLE_SEP*/, 0, 0, 0, -1}, "", 0, ""},
	
	{ { 3, ID_TBBTN_UPPERLEVEL, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 3}, "Upper Level", 0, "Go Up"},
	{ { 5,	0, 0, BTNS_SEP, 0, 0, 0, -1}, "", 0, ""},
	
	{ { 4, ID_TBBTN_NEWFILTER, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 4}, "New Filter", 0, "Add Filter"},
	{ { 5, ID_TBBTN_NEWTEMPLATE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 5}, "New Template", 0, "Add Template"},
	{ { 6, ID_TBBTN_REMOVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 6}, "Remove", 0, "Remove"},
	{ { 5,	0, 0, BTNS_SEP, 0, 0, 0, -1}, "", 0, ""},
	
	{ { 7, ID_TBBTN_EDIT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 7}, "Edit", 0, "Edit"},
	{ { 8, ID_TBBTN_EDITSCRIPT, TBSTATE_INDETERMINATE, TBSTYLE_BUTTON, 0, 0, 0, 8}, "EditScript", 0, "Edit Script"},
};

#define LISTVIEW_HEADER_COUNT 3

struct LVHEADER_INFO
{
	INT32 nWidth;
	//const char* pszTitle;
	char	pszTitle[16];
};

//auto sizing 안되나? 
static LVHEADER_INFO s_lvHeaders[LISTVIEW_HEADER_COUNT] = 
{
	{	140,	"Name"	},
	{	80,		"Type"	},
	{	150,	"ClassMeta"	},
};


//////////////////////////////
// Implementation
//////////////////////////////

IMPLEMENT_DYNAMIC(CUIPaneTemplateExplorer, CUIPaneBase)

CUIPaneTemplateExplorer::CUIPaneTemplateExplorer()
{
	m_pLibrary = NULL;
	
	// drag & drop in listview
	m_bDragItem = false;
	m_pDragImageList = NULL;
	m_nDragItem = -1;
	m_nDropTarget = -1;
	m_nOldTarget = -1;

	m_hDragItem = NULL;
	m_hDropTarget = NULL;
	m_hOldTarget = NULL;

	m_pRoot = NULL;
	m_pCur = NULL;		//Current Parent Filter

	m_bCut = false;

	m_bShowListView = TRUE;

	m_pCurTNode = NULL;

	m_bHovering = false;

	m_FindFilter = "";
}

CUIPaneTemplateExplorer::~CUIPaneTemplateExplorer()
{
	I3_SAFE_RELEASE( m_pLibrary);
}

BEGIN_MESSAGE_MAP(CUIPaneTemplateExplorer, CUIPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//From ListView
	ON_NOTIFY( LVN_KEYDOWN, ID_TE_CHILD_LISTVIEW, OnLVNKeydown)	//TE : TemplateExplorer
	ON_NOTIFY( LVN_COLUMNCLICK, ID_TE_CHILD_LISTVIEW, OnLVNColumnClick)
	ON_NOTIFY( LVN_ENDLABELEDIT, ID_TE_CHILD_LISTVIEW, OnLVNEndLabelEdit)
	ON_NOTIFY( LVN_BEGINDRAG, ID_TE_CHILD_LISTVIEW, OnLVNBeginDrag)
	ON_NOTIFY( LVN_ITEMCHANGED, ID_TE_CHILD_LISTVIEW, OnLVNItemChanged)
	ON_NOTIFY( NM_DBLCLK, ID_TE_CHILD_LISTVIEW, OnLVNNMDblClk)
	//From TreeView
	ON_NOTIFY( TVN_BEGINDRAG, ID_TE_CHILD_TREEVIEW, OnTVNBeginDrag)
	ON_NOTIFY( NM_DBLCLK, ID_TE_CHILD_TREEVIEW, OnTVNNMDblClk)
	ON_NOTIFY( TVN_ENDLABELEDIT, ID_TE_CHILD_TREEVIEW, OnTVNEndLabelEdit)
	ON_NOTIFY( TVN_SELCHANGED, ID_TE_CHILD_TREEVIEW, OnTVNSelChanged)
	ON_NOTIFY( NM_RCLICK, ID_TE_CHILD_TREEVIEW, OnTVNNMRClick)
	
	/*
	ON_NOTIFY( TVN_KEYDOWN,		ID_TEMPLATE_TREECTRL, OnTVNKeyDown)
	*/
	ON_WM_TIMER()
END_MESSAGE_MAP()

int CALLBACK _LVCompareProc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if( s_pListView == NULL)
		return 0;

	//lParam을 통해 실제 Item을 찾는다. 
	LVFINDINFO info;
	info.flags = LVFI_PARAM;
	
	info.lParam = lParam1;
	INT32 nItem1 = s_pListView->FindItem( &info);
	
	info.lParam = lParam2;
	INT32 nItem2 = s_pListView->FindItem( &info);
	
	I3ASSERT( nItem1 > -1 && nItem2 > -1);

	//StringCompare
	char buf1[MAX_PATH];
	s_pListView->GetItemText( nItem1, lParamSort, buf1, MAX_PATH);	//lParamSort == column idx

	char buf2[MAX_PATH];
	s_pListView->GetItemText( nItem2, lParamSort, buf2, MAX_PATH);

	INT32 nSign = s_bAsc ? 1 : -1;

	return nSign * i3::generic_icompare( buf1, buf2);
}

int CALLBACK _TVCompareProc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//TVITEM item1, item2;
	//item1.mask = TVIF_HANDLE;
	//item1.lParam = lParam1;
	//BOOL bRes = m_TreeView.GetItem( &item1);
	//I3ASSERT( bRes == TRUE);

	//item2.mask = TVIF_HANDLE;
	//item2.lParam = lParam2;
	//bRes = m_TreeView.GetItem( &item2);
	//I3ASSERT( bRes == TRUE);

	i3TreeNode* pNode1 = (i3TreeNode*)lParam1;
	i3TreeNode* pNode2 = (i3TreeNode*)lParam2;

	char str1[MAX_PATH];
	char str2[MAX_PATH];

	for( INT32 i=0; i< 2; i++)
	{
		i3TreeNode* pNode = (i == 0) ? pNode1 : pNode2;
		char* pStr = (i == 0) ? str1 : str2;
		
		if( i3::same_of<i3UILTreeNode_Template*>(pNode)) //->IsExactTypeOf( i3UILTreeNode_Template::static_meta()))
		{
			i3UILTreeNode_Template* pTNode = (i3UILTreeNode_Template*)pNode;
			i3UITemplate* pTemplate = pTNode->getTemplate();
			i3::string_ncopy_nullpad( pStr, pTemplate->meta()->class_name().c_str(), MAX_PATH);
		}
		else if( i3::same_of<i3UILTreeNode_Filter*>(pNode)) //->IsExactTypeOf( i3UILTreeNode_Filter::static_meta()))
		{
			i3::string_ncopy_nullpad( pStr, "", MAX_PATH);
		}
		else
		{
			I3ASSERT( 0);
		}
	}

	return i3::generic_icompare( str1, str2);

	//return _LVCompareProc( lParam1, lParam2, 2);
}

bool CUIPaneTemplateExplorer::_BuildTree( void)
{
	if( m_pLibrary == NULL)
	{
		m_pRoot = NULL;
		m_pCur = NULL;
	}
	else
	{
		m_pRoot = m_pLibrary->getNewLibRoot();
		m_pCur = m_pRoot;
	}

	return (m_pRoot != NULL);
}

void CUIPaneTemplateExplorer::SetUILibrary( i3UILibrary* pLib)
{
	I3_REF_CHANGE( m_pLibrary, pLib);

	_BuildTree();
		
	UpdateView();
}

int CUIPaneTemplateExplorer::OnCreate( LPCREATESTRUCT lpCreateStruct)
{
	if( CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect client;

	GetClientRect( &client);
	
	//Create Image LIst
	m_ImageListToolBar.Create( IDB_TEMPLATE_EXPLORER_TOOLBAR, 16, 9, RGB(255, 255, 255));
	CImageList* pImgListBig = UIGlobalRes::getDefBigImageList();
	CImageList* pImgListSmall = UIGlobalRes::getDefSmallImageList();

	//Create Toolbar
	{
		CRect rt;
		
		rt = client;
		rt.bottom = TOOLBAR_HEIGHT;

		//TBSTYLE_FLAT: SEP을 line으로 표시
		//TBSTYLE_AUTOSIZE: Button마다 다른 크기
		m_ToolBar.Create( WS_CHILD|WS_VISIBLE|WS_BORDER|TBSTYLE_TOOLTIPS/*|TBSTYLE_AUTOSIZE|TBSTYLE_FLAT*/, rt, this, ID_TE_CHILD_TOOLBAR);
		m_ToolBar.SetImageList( &m_ImageListToolBar);
		
		for( INT32 i=0; i< TOOLBAR_BUTTON_COUNT; i++)
		{
			m_ToolBar.AddButtons( 1, &s_tbButtons[i].tbButton);
		}
	}

	// Create Search
	{
		CRect rt;

		rt = client;

		rt.top = TOOLBAR_HEIGHT + 3;
		rt.bottom = rt.top + FIND_HEIGHT;

		m_ED_Find.Create( WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_LEFT, rt, this, IDC_ED_SEARCH);

		HFONT hFont = i3TDK::CreateDefFont( -11, "Tahoma");

		m_ED_Find.SetFont( CFont::FromHandle( hFont));
	}

	//Create StatusBar
	{
		CRect rt;
		
		rt = client;

		rt.top = rt.bottom - STATUSBAR_HEIGHT;
		m_StatusBar.Create( WS_CHILD|WS_BORDER|WS_VISIBLE|CCS_BOTTOM, rt, this, ID_TE_CHILD_STATUSBAR);
		//m_StatusBar.SetMinHeight();
	}

	//Create List View
	{
		CRect rt;
		GetClientRect( &rt);
		rt.top += TOOLBAR_HEIGHT + FIND_HEIGHT + (3 * 2);
		rt.bottom = rt.bottom - STATUSBAR_HEIGHT;

		//default: multi-select (cf.LVS_SINGLESEL)
		//LVS__SHAREIMAGELISTS: ImageList의 파괴를 직접 처리
		m_ListView.Create( WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_ICON|LVS_AUTOARRANGE|LVS_EDITLABELS|LVS_SHAREIMAGELISTS, 
							rt, this, ID_TE_CHILD_LISTVIEW);	
		m_ListView.SetImageList( pImgListBig, LVSIL_NORMAL);		//icon view
		m_ListView.SetImageList( pImgListSmall, LVSIL_SMALL);		//report view

		//Initialize List View
		_InitListView();

		s_pListView = &m_ListView;	//Sort 할때 필요
	}

	//Create Tree View
	{
		CRect rt;
		GetClientRect( &rt);
		rt.top += TOOLBAR_HEIGHT;
		rt.bottom = rt.bottom - STATUSBAR_HEIGHT;

		m_TreeView.Create( WS_CHILD|WS_VISIBLE|WS_BORDER|TVS_HASBUTTONS|TVS_HASLINES|TVS_EDITLABELS|TVS_SHOWSELALWAYS, 
							rt, this, ID_TE_CHILD_TREEVIEW);
		m_TreeView.SetImageList( pImgListSmall, TVSIL_NORMAL);

		_InitTreeView();
	}

	{
		i3TDK::RegisterUpdate( m_hWnd, i3UILibrary::static_meta(), I3_TDK_UPDATE_SET);
	}

	UpdateView();

	return 0;
}

//OnSize... 크기가 커질때와 작아질때 동작이 좀 다른듯.. ;;
void CUIPaneTemplateExplorer::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	CRect rt;
	GetClientRect( rt);
	INT32 y = 0;

	m_ToolBar.SetWindowPos( NULL, rt.left, y, rt.right, TOOLBAR_HEIGHT, SWP_NOACTIVATE| SWP_NOZORDER);// | SWP_NOZORDER);
	y += TOOLBAR_HEIGHT + 3;

	m_ED_Find.SetWindowPos( NULL, rt.left, y, rt.right, FIND_HEIGHT, SWP_NOACTIVATE | SWP_NOZORDER);
	y += FIND_HEIGHT + 3;

	m_ListView.SetWindowPos( NULL, rt.left, y, rt.right, rt.bottom - STATUSBAR_HEIGHT - 1, SWP_NOACTIVATE| SWP_NOZORDER);

	m_TreeView.SetWindowPos( NULL, rt.left, y, rt.right, rt.bottom - STATUSBAR_HEIGHT - 1, SWP_NOACTIVATE| SWP_NOZORDER);

	//넉넉하게 50정도로 잡아도 알아서 사이즈 조정되는듯;
	m_StatusBar.SetWindowPos( NULL, rt.left, rt.bottom - STATUSBAR_HEIGHT, rt.right, STATUSBAR_HEIGHT, SWP_NOACTIVATE| SWP_NOZORDER);
}

BOOL CUIPaneTemplateExplorer::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	LPNMHDR hdr = (LPNMHDR)lParam;

	switch( hdr->code)
	{
	case TTN_GETDISPINFO:	//ToolTip
		for( INT32 i=0; i< TOOLBAR_BUTTON_COUNT; i++)
		{
			if( (INT32)wParam == s_tbButtons[i].tbButton.idCommand)
			{
				i3::string_ncopy_nullpad( ((LPNMTTDISPINFO)lParam)->lpszText, s_tbButtons[i].pszTooltip, 80);
				break;
			}
		}
		break;
	default:
		break;
	}

	return CDockablePane::OnNotify(wParam, lParam, pResult);
}

BOOL CUIPaneTemplateExplorer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD id = LOWORD(wParam);
	WORD code = HIWORD( wParam);

	switch( id)
	{
	case ID_TBBTN_TREEVIEW:		OnBtnTreeView();	break;
	case ID_TBBTN_ICONVIEW:		OnBtnIconView();	break;
	case ID_TBBTN_REPORTVIEW:		OnBtnReportView();	break;
	
	case ID_TBBTN_UPPERLEVEL:		OnBtnUpperLevel();	break;
	
	case ID_TBBTN_NEWFILTER:		OnBtnNewFilter();	break;
	case ID_TBBTN_NEWTEMPLATE:	OnBtnNewTemplate();	break;
	case ID_TBBTN_REMOVE:		OnBtnRemove();		break;

	case ID_TBBTN_EDIT:			_EditTemplate();		break;
	case ID_TBBTN_EDITSCRIPT:		OnBtnEditScript();	break;

	case IDC_ED_SEARCH :
		if( code == EN_CHANGE)
		{
			SetTimer( IDC_ED_SEARCH, 800, NULL);	break;
		}
		break;

	case ID_EDIT_COPY :			OnBtnCopy();		break;
	case ID_EDIT_CUT :			OnBtnCut();		break;
	case ID_EDIT_PASTE :		OnBtnPaste();		break;
	case ID_EDIT_DELETE :		OnBtnRemove();		break;

	default:		break;
	}

	return CDockablePane::OnCommand(wParam, lParam);
}


bool CUIPaneTemplateExplorer::_GetUniqueName( i3TreeNode* pNode, const char* pszDefaultName, char* pBuf, INT32 nBufSize)
{
	I3ASSERT( pNode != NULL);

	i3::string_ncopy_nullpad( pBuf, pszDefaultName, nBufSize);

	INT32 postfix=0;
	while( !_IsUniqueName( pNode, pBuf))
	{
		i3::snprintf( pBuf, nBufSize, "%s_%d", pszDefaultName, postfix);

		postfix++;
	}

	if( postfix == 0)
		return true;

	return false;
}

// pNode의 이름을 pszName으로 설정하는 것이 괜찮은지 검사
// Filter와 Template은 서로 Unique Name을 구하는 방식이 다르다..
// Template은 lib root에 대해 unique, Filter는 parent에 대해 unique
bool CUIPaneTemplateExplorer::_IsUniqueName( i3TreeNode* pNode, const char* pszName)
{
	if( pszName[0] == 0)
		return false;

	if( i3UILTreeNode_Filter* pFNode = ConvertToFilterNode( pNode))
	{
		if( pFNode->getParent() == NULL)
			return true;
		
		i3UILTreeNode_Filter* pParent = ConvertToFilterNode( pFNode->getParent());
		I3ASSERT( pParent != NULL);

		//check siblings
		for( INT32 i=0; i< pParent->getChildCount(); i++)
		{
			if( i3UILTreeNode_Filter* pChild = ConvertToFilterNode( pParent->getChild( i)))
			{
				if( pChild != pNode)
				{
					if( i3::generic_is_iequal( pChild->GetName(), pszName) )
					{
						return false;
					}
				}
			}
		}

		return true;
	}
	else if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
	{
		if( m_pLibrary == NULL)
		{
			m_pLibrary = i3UI::getUILibrary();
		}

		if( m_pLibrary == NULL)
			return true;

		i3UITemplate* pTemplate = m_pLibrary->FindByName( pszName);
		//if( pTemplate == NULL || pTemplate == pTNode->getTemplate())
		if( pTemplate == NULL)
			return true;
	}

	return false;
}

//Path를 String으로 만들어 pBuf로 out
void CUIPaneTemplateExplorer::_GetCurrentFilterPath( char* pBuf, INT32 nBufSize)
{
	char conv[MAX_PATH] = {0};
	pBuf[0] = 0;

	i3TreeNode* pCur = m_pCur;
	while( pCur != NULL)
	{
		i3::snprintf( pBuf, nBufSize, "%s\\%s", pCur->GetName(), conv);
		i3::string_ncopy_nullpad( conv, pBuf, sizeof( conv));

		pCur = pCur->getParent();
	}
}


void CUIPaneTemplateExplorer::_InitListView( void)
{
	// Header
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;

	for( INT32 i=0; i< LISTVIEW_HEADER_COUNT; i++)
	{
		col.cx = s_lvHeaders[i].nWidth;
		col.pszText = s_lvHeaders[i].pszTitle;	
		col.iSubItem = i;
		m_ListView.InsertColumn( i, &col);
	}
}

void CUIPaneTemplateExplorer::_InsertListViewItem( i3TreeNode* pNode)
{
	if( pNode == NULL)
		return;

	LVITEM item;
	item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;		
	item.iItem = m_ListView.GetItemCount();
	char szType[32];
	char szClassMeta[64];

	item.iImage = GetImageIndex( pNode);

	if( i3UILTreeNode_Filter* pFNode = ConvertToFilterNode( pNode))
	{
		i3::string_ncopy_nullpad( szType, "Filter", sizeof( szType));
		i3::string_ncopy_nullpad( szClassMeta, "", sizeof( szClassMeta));
	}
	else if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
	{
		//Template Node
		//item.iImage = 1;
		i3::string_ncopy_nullpad( szType, "Template", sizeof( szType));
		
		i3UITemplate* pTemplate = pTNode->getTemplate();
		
		I3ASSERT( pTemplate != NULL);
		//I3ASSERT( i3String::NCompare( pTemplate->GetName(), pNode->GetName(), MAX_PATH) == 0);
		/*if( _IsUniqueName( pNode, pTemplate->GetName()) )
		{
			char buf[ MAX_PATH] = {0,};
			_GetUniqueName( pTNode, pTNode->GetName(), buf, MAX_PATH);
			pNode->SetName( buf);

			::MessageBox( m_hWnd, "Duplicate name", "Rename", MB_OK );
		}
		else
			pNode->SetName( pTemplate->GetName());*/

		i3::string_ncopy_nullpad( szClassMeta, pTemplate->meta()->class_name().c_str(), sizeof( szClassMeta));

	}
	else
	{
		I3ASSERT( 0);
		return;
	}

	//Name
	item.iSubItem = 0;
	item.pszText = (char*)pNode->GetName();		// const char* -> LPSTR
	item.lParam = (LPARAM)pNode;	
	m_ListView.InsertItem( &item);

	item.mask &= ~LVIF_PARAM;	//LVIF_PARAM 이걸 빼줘야함

	//Type
	item.iSubItem = 1;
	item.pszText = szType;		//local variable인 경우에도 되네
	m_ListView.SetItem( &item);

	//ClassMeta
	item.iSubItem = 2;
	item.pszText = szClassMeta;		
	m_ListView.SetItem( &item);
}

void CUIPaneTemplateExplorer::_UpdateListView( void)
{
	m_ListView.DeleteAllItems();

	if( m_pCur == NULL)
		return;
	
	for( INT32 i=0; i< m_pCur->getChildCount(); i++)
	{
		i3TreeNode* pChild = m_pCur->getChild( i);
		_InsertListViewItem( pChild);
	}

	s_bAsc = true;
	m_ListView.SortItems( _LVCompareProc, 2);	// report view 에서만 동작?

	::SetFocus( m_ListView.m_hWnd);
}

void CUIPaneTemplateExplorer::_InitTreeView( void)
{
}

void CUIPaneTemplateExplorer::_RecInsertTreeItem( i3TreeNode* pNode, HTREEITEM hParent)
{
	if( pNode == NULL)
		return;

	bool bAdd = true;

	TVINSERTSTRUCT is;

	is.hParent = hParent;
	is.hInsertAfter = TVI_LAST;
	is.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_STATE;
	is.item.lParam = (LPARAM)pNode;
	is.item.state = TVIS_EXPANDED;
	is.item.stateMask = TVIS_EXPANDED;
	is.item.pszText = (char*)pNode->GetName();
	is.item.iImage = GetImageIndex( pNode);
	is.item.iSelectedImage = is.item.iImage + 1;

	if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
	{
		i3UITemplate* pTemplate = pTNode->getTemplate();
		
		I3ASSERT( pTemplate != NULL);
		
		pNode->SetName( pTemplate->GetName());

		if( m_FindFilter.IsEmpty() == false)
		{
			if( i3::icontain_string( pTemplate->GetName(), LPCTSTR( m_FindFilter)) == -1)
				bAdd = false;
		}
	}

	if( bAdd)
	{
		HTREEITEM hInsertedItem = m_TreeView.InsertItem( &is);
		I3ASSERT( hInsertedItem != NULL);

		for( INT32 i=0; i< pNode->getChildCount(); i++)
		{
			i3TreeNode* pChild = (i3TreeNode*)pNode->getChild( i);
			_RecInsertTreeItem( pChild, hInsertedItem);
		}
	}
}

void CUIPaneTemplateExplorer::_UpdateTreeView( void)
{
	m_TreeView.DeleteAllItems();

	if( m_pRoot == NULL)
		return;

	_RecInsertTreeItem( m_pRoot);

	//Sort
	TVSORTCB info;
	info.hParent = m_TreeView.GetRootItem();
	info.lpfnCompare = _TVCompareProc;
	info.lParam = 0;	//don't care
	m_TreeView.SortChildrenCB( &info);
	//m_TreeView.Expand( m_TreeView.GetRootItem(), TVE_EXPAND);

	::SetFocus( m_TreeView.m_hWnd);

	HTREEITEM hCur = GetTVItemIndex( m_pCur);
	m_TreeView.Select( hCur, TVGN_CARET);
}

void CUIPaneTemplateExplorer::UpdateView( void)
{
	if( m_bShowListView)
	{
		m_ListView.ShowWindow( SW_SHOW);
		m_TreeView.ShowWindow( SW_HIDE);
	}
	else
	{
		m_ListView.ShowWindow( SW_HIDE);
		m_TreeView.ShowWindow( SW_SHOW);
	}

	//내부에서는 다 지우고 다시 그린다.. ㅜ
	if( m_bShowListView)
	{
		_UpdateListView();
		m_TreeView.DeleteAllItems();
	}
	else
	{
		_UpdateTreeView();
		m_ListView.DeleteAllItems();
	}

	//Tool Bar Button
	UpdateButtonStates();

	//Status Bar
	UpdateStatusBarText();

}

i3UILTreeNode_Filter*  CUIPaneTemplateExplorer::ConvertToFilterNode( i3TreeNode* pNode)
{
	//Null check와 Type check를 동시에
	if( i3::same_of<i3UILTreeNode_Filter*>(pNode)) //->IsExactTypeOf( i3UILTreeNode_Filter::static_meta()))
	{
		return (i3UILTreeNode_Filter*)pNode;
	}

	return NULL;
}

i3UILTreeNode_Template* CUIPaneTemplateExplorer::ConvertToTemplateNode( i3TreeNode* pNode)
{
	//Null check와 Type check를 동시에
	if( i3::same_of<i3UILTreeNode_Template*>(pNode)) //->IsExactTypeOf( i3UILTreeNode_Template::static_meta()))
	{
		return (i3UILTreeNode_Template*)pNode;
	}

	return NULL;
}

void CUIPaneTemplateExplorer::OnBtnTreeView( void)
{
	m_bShowListView = false;

	UpdateView();
}

void CUIPaneTemplateExplorer::OnBtnIconView( void)
{
	m_bShowListView = true;
	m_ListView.SetView( LVS_ICON);	//LVS_ICON

	UpdateView();
}

void CUIPaneTemplateExplorer::OnBtnReportView( void)
{
	m_bShowListView = true;
	m_ListView.SetView( LVS_REPORT);	//LVS_REPORT

	UpdateView();
}

//상위 필터로 이동한다는 이야기는 m_pCur 와 m_pCur->getParent() 모두 NULL이 아니란 이야기
void CUIPaneTemplateExplorer::OnBtnUpperLevel( void)
{
	I3ASSERT( m_pCur != NULL);

	m_pCur = ConvertToFilterNode( m_pCur->getParent());

	I3ASSERT( m_pCur != NULL);

	UpdateView();
}

void CUIPaneTemplateExplorer::OnBtnNewFilter( void)
{
	if( m_pCur == NULL)
		return;

	i3UILTreeNode_Filter* pNode = i3UILTreeNode_Filter::new_object();
	
	// add 
	m_pCur->AddChild( pNode);	
	pNode->Release();	//refcount--

	// name
	char buf[MAX_PATH];
	_GetUniqueName( pNode, "Filter", buf, sizeof( buf));	// this function requires parent information)
	pNode->SetName( buf);
	
	UpdateView();

	// edit label
	EditLabel( pNode);
}

void CUIPaneTemplateExplorer::OnBtnNewTemplate( void)
{
	if( m_pCur == NULL)
		return;

	//Dialog
	CDlg_SelTemplateType dlg;

	if( dlg.DoModal() == IDOK)
	{
		i3ClassMeta* pMeta = i3ClassMetaSystem::i()->find_class_meta( dlg.m_szSelectedMeta);
			// i3ClassMeta::FindClassMetaByName( dlg.m_szSelectedMeta);
		if( pMeta != NULL)
		{
			i3UILTreeNode_Template* pNode = i3UILTreeNode_Template::new_object();

			// name
			char szName[MAX_PATH], buf[MAX_PATH];
			i3::string_ncopy_nullpad( szName, pMeta->class_name().c_str(), MAX_PATH);
			_GetUniqueName( pNode, &szName[13], buf, sizeof( buf));		//prefix 제거: "i3UITemplate_"
			pNode->SetName( buf);

			// template
			i3UITemplate * pNewTemplate = (i3UITemplate*)pMeta->create_instance(); // CREATEINSTANCE( pMeta);
			pNewTemplate->Create();
			pNewTemplate->SetResourcePath( buf);
			pNode->SetTemplate( pNewTemplate);
			//pNewTemplate->Release();	//refcount--
			
			//Add To Tree	
			m_pCur->AddChild( pNode);
			pNode->Release();	//refcount--

			m_pLibrary->AddElement( pNewTemplate);

			UpdateView();

			// edit label
			EditLabel( pNode);
		}
	}
}

void CUIPaneTemplateExplorer::OnBtnExportTemplate( i3UILTreeNode_Template* pNode)
{
	TCHAR szFilters[] = "i3Utx 파일(*.i3Utx)|*.i3Utx|";

	i3UILTreeNode_Template* pTNode = pNode;

	if( pTNode == NULL)
	{
		i3TreeNode* pNode2 = GetFocusedNode();	//get selected node
		pTNode = ConvertToTemplateNode( pNode2);
	}

	if( pTNode != NULL)
	{
		CFileDialog fileDlg( FALSE, _T("i3Utx"), _T(""), OFN_HIDEREADONLY, szFilters);

		if( fileDlg.DoModal() == IDOK)
		{
			//char conv[ MAX_PATH];
			//i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), (LPCTSTR)fileDlg.GetPathName(), conv, MAX_PATH );

			i3XMLFile file;

			i3UITemplate* pTemplate = pTNode->getTemplate();
			I3ASSERT( pTemplate != NULL);

			file.setKeyObject( pTemplate);

			if( file.Save( fileDlg.GetPathName()) == true)
			{
				MessageBox( I3STR("Template Export OK!") );
			}
			else
			{
				MessageBox( I3STR("Template Export Fail!") );
			}
		}
	}
}

void CUIPaneTemplateExplorer::OnLoadTemplate( i3UITemplate * pTemplate)
{
	i3UILTreeNode_Template* pNode = i3UILTreeNode_Template::new_object();

	// name
	char szName[MAX_PATH], buf[MAX_PATH];
	i3ClassMeta * pMeta = pTemplate->meta();
	if( pMeta != NULL)
	{
		i3::string_ncopy_nullpad( szName, pMeta->class_name().c_str(), MAX_PATH);
		if( _GetUniqueName( pNode, &szName[13], buf, sizeof( buf)) == false)		//prefix 제거: "i3UITemplate_"
		{
			char szMsg[ 512] = {0,};
			i3::snprintf( szMsg, sizeof( szMsg), "중복된 이름입니다. %s -> %s 로 변경합니다.", &szName[13], buf);
			::MessageBox( m_hWnd, szMsg, "Duplicate name", MB_OK );
		}
		pNode->SetName( buf);
	}

	pNode->SetTemplate( pTemplate);
	
	//Add To Tree	
	m_pCur->AddChild( pNode);
	pNode->Release();	//refcount--

	m_pLibrary->AddElement( pTemplate);

	UpdateView();
}

void CUIPaneTemplateExplorer::setCurrentTNode( i3UILTreeNode_Template* pTNode)
{
	if( pTNode == NULL)
	{
		i3TreeNode* pNode = GetFocusedNode();	//get selected node
		pTNode = ConvertToTemplateNode( pNode);
	}

	I3ASSERT( pTNode != NULL);

	m_pCurTNode = pTNode;
}

void CUIPaneTemplateExplorer::OnBtnEditScript( void)
{
	i3TreeNode* pNode = GetFocusedNode();

	if( pNode == NULL)
		return;

	if( pNode == m_pRoot)
	{
		CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();

		i3UIManager* pUIManager = i3UI::getManager();
		I3ASSERT( pUIManager != NULL);

		i3UILibrary* pLib = pUIManager->getUILibrary();
		I3ASSERT( pLib != NULL);

		const char* pszScript = pLib->getScriptFile();

		pMainFrm->OnEditScript( pszScript, true, false);
	}
	else if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
	{
		i3UITemplate* pTemplate = pTNode->getTemplate();
		I3ASSERT( pTemplate != NULL);

		if( i3::kind_of<i3UITemplate_User*>(pTemplate)) //->IsTypeOf( i3UITemplate_User::static_meta()))
		{
			i3UITemplate_User* pUser = (i3UITemplate_User*)pTemplate;
			const char* pszScript = pUser->GetName();

			CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
			pMainFrm->OnEditScript( pszScript, false, false);
		}
		else if( i3::same_of<i3UITemplate_MessageBox*>(pTemplate)) //->IsExactTypeOf( i3UITemplate_MessageBox::static_meta()))
		{
			i3UITemplate_MessageBox* pBox = (i3UITemplate_MessageBox*)pTemplate;
			const char* pszScript = pBox->GetName();

			CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
			pMainFrm->OnEditScript( pszScript, false, false);
		}

	}
}

void CUIPaneTemplateExplorer::OnBtnSelectAll( void)
{
	if( m_bShowListView)
	{
		for( INT32 i=0; i< m_ListView.GetItemCount(); i++)
		{
			m_ListView.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
	/*else
	{
		for( 
		TVIS_SELECTED*/
}

void CUIPaneTemplateExplorer::OnUnselectAll( void)
{
	if( m_bShowListView)
	{
		for( INT32 i=0; i< m_ListView.GetItemCount(); i++)
		{
			m_ListView.SetItemState( i, 0, LVIS_SELECTED);
		}
	}
}

void CUIPaneTemplateExplorer::OnBtnRemove( void)
{
	i3::vector<i3TreeNode*> list;
	if( GetSelectedNodes( list) > 0)
	{
		INT32 nAnswer =  MessageBox( "템플릿을 제거 하시겠습니까? \n"	\
									"이 템플릿을 사용하고 있는 컨트롤이 존재하는 경우 오동작을 일으킬 수 있습니다."	\
									"전혀 사용하고 있지 않은 것을 확신하는 경우 이거나,"	\
									"템플릿 라이브러리를 제작 하는 경우에만 사용하여 주십시오", \
									"확인", MB_YESNO|MB_ICONINFORMATION );

		if( nAnswer == IDYES)
		{
			for( size_t i=0; i< list.size(); i++)
			{
				i3TreeNode* pNode = list[i];

				//일단 library에서 제거
				Rec_RemoveFromLibrary( pNode);

				if( pNode->GetRefCount() == 1)
				{
					pNode->AddRef();
				}

				if( i3::same_of<i3UILTreeNode_Template*>(pNode)) //->IsExactTypeOf( i3UILTreeNode_Template::static_meta()))
				{
					
					pNode->RemoveFromParent();

					if( pNode == m_pCurTNode)
						m_pCurTNode = NULL;
				}
				else if( i3::same_of<i3UILTreeNode_Filter*>(pNode)) //->IsExactTypeOf( i3UILTreeNode_Filter::static_meta()))
				{
					i3UILTreeNode_Filter* pFilterNode = (i3UILTreeNode_Filter*)pNode;
					
					if( m_pCurTNode != NULL)
					{
						i3TreeNode* pParent = m_pCurTNode->getParent();
						while( pParent != NULL)
						{
							if( pParent == pNode)
							{
								m_pCurTNode = NULL;
								break;
							}
							pParent = pParent->getParent();
						}
					}

					//remove all children and grandchildren
					pFilterNode->RemoveDescendants( true);
				}
				else
				{
					I3ASSERT( 0);
				}

				I3_SAFE_RELEASE( pNode);

				I3TRACE("%p\n", pNode);
			}

			UpdateView();

			//UpdateOtherTemplateWindows( m_pCurTNode);
		}
	}
}

void CUIPaneTemplateExplorer::OnBtnCopy( void)
{
	m_CopiedNodeList.clear();
	
	GetSelectedNodes( m_CopiedNodeList);

	m_bCut = false;
}

void CUIPaneTemplateExplorer::OnBtnCut( void)
{
	//이전에 선택된 아이템들을 원래 상태로 복구
	//if( m_bShowListView)
	//{	
	//	//i3TreeNode* 를 가지고 역으로 nItem, hItem을 알아내야 한다.. 

	//	m_ListView.SetItemState( nItem, 0, LVIS_CUT);
	//}
	//else
	//{
	//	m_TreeView.SetItemState( hItem, 0, TVIS_CUT);
	//}

	m_CopiedNodeList.clear();
	
	GetSelectedNodes( m_CopiedNodeList);

	if( m_bShowListView)
	{	
		//listview item operation
		POSITION pos = m_ListView.GetFirstSelectedItemPosition();

		while( pos != NULL)
		{
			INT32 nItem = m_ListView.GetNextSelectedItem( pos);

			m_ListView.SetItemState( nItem, LVIS_CUT, LVIS_CUT);
		}
	}
	else
	{
		HTREEITEM hItem = m_TreeView.GetSelectedItem();
		m_TreeView.SetItemState( hItem, TVIS_CUT, TVIS_CUT);
	}

	m_bCut = true;
}

void CUIPaneTemplateExplorer::OnBtnPaste( i3TreeNode* pTarget)
{
	if( m_pCur == NULL)
		return;

	if (m_ListView.GetEditControl() || m_TreeView.GetEditControl())
		return;

	if( pTarget == NULL)
		pTarget = m_pCur;

	for( size_t i=0; i< m_CopiedNodeList.size(); i++)
	{
		i3TreeNode* pNode = m_CopiedNodeList[i];
		
		if( m_bCut)
		{
			if( pNode == pTarget )
			{
				continue;
			}
	
 			Rec_RemoveFromLibrary( pNode);
 
 			pNode->AddRef();
 			pNode->RemoveFromParent();
 			pTarget->AddChild( pNode);
 
 			AdjustNames( pNode);
 
 			Rec_AddToLibrary( pNode);

			pNode->Release();
		}
		else
		{
			i3TreeNode* pCopiedNode = CreateClone( pNode);
			
			pCopiedNode->AddRef();
			pCopiedNode->RemoveFromParent();
			pTarget->AddChild( pCopiedNode);

			AdjustNames( pCopiedNode);

			Rec_AddToLibrary( pCopiedNode);

			pCopiedNode->Release();
		}
	}

	m_bCut = false;

	UpdateView();
}

void CUIPaneTemplateExplorer::OnKeyDownReturn( void)
{
	i3TreeNode* pNode = GetFocusedNode();
	i3UILTreeNode_Template * pTNode;

	pTNode = ConvertToTemplateNode( pNode);

	if( pTNode != NULL)
	{
		_EditTemplate( pTNode);
		return;
	}

	if( m_bShowListView)
	{
		if( i3UILTreeNode_Filter* pFilterNode = ConvertToFilterNode( pNode))
		{
			m_pCur = pFilterNode;
			UpdateView();
		}	
	}
}

void CUIPaneTemplateExplorer::OnKeyDownEscape( void)
{
	OnUnselectAll();
}

//i3TreeNode*
INT32 CUIPaneTemplateExplorer::GetSelectedNodes( i3::vector<i3TreeNode*>& Out)
{
	//LVIS_FOCUS: 오직 1개
	//LVIS_SELECTED: 여러 개
	//INT32 GetNextItem( LVNI_FOCUSED)
	//SetItemState

	if( m_bShowListView)
	{
		POSITION pos = m_ListView.GetFirstSelectedItemPosition();
		INT32 nCount = m_ListView.GetSelectedCount();
		while( pos != NULL)
		{
			INT32 nItem = m_ListView.GetNextSelectedItem( pos);
			
			i3TreeNode* pNode = GetTreeNode( nItem);		
			Out.push_back( pNode);
		}
		return nCount;
	}
	else
	{
		HTREEITEM hItem = m_TreeView.GetSelectedItem();

		if( hItem != NULL)
		{
			i3TreeNode* pNode = GetTreeNode( hItem);
			Out.push_back( pNode);

			return 1;
		}
	}

	return 0;
}

i3TreeNode* CUIPaneTemplateExplorer::GetFocusedNode( void)
{
	if( m_bShowListView)
	{
		INT32 nIdx = m_ListView.GetNextItem( -1, LVNI_FOCUSED|LVNI_SELECTED);
		return GetTreeNode( nIdx);
	}
	else
	{
		HTREEITEM hSel = m_TreeView.GetSelectedItem();
		return GetTreeNode( hSel);
	}
}

i3TreeNode* CUIPaneTemplateExplorer::GetTreeNode( INT32 iItem)
{
	if( iItem < 0)		return NULL;

	return (i3TreeNode*)m_ListView.GetItemData( iItem);
}

i3TreeNode* CUIPaneTemplateExplorer::GetTreeNode( HTREEITEM hItem)
{
	if( hItem == NULL)	return NULL;
	
	return (i3TreeNode*)m_TreeView.GetItemData( hItem);
}

//ListView;
INT32 CUIPaneTemplateExplorer::GetLVItemIndex( i3TreeNode* pNode)
{
	LVFINDINFO info;
	info.flags = LVFI_PARAM;
	info.lParam = (LPARAM)pNode;
	
	return m_ListView.FindItem( &info);
}

//TreeView;
HTREEITEM CUIPaneTemplateExplorer::_RecFindTreeItem( HTREEITEM hRoot, i3TreeNode* pNode)
{
	i3TreeNode* pTreeNode = (i3TreeNode*)m_TreeView.GetItemData( hRoot);
	if( pTreeNode == pNode)
	{
		return hRoot;
	}

	HTREEITEM hChild = m_TreeView.GetChildItem( hRoot);
	while( hChild != NULL)
	{
		HTREEITEM hNext = m_TreeView.GetNextItem( hChild, TVGN_NEXT);

		HTREEITEM hFound = _RecFindTreeItem( hChild, pNode);
		if( hFound != NULL)
			return hFound;

		hChild = hNext;
	}

	return NULL;
}

HTREEITEM CUIPaneTemplateExplorer::GetTVItemIndex( i3TreeNode* pNode)
{
	//tree-traversal
	//GetItem 이 lparam으로 HTREEITEM을 찾는걸 지원하지 않는듯.. ㅜ
	/*TVITEM item;
	item.mask = TVIF_HANDLE;
	info.mask = TVIF_PARAM;
	item.lParam = (LPARAM)pNode;
	BOOL bRes = m_TreeView.GetItem( &item);
	I3ASSERT( bRes == TRUE);
	HTREEITEM hItem = item.hItem;*/
	
	HTREEITEM hRoot = m_TreeView.GetRootItem();
	return _RecFindTreeItem( hRoot, pNode);
}

HTREEITEM CUIPaneTemplateExplorer::_RecFindTreeItemByName( HTREEITEM hRoot, const char * pszName)
{
	i3TreeNode* pTreeNode = (i3TreeNode*)m_TreeView.GetItemData( hRoot);
	if( pTreeNode != NULL)
	{
		if( i3::generic_is_iequal( pTreeNode->GetName(), pszName) )
			return hRoot;
	}

	HTREEITEM hChild = m_TreeView.GetChildItem( hRoot);
	while( hChild != NULL)
	{
		HTREEITEM hNext = m_TreeView.GetNextItem( hChild, TVGN_NEXT);

		HTREEITEM hFound = _RecFindTreeItemByName( hChild, pszName);
		if( hFound != NULL)
			return hFound;

		hChild = hNext;
	}

	return NULL;
}

HTREEITEM CUIPaneTemplateExplorer::FindTreeNodeByName( const char * pszName)
{
	HTREEITEM hRoot = m_TreeView.GetRootItem();

	if( hRoot != NULL)
		return _RecFindTreeItemByName( hRoot, pszName);
	
	return NULL;
}

void CUIPaneTemplateExplorer::_EditTemplate( i3UILTreeNode_Template * pTNode)
{
	if( pTNode == NULL)
	{
		i3TreeNode* pNode = GetFocusedNode();	//get selected node
		pTNode = ConvertToTemplateNode( pNode);
	}

	if( pTNode != NULL)
	{
		UpdateOtherTemplateWindows( pTNode);

		setCurrentTNode( pTNode);
	}
}

void CUIPaneTemplateExplorer::EditTemplate( i3UITemplate * pTemplate)
{
	HTREEITEM hFind = FindTreeNodeByName( pTemplate->GetName());

	i3TreeNode * pNode = NULL;

	if( hFind != NULL)
		pNode = GetTreeNode( hFind);

	if( pNode != NULL)
	{
		if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
		{
			setCurrentTNode( pTNode);
		}
	}
}


void CUIPaneTemplateExplorer::UpdateButtonStates( void)
{
	if( m_pCur == NULL || ::IsWindow( m_ToolBar.m_hWnd) == FALSE)
	{
		m_ToolBar.SetState( ID_TBBTN_TREEVIEW, 0);
		m_ToolBar.SetState( ID_TBBTN_ICONVIEW, 0);
		m_ToolBar.SetState( ID_TBBTN_REPORTVIEW, 0);
		m_ToolBar.SetState( ID_TBBTN_UPPERLEVEL, 0);
		m_ToolBar.SetState( ID_TBBTN_NEWFILTER, 0);
		m_ToolBar.SetState( ID_TBBTN_NEWTEMPLATE, 0);
		m_ToolBar.SetState( ID_TBBTN_REMOVE, 0);
		m_ToolBar.SetState( ID_TBBTN_EDIT, 0);
		m_ToolBar.SetState( ID_TBBTN_EDITSCRIPT, 0);

		return;
	}
	
	m_ToolBar.SetState( ID_TBBTN_TREEVIEW, TBSTATE_ENABLED);
	m_ToolBar.SetState( ID_TBBTN_ICONVIEW, TBSTATE_ENABLED);
	m_ToolBar.SetState( ID_TBBTN_REPORTVIEW, TBSTATE_ENABLED);
	m_ToolBar.SetState( ID_TBBTN_NEWFILTER, TBSTATE_ENABLED);
	m_ToolBar.SetState( ID_TBBTN_NEWTEMPLATE, TBSTATE_ENABLED);

	{
		//View 관련 버튼 3개 중 1개는 checked 상태로 만들어 준다. 
		INT32 nID = -1;
		if( m_bShowListView)
		{
			if( m_ListView.GetView() == LVS_ICON)
				nID = ID_TBBTN_ICONVIEW;
			else
				nID = ID_TBBTN_REPORTVIEW;
		}
		else
		{
			nID = ID_TBBTN_TREEVIEW;
		}

		m_ToolBar.SetState( nID, TBSTATE_ENABLED | TBSTATE_CHECKED);
	}
	
	//포커스가 어디에 있느냐에 따라 버튼의 enable/disable 상태가 결정된다. 
	i3TreeNode* pFocused = GetFocusedNode();
	
	{
		UINT32 nState = ( m_bShowListView == FALSE || m_pCur->getParent() == NULL) ? 0 : TBSTATE_ENABLED;
		m_ToolBar.SetState( ID_TBBTN_UPPERLEVEL, nState);
	}

	{
		UINT32 nState = _isEditable( pFocused) ? TBSTATE_ENABLED : 0;
		m_ToolBar.SetState( ID_TBBTN_EDIT, nState);
	}

	{
		UINT32 nState = _isScriptEditable( pFocused) ? TBSTATE_ENABLED : 0;
		m_ToolBar.SetState( ID_TBBTN_EDITSCRIPT, nState);
	}

	{
		UINT32 nState = (pFocused == m_pRoot) ? 0 : TBSTATE_ENABLED;
		m_ToolBar.SetState( ID_TBBTN_REMOVE, nState);
	}

	//if( i3UI::getUILibrary() != NULL)
	{
		//i3UI::getUILibrary()->setEdited( true);
	}
}

void CUIPaneTemplateExplorer::UpdateStatusBarText( void)
{
	char buf[MAX_PATH];
	_GetCurrentFilterPath( buf, MAX_PATH);
	m_StatusBar.SetText( buf, 0, SBT_NOBORDERS);	
}

void CUIPaneTemplateExplorer::EditLabel( i3TreeNode* pNode)
{
	if( pNode == NULL)
		return;

	if( m_bShowListView)
	{
		INT32 nItem = GetLVItemIndex( pNode);
		
		::SetFocus( m_ListView.m_hWnd);
		m_ListView.EditLabel( nItem);
	}
	else
	{
		HTREEITEM hItem = GetTVItemIndex( pNode);

		::SetFocus( m_TreeView.m_hWnd);
		m_TreeView.EditLabel( hItem);
	}
}

void CUIPaneTemplateExplorer::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if( m_pCur == NULL)
		return;

	// TODO: Add your message handler code here
	if( !m_bShowListView)
		I3TRACE("CUIPaneTemplateExplorer::OnContextMenu\n"); 

	CMenu Menu;
	CMenu* pPopup = NULL;
	UINT32 cmd = 0;

	i3TreeNode* pFocused = GetFocusedNode();

	if( pFocused != NULL)
	{
		Menu.LoadMenu( IDR_MENU_TEMPLATE_EXPLORER);
		pPopup = Menu.GetSubMenu( 1);

		if( pPopup == NULL)
		{
			I3PRINTLOG(I3LOG_WARN, "CUIPaneTemplateExplorer::OnContextMenu: pPopup == NULL\n");
			return;
		}

		if( pFocused != m_pRoot)
		{
			pPopup->DeleteMenu( ID_LISTVIEW_SETSCRIPT, MF_BYCOMMAND);
			pPopup->DeleteMenu( ID_LISTVIEW_REMOVESCRIPT, MF_BYCOMMAND);
		}

		if( !_isEditable( pFocused))
			pPopup->DeleteMenu( ID_LISTVIEW_EDIT, MF_BYCOMMAND);

		if( !_isScriptEditable( pFocused))
			pPopup->DeleteMenu( ID_LISTVIEW_EDITSCRIPT, MF_BYCOMMAND);
	}
	else
	{
		Menu.LoadMenu( IDR_MENU_TEMPLATE_EXPLORER);

		/*pPopup = Menu.GetSubMenu( 1);
		if( pPopup != NULL)
			pPopup->DeleteMenu( ID_LISTVIEW_EXPORTTEMPLATE, MF_BYCOMMAND);*/

		pPopup = Menu.GetSubMenu( 0);

		if( pPopup == NULL)
		{
			I3PRINTLOG(I3LOG_WARN, "CUIPaneTemplateExplorer::OnContextMenu: pPopup == NULL\n");
			return;
		}
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
									point.x, point.y, (CWnd *) this, NULL);

	switch( cmd)
	{
	case ID_VIEW_TREEVIEW:			OnBtnTreeView();		break;
	case ID_VIEW_ICONVIEW:			OnBtnIconView();		break;
	case ID_VIEW_REPORTVIEW:		OnBtnReportView();		break;
	case ID_LISTVIEW_NEWFILTER:		OnBtnNewFilter();		break;
	case ID_LISTVIEW_NEWTEMPLATE:	OnBtnNewTemplate();		break;
	case ID_LISTVIEW_EXPORTTEMPLATE : OnBtnExportTemplate( (i3UILTreeNode_Template*)pFocused);	break;

	case ID_LISTVIEW_EDIT:			_EditTemplate();		break;
	case ID_LISTVIEW_EDITSCRIPT:	OnBtnEditScript();	break;
	case ID_LISTVIEW_DELETE:		OnBtnRemove();		break;
	case ID_LISTVIEW_COPY:			OnBtnCopy();		break;
	case ID_LISTVIEW_CUT:			OnBtnCut();			break;
	case ID_LISTVIEW_PASTE:			OnBtnPaste();		break;

	case ID_LISTVIEW_SETSCRIPT:		SetScript();		break;
	case ID_LISTVIEW_REMOVESCRIPT:	RemoveScript();		break;
	
	default: break;
	}
}

void CUIPaneTemplateExplorer::SetScript( void)
{
	i3TreeNode* pNode = GetFocusedNode();
	I3ASSERT( m_pRoot == pNode);

	i3UIManager* pUIManager = i3UI::getManager();
	I3ASSERT( pUIManager != NULL);

	i3UILibrary* pLib = pUIManager->getUILibrary();
	I3ASSERT( pLib != NULL);

	TCHAR szFilters[] = _T( "Lua 파일(*.lua)|*.lua");
	
	CFileDialog fileDlg(FALSE, _T("lua"), (LPCTSTR)pNode->GetName(), OFN_HIDEREADONLY, szFilters);
	if( fileDlg.DoModal() == IDOK)
	{
		// If the file doesn't exist, create.
		{
			CFile cf;
			
			//Returns nonzero if the open was successful; otherwise 0. The pError parameter is meaningful only if 0 is returned.
			BOOL bResult = cf.Open( fileDlg.GetPathName(), CFile::modeCreate|CFile::modeNoTruncate );
			I3ASSERT( bResult);
			
			//If you have not closed the file before destroying the object, the destructor closes it for you
		}
		
		CString filename = fileDlg.GetPathName();
		pLib->setScriptFile( (const char*)LPCTSTR( filename));
		ScriptManager::SetLibScript( (const char*)LPCTSTR( filename));

		//CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
		//pFrame->UpdatePropertyPane();

		//m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);
	}
	else
	{
		CFile cf;

		char conv[MAX_PATH];
		i3::snprintf( conv, _countof(conv), "%s/%s", i3ResourceFile::GetWorkingFolder(), pLib->getScriptFile());
		BOOL bResult = cf.Open( conv, CFile::modeRead);
		
		if( bResult == FALSE)
		{
			RemoveScript();
		}

		//Close?
	}
}

void CUIPaneTemplateExplorer::RemoveScript( void)
{
	i3TreeNode* pNode = GetFocusedNode();
	I3ASSERT( pNode == m_pRoot)

	i3UIManager* pUIManager = i3UI::getManager();
	I3ASSERT( pUIManager != NULL);

	i3UILibrary* pLibrary = pUIManager->getUILibrary();
	I3ASSERT( pLibrary != NULL);

	pLibrary->setScriptFile(NULL);
	ScriptManager::SetLibScript( (const char*)NULL);

	//CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	//pFrame->UpdatePropertyPane();

	//m_TreeCtrl.SetItemState( hItem, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);
}

void CUIPaneTemplateExplorer::OnLVNKeydown( NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* lvKeyDown = (LV_KEYDOWN*)pNMHDR;
	I3TRACE("CUIPaneTemplateExplorer::OnLVNKeydown: %d\n", lvKeyDown->wVKey);

	if( GetKeyState( VK_CONTROL) & 0x8000)
	{
		switch( lvKeyDown->wVKey)
		{
		case 'A':	OnBtnSelectAll();	break;
		
		default:	break;
		}
	}
	else
	{
		switch( lvKeyDown->wVKey)
		{
		case VK_DELETE: OnBtnRemove();	break;
		}
	}

	*pResult = 0;
}

void CUIPaneTemplateExplorer::OnLVNColumnClick( NMHDR* pNMHDR, LRESULT* pResult)
{
	I3TRACE("CUIPaneTemplateExplorer::OnColumnClickLV: \n");

	NMLISTVIEW* pNMLV = (NMLISTVIEW*)pNMHDR;
	
	m_ListView.SortItems( _LVCompareProc, pNMLV->iSubItem);	// (compare proc, column idx)

	//시작할때 Sorting?
	//폴더 변경될때마다 Sorting?

	s_bAsc = !s_bAsc;
	
	*pResult = 0;
}

void CUIPaneTemplateExplorer::OnLVNEndLabelEdit( NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;	//pNMLVDI->item.pszText == NULL  이면 return value 무시

	NMLVDISPINFO* pNMLVDI = (NMLVDISPINFO*)pNMHDR;
	
	LVITEM item = pNMLVDI->item;

	if( item.pszText != NULL)
	{
		i3TreeNode* pNode = GetTreeNode( item.iItem);

		if( i3::generic_is_iequal( item.pszText, pNode->GetName()) )
			return;

		if( _IsUniqueName( pNode, item.pszText))
		{
			if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
			{
				if( m_pLibrary != NULL)
				{
					// Library 내부에서 Template들은 문자열 이름으로 직접 검색되지 않고, MD5 기반의 Checksum Code로 
					// 검색이 이루어진다. RenameElement() 함수 내부에서는 변경되는 이름의 새로운 Checksum Code를 기반으로
					// 내부 DB에 Update를 하는 함수.
					// 
					m_pLibrary->RenameElement( pTNode->getTemplate(), item.pszText);
				}
			}

			pNode->SetName( item.pszText);


			*pResult = 1;	//return false 하면 원래대로 복구
		}
		else
		{
			::MessageBox( m_hWnd, "Duplicate name", "Rename", MB_OK );
			::SetFocus( m_ListView.m_hWnd);
			m_ListView.EditLabel( item.iItem);
		}
	}
}

void CUIPaneTemplateExplorer::OnLVNBeginDrag( NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLISTVIEW* pNMLV = (NMLISTVIEW*)pNMHDR;

	//ref. Windows API 정복 p.901
	//드래그 되는 항목 저장
	m_nDragItem = pNMLV->iItem;

	//드래그 이미지 리스트 만듦
	POINT pt;
	GetCursorPos( &pt);

	m_pDragImageList = m_ListView.CreateDragImage( m_nDragItem, &pt);

	//핫 스팟. 커서 위치에서 이미지 좌상단 좌표를 뺀 좌표로 계산된다. 
	INT32 hotx = pNMLV->ptAction.x - pt.x;
	INT32 hoty = pNMLV->ptAction.y - pt.y;
	m_pDragImageList->BeginDrag( 0, CPoint( hotx, hoty));

	//현재 마우스 커서 위치에 드래그 이미지를 그림
	pt.x = pNMLV->ptAction.x;
	pt.y = pNMLV->ptAction.y;
	::ClientToScreen( m_hWnd, &pt);
	m_pDragImageList->DragEnter( NULL, pt);

	//계속적인 드래그 동작을 위해 커서를 캡처함
	::SetCapture( m_hWnd);
	m_bDragItem = true;
	
	*pResult = 0;
}

void CUIPaneTemplateExplorer::OnLVNItemChanged( NMHDR* pNMHDR, LRESULT* pResult)
{
	//3번씩 들어옴
	UpdateButtonStates();

	*pResult = 0;
}

void CUIPaneTemplateExplorer::OnLVNNMDblClk( NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	NMITEMACTIVATE* pNMIA = (NMITEMACTIVATE*)pNMHDR;

	i3TreeNode* pNode = GetTreeNode( pNMIA->iItem);

	if( pNode == NULL)
		return;

	i3UILTreeNode_Template * pTNode = ConvertToTemplateNode( pNode);
	
	if( pTNode != NULL)	//Double-clicked Template
	{
		_EditTemplate( pTNode);
	}
	else if( i3UILTreeNode_Filter* pFilterNode = ConvertToFilterNode( pNode))	/*( Filter)*/
	{
		m_pCur = pFilterNode;
		UpdateView();
	}
}

void CUIPaneTemplateExplorer::OnTVNBeginDrag( NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	m_hDragItem = pNMTreeView->itemNew.hItem;

	//Root는 드래그 할 수 없음
	if( GetTreeNode( m_hDragItem) == m_pRoot)
		return;		

	CPoint pt = pNMTreeView->ptDrag;
	int cxCursor, cyCursor;

	cxCursor = 10;
	cyCursor = 10;

	GetCursorPos( &pt);

	SetCapture();

	m_bDragItem = true;

	m_pDragImageList = m_TreeView.CreateDragImage( m_hDragItem);
	if( m_pDragImageList != NULL)
	{
		m_pDragImageList->DragShowNolock( FALSE);
		m_pDragImageList->BeginDrag( 0, CPoint( -cxCursor, -cyCursor));
		m_pDragImageList->DragEnter( NULL, pt);
	}
}

void CUIPaneTemplateExplorer::OnTVNNMDblClk( NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if( pNMTreeView->itemNew.hItem == NULL)
		return;

	CPoint point;

	GetCursorPos( &point);
	m_TreeView.ScreenToClient( &point);

	HTREEITEM hSel = m_TreeView.HitTest( point, NULL);

	i3TreeNode* pSelNode = GetTreeNode( hSel);

	if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pSelNode))
	{
		_EditTemplate( pTNode);
	}

	*pResult = 0;
}

void CUIPaneTemplateExplorer::OnTVNEndLabelEdit( NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	NMTVDISPINFO *pDispInfo = reinterpret_cast<NMTVDISPINFO*>(pNMHDR);
    
	if( pDispInfo->item.pszText != NULL)
	{
		i3TreeNode* pNode = GetTreeNode( pDispInfo->item.hItem);
		I3ASSERT( pNode != NULL);

		if( i3::generic_is_iequal( pDispInfo->item.pszText, pNode->GetName()) )
			return;

		if( _IsUniqueName( pNode, pDispInfo->item.pszText))
		{
			pNode->SetName( pDispInfo->item.pszText);

			if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
			{
				if( m_pLibrary != NULL)
				{
					m_pLibrary->RenameElement( pTNode->getTemplate(), pTNode->GetName());

					//UpdateOtherTemplateWindows( m_pCurTNode);
				}
			}

			*pResult = 1;	//return false 하면 원래대로 복구
		}
		else
		{
			::MessageBox( m_hWnd, "Duplicate name", "Rename", MB_OK );
			::SetFocus( m_TreeView.m_hWnd);
			m_TreeView.EditLabel(pDispInfo->item.hItem);
		}
	}	
}

void CUIPaneTemplateExplorer::OnTVNSelChanged( NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTREEVIEW * pNMTV = (NMTREEVIEW*)pNMHDR;

	//i3TreeNode* pOldNode = GetTreeNode( pNMTV->itemOld.hItem);
	i3TreeNode* pNewNode = GetTreeNode( pNMTV->itemNew.hItem);

	if( i3UILTreeNode_Filter* pFNode = ConvertToFilterNode( pNewNode))
	{
		m_pCur = pFNode;
	}
	else if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNewNode))
	{
		m_pCur = ConvertToFilterNode( pTNode->getParent());
	}

	I3ASSERT( m_pCur != NULL);

	UpdateButtonStates();

	UpdateStatusBarText();

	*pResult = 0;
}

void CUIPaneTemplateExplorer::OnTVNNMRClick( NMHDR* pNMHDR, LRESULT* pResult)
{
	UINT uFlags;
	CPoint pt;
	GetCursorPos( &pt);
	m_TreeView.ScreenToClient( &pt);
	HTREEITEM hItem = m_TreeView.HitTest( pt, &uFlags);

	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		m_TreeView.Select(hItem, TVGN_CARET);
	} 
}

void CUIPaneTemplateExplorer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//I3TRACE("OnLButtonUp\n");

	if( m_bDragItem)
	{
		//ImageList 제거
		m_pDragImageList->DragLeave( NULL);
		m_pDragImageList->EndDrag();
		::ReleaseCapture();
		m_bDragItem = false;
		
		delete m_pDragImageList;	//delete? 
		m_pDragImageList = NULL;

		//SrcNode 구하기
		i3::vector<i3TreeNode*> list;
		INT32 nCount = 0;

		if( m_bShowListView)
		{
			nCount = GetSelectedNodes( list);
		}
		else
		{
			i3TreeNode* pNode = GetTreeNode( m_hDragItem);
			list.push_back( pNode);
			nCount = 1;
		}

		//TargetNode 구하기
		i3TreeNode* pTargetNode = NULL;
		if( m_bShowListView)
		{
			m_ListView.SetItemState( m_nOldTarget, 0, LVIS_DROPHILITED);
			pTargetNode = GetTreeNode( m_nOldTarget);
		}
		else
		{
			m_TreeView.SetItemState( m_hOldTarget, 0, TVIS_DROPHILITED);
			pTargetNode = GetTreeNode( m_hOldTarget);
		}
		

		// Do Something Here!
		i3UILTreeNode_Filter* pNodeFilter		= ConvertToFilterNode( pTargetNode);
		i3UILTreeNode_Template* pNodeTemplate	= ConvertToTemplateNode( pTargetNode);
		if( pNodeFilter )	
		{
			//Drop To Filter
			//Cut & Paste
			m_CopiedNodeList.clear();
			m_bCut = true;

			for( INT32 i=0; i< nCount; i++)
			{
				i3TreeNode* pSrc = list[i];
				
				if( pTargetNode != pSrc )
					m_CopiedNodeList.push_back( pSrc);
			}

			OnBtnPaste( pNodeFilter);
		}
		else if(pNodeTemplate )
		{
			//Drop To Template
			//Overwrite
			if( nCount == 1)
			{
				i3TreeNode* pSrcNode = list[0];

				if( i3UILTreeNode_Template* pSrc = ConvertToTemplateNode( pSrcNode))
				{
					if( pSrc != pNodeTemplate)
					{
						i3UITemplate* pTargetTemplate = pNodeTemplate->getTemplate();
						i3UITemplate* pSrcTemplate = pSrc->getTemplate();

						if( pSrcTemplate->same_of( pTargetTemplate->meta()))
						{
							INT32 nAnswer = ::MessageBox( m_hWnd, "Overwrite?", "Warning", MB_YESNO);
							if( nAnswer == IDYES)
							{
								char szTargetName[MAX_PATH];
								i3::string_ncopy_nullpad( szTargetName, pTargetTemplate->GetName(), MAX_PATH);	//바뀌기 전에 이름 미리 보존

								pSrcTemplate->CopyTo( pTargetTemplate, I3_COPY_INSTANCE);
								
								pTargetTemplate->SetName( szTargetName);	//이름까지 바꿔버리면 안된다..

								//UpdateOtherTemplateWindows( m_pCurTNode);
							}
						}
						else
						{
							::MessageBox( m_hWnd, "Cannot overwrite to a different type of a template", "Warning", MB_OK);
						}
					}
				}
			}
		}
		else
		{
			//Drop To View
			i3TreeNode* pSrcNode = list[0];

			if( i3UILTreeNode_Template* pSrc = ConvertToTemplateNode( pSrcNode))
			{
				i3UITemplate* pTemplate = pSrc->getTemplate();

				CPoint pt;
				pt = point;

				ClientToScreen( &pt);

				CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
				pMainWnd->DropObject( pTemplate, pt);

				pMainWnd->getPropertiesPane()->DragTemplate(pTemplate);
			}

			//Drop To TemplatePane
			// Find Target Window
			CPoint pt( point);		//Get current mouse coordinates
			ClientToScreen( &pt);	//Convert to screen coordinates
			
			CWnd* pDropWnd = WindowFromPoint( pt);	// Get the CWnd pointer of the window that is under the mouse cursor
			I3ASSERT( pDropWnd);		//make sure we have a window pointer
			
			//if( pDropWnd->IsKindOf( RUNTIME_CLASS( CUIPaneTemplate)))
			if( pDropWnd->IsKindOf( RUNTIME_CLASS( CTemplateTree)))
			{
				//m_pDropList = (CListCtrl*)pDropWnd;			//Set pointer to the list we are dropping on
				//I3TRACE("Drop To: CUIPaneTemplate\n");
				I3TRACE("Drop To: CTemplateTree\n");

				CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
				CUIPaneTemplate* pTemplatePane = pMainWnd->getTemplatePane();

				if( nCount == 1)
				{
					i3TreeNode* pSrcNode2 = list[0];

					if( i3UILTreeNode_Template* pSrc = ConvertToTemplateNode( pSrcNode2))
					{
						i3UITemplate* pSrcTemplate = pSrc->getTemplate();

						pTemplatePane->OverWriteTemplate( pSrcTemplate, pt);
						I3TRACE("pt: %d, %d\n", pt.x, pt.y);
					}
				}
			}

			
		}

		m_bHovering = false;
		::KillTimer( m_hWnd, TIMER_HOVERING);
		::KillTimer( m_hWnd, TIMER_SCROLLUP);
		::KillTimer( m_hWnd, TIMER_SCROLLDOWN);
	}

	CDockablePane::OnLButtonUp(nFlags, point);
}

void CUIPaneTemplateExplorer::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//I3TRACE("OnMouseMove: %d, %d\n", point.x, point.y);
	
	if( m_bDragItem)
	{
		//마우스 위치를 화면 좌표로 바꿔 드래그 이미지를 그린다.
		CPoint pt = point;
		::ClientToScreen( m_hWnd, &pt);
		m_pDragImageList->DragMove( pt);

		if( m_bShowListView)	//ListView
		{
			//커서 위치의 항목을 조사한다. 
			RECT rc;
			::GetWindowRect( m_ListView.m_hWnd, &rc);
			::ScreenToClient( m_hWnd, (POINT*)&rc);
			
			LVHITTESTINFO info;
			info.pt.x = point.x - rc.left;
			info.pt.y = point.y - rc.top;
			m_nDropTarget = m_ListView.HitTest( &info);
			
			//드롭 타겟 표시
			if( m_nDropTarget != -1)
			{
				//m_pDragImageList->DragLeave( NULL);
				m_ListView.SetItemState( m_nDropTarget, LVIS_DROPHILITED, LVIS_DROPHILITED);

				if( m_nDropTarget != m_nOldTarget)
				{
					m_ListView.SetItemState( m_nOldTarget, 0, LVIS_DROPHILITED);
					m_nOldTarget = m_nDropTarget;
				}

				//::UpdateWindow( m_ListView.m_hWnd);
				//m_pDragImageList->DragEnter( NULL, pt);
			}
			else
			{
				m_ListView.SetItemState( m_nOldTarget, 0, LVIS_DROPHILITED);
				m_nOldTarget = m_nDropTarget;
			}
		}
		else	// TreeView
		{
			//커서 위치의 항목을 조사한다. 
			RECT rc;
			::GetWindowRect( m_TreeView.m_hWnd, &rc);
			::ScreenToClient( m_hWnd, (POINT*)&rc);
			
			TVHITTESTINFO info;
			info.pt.x = point.x - rc.left;
			info.pt.y = point.y - rc.top;
			m_hDropTarget = m_TreeView.HitTest( &info);
			
			//드롭 타겟 표시
			if( m_hDropTarget != NULL)
			{
				m_TreeView.SetItemState( m_hDropTarget, TVIS_DROPHILITED, TVIS_DROPHILITED);

				if( m_hDropTarget != m_hOldTarget)
				{
					m_TreeView.SetItemState( m_hOldTarget, 0, TVIS_DROPHILITED);
					m_hOldTarget = m_hDropTarget;
				}
			}
			else
			{
				m_TreeView.SetItemState( m_hOldTarget, 0, TVIS_DROPHILITED);
				m_hOldTarget = m_hDropTarget;
			}
		}

		// Scroll when hovering
		HWND hCurWnd = m_bShowListView ? m_ListView.m_hWnd : m_TreeView.m_hWnd;
		
		RECT rc;
		::GetWindowRect( hCurWnd, &rc);
		
		if( (short)pt.y > rc.top - HOVERING_VALID_RANGE && (short)pt.y < rc.top + HOVERING_VALID_RANGE)
		{
			::SetTimer( m_hWnd, TIMER_HOVERING, HOVERING_TIMEOUT, NULL);
			::SetTimer( m_hWnd, TIMER_SCROLLUP, SCROLLING_FREQUENCY, NULL);
			::KillTimer( m_hWnd, TIMER_SCROLLDOWN);

			if( m_bHovering)
				::SendMessage( hCurWnd, WM_VSCROLL, MAKEWPARAM( SB_LINEUP, 0), NULL);
		}
		else if( (short)pt.y > rc.bottom - HOVERING_VALID_RANGE && (short)pt.y < rc.bottom + HOVERING_VALID_RANGE)
		{
			::SetTimer( m_hWnd, TIMER_HOVERING, HOVERING_TIMEOUT, NULL);
			::SetTimer( m_hWnd, TIMER_SCROLLDOWN, SCROLLING_FREQUENCY, NULL);
			::KillTimer( m_hWnd, TIMER_SCROLLUP);

			if( m_bHovering)
				::SendMessage( hCurWnd, WM_VSCROLL, MAKEWPARAM( SB_LINEDOWN, 0), NULL);
		}
		else
		{
			m_bHovering = false;
			::KillTimer( m_hWnd, TIMER_HOVERING);
			::KillTimer( m_hWnd, TIMER_SCROLLUP);
			::KillTimer( m_hWnd, TIMER_SCROLLDOWN);
		}

		//cursor on template pane
		{
			CPoint pt2( point);		//Get current mouse coordinates
			ClientToScreen( &pt2);	//Convert to screen coordinates
				
			CWnd* pDropWnd = WindowFromPoint( pt2);	// Get the CWnd pointer of the window that is under the mouse cursor
			I3ASSERT( pDropWnd);		//make sure we have a window pointer
				
			if( pDropWnd->IsKindOf( RUNTIME_CLASS( CTemplateTree)))
			{
				CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
				CUIPaneTemplate* pPane = pMainFrm->getTemplatePane();
				pPane->HighLightItemAtPoint( pt2);
			}
			else
			{
				CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();

				i3::vector<i3TreeNode*> list;
				INT32 nCount = 0;
				bool bInsideFrame = false;
				i3ClassMeta * pInsideFrameMeta = NULL;

				if( m_bShowListView)
				{
					nCount = GetSelectedNodes( list);
				}
				else
				{
					i3TreeNode* pNode = GetTreeNode( m_hDragItem);
					list.push_back( pNode);
					nCount = 1;
				}

				// 특정 Template은 하위 Ctrl에 배치될 수 있다.
				if( nCount > 0)
				{
					i3TreeNode* pSrcNode = list[0];

					if( i3UILTreeNode_Template* pSrc = ConvertToTemplateNode( pSrcNode))
					{
						i3UITemplate* pTemplate = pSrc->getTemplate();

						if( i3::same_of<i3UITemplate_LVHeader*>(pTemplate) //->IsExactTypeOf( i3UITemplate_LVHeader::static_meta()) 
							|| i3::same_of<i3UITemplate_LVItem*>(pTemplate)) //->IsExactTypeOf( i3UITemplate_LVItem::static_meta()) )
						{
							bInsideFrame = true;
							pInsideFrameMeta = i3UIListView_Box::static_meta();
						}
					}
				}

				pMainWnd->DragMoveObjectTemplate( pt2, bInsideFrame, pInsideFrameMeta);
			}
		}

	}
	else	//else인 경우에만!!!
	{
		CDockablePane::OnMouseMove(nFlags, point);
	}
}


BOOL CUIPaneTemplateExplorer::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch( pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if( GetKeyState( VK_CONTROL) & 0x8000)
			{
				switch( pMsg->wParam)
				{
				case 'C':	OnBtnCopy();		break;
				case 'X':	OnBtnCut();			break;
				case 'V':	OnBtnPaste();		break;
				case 'D':	OnBtnRemove();		break;
				default:	break;
				}
			}
			else
			{
				switch( pMsg->wParam)
				{
				//case VK_RETURN:	I3TRACE("VK_RETURN\n");	break;
				//case VK_ESCAPE: OnKeyDownEscape();	break;
				case VK_F2:	
					{
						//I3TRACE("VK_F2\n");	
						EditLabel( GetFocusedNode());
					}
					break;
				default:	break;
				}
			}
		}
		break;
	default: break;
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}

INT32 CUIPaneTemplateExplorer::GetImageIndex( i3TreeNode* pNode)
{
	INT32 idx = -1;

	if( ConvertToFilterNode( pNode))
	{
		idx = UIGlobalRes::getImageIndex( pNode->meta());
	}
	else if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
	{
		i3UITemplate* pTemplate = pTNode->getTemplate();
		
		I3ASSERT( pTemplate != NULL);
		
		idx = UIGlobalRes::getImageIndex( pTemplate->meta());
	}

	I3ASSERT( idx != -1);

	return idx;
}

void CUIPaneTemplateExplorer::UpdateOtherTemplateWindows( i3UILTreeNode_Template* pTNode)
{
	CMainFrame * pMainWnd = (CMainFrame *) AfxGetMainWnd();
	i3UITemplate * pTemplate = NULL;

	if( pTNode != NULL)
		pTemplate = pTNode->getTemplate();

	pMainWnd->GotoViewTemplateEdit( pTemplate);
}

i3TreeNode* CUIPaneTemplateExplorer::CreateClone( i3TreeNode* pNode)
{
	i3TreeNode* pCloneNode = i3TreeNode::CreateClone( pNode, I3_COPY_INSTANCE);

	//test
	{
		I3TRACE( "Name: %s\n", pCloneNode->GetName());

		for( INT32 i=0; i< pCloneNode->getChildCount(); i++)
		{
			i3TreeNode* pChild = pCloneNode->getChild( i);
			I3TRACE( "Name: %s\n", pChild->GetName());
		}
	}

	return pCloneNode;
}

//Rename to unique name
void CUIPaneTemplateExplorer::AdjustNames( i3TreeNode* pNode)
{
	if( pNode == NULL)
		return;

	//i3TreeNode* pParent = pNode->getParent();

	i3UILTreeNode_Filter* pFNode	= ConvertToFilterNode( pNode);
	i3UILTreeNode_Template* pTNode	= ConvertToTemplateNode( pNode);
	if(pFNode)
	{
		for( INT32 i=0; i< pFNode->getChildCount(); i++)
		{
			i3TreeNode* pChild = pFNode->getChild( i);
			AdjustNames( pChild);
		}
		
		if( !_IsUniqueName( pFNode, pFNode->GetName()))
		{
			char buf[MAX_PATH];
			_GetUniqueName( pFNode, pFNode->GetName(), buf, MAX_PATH);
			pFNode->SetName( buf);
		}
	}
	else if(pTNode)
	{
		if( !_IsUniqueName( pTNode, pTNode->GetName()))
		{
			char buf[MAX_PATH];
			_GetUniqueName( pTNode, pTNode->GetName(), buf, MAX_PATH);
			pTNode->SetName( buf);
		}
	}
}

void CUIPaneTemplateExplorer::Rec_AddToLibrary( i3TreeNode* pNode)
{
	if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
	{
		if( m_pLibrary != NULL)
		{
			m_pLibrary->AddElement( pTNode->getTemplate());
		}
	}
	else if( i3UILTreeNode_Filter* pFNode = ConvertToFilterNode( pNode))
	{
		for( INT32 i=0; i< pFNode->getChildCount(); i++)
		{
			i3TreeNode* pNode2 = pFNode->getChild( i);
			Rec_AddToLibrary( pNode2);
		}
	}
}

void CUIPaneTemplateExplorer::Rec_RemoveFromLibrary( i3TreeNode* pNode)
{
	if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
	{
		if( m_pLibrary != NULL)
		{
			m_pLibrary->RemoveElement( pTNode->getTemplate());
		}
	}
	else if( i3UILTreeNode_Filter* pFNode = ConvertToFilterNode( pNode))
	{
		for( INT32 i=0; i< pFNode->getChildCount(); i++)
		{
			i3TreeNode* pNode2 = pFNode->getChild( i);
			Rec_RemoveFromLibrary( pNode2);
		}
	}
}

bool CUIPaneTemplateExplorer::_isEditable( i3TreeNode* pNode)
{
	if( ConvertToTemplateNode( pNode))
		return true;

	return false;
}

bool CUIPaneTemplateExplorer::_isScriptEditable( i3TreeNode* pNode)
{
	if( ConvertToFilterNode( pNode))
	{
		if( pNode == m_pRoot)
			return true;
	}
	else if( i3UILTreeNode_Template* pTNode = ConvertToTemplateNode( pNode))
	{
		i3UITemplate* pTemplate = pTNode->getTemplate();
		I3ASSERT( pTemplate != NULL);

		if( i3::same_of<i3UITemplate_MessageBox*>(pTemplate) //->IsExactTypeOf( i3UITemplate_MessageBox::static_meta()) 
			|| i3::kind_of<i3UITemplate_User*>(pTemplate)) //->IsTypeOf( i3UITemplate_User::static_meta()))
		{
			return true;
		}
	}

	return false;
}

void CUIPaneTemplateExplorer::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	HWND hCurWnd = m_bShowListView ? m_ListView.m_hWnd : m_TreeView.m_hWnd;
	
	I3ASSERT( hCurWnd != NULL)
	//if( hCurWnd != NULL)
	{
		switch( nIDEvent)
		{
		case TIMER_HOVERING:	
			m_bHovering = true;		
			break;
		case TIMER_SCROLLUP:	
			if( m_bHovering)	
				::SendMessage( hCurWnd, WM_VSCROLL, MAKEWPARAM( SB_LINEUP, 0), NULL);		
			break;
		case TIMER_SCROLLDOWN:	
			if( m_bHovering)
				::SendMessage( hCurWnd, WM_VSCROLL, MAKEWPARAM( SB_LINEDOWN, 0), NULL);		
			break;

		case IDC_ED_SEARCH :
			OnChangeFindString();
			break;

		default: 
			break;
		}
	}
	
	//CDockablePane::OnTimer(nIDEvent);
}

void CUIPaneTemplateExplorer::OnChangeFindString(void)
{
	m_ED_Find.GetWindowText( m_FindFilter);

	I3TRACE( "FIND : %s\n", LPCTSTR( m_FindFilter));

	KillTimer( IDC_ED_SEARCH);

	if( m_bShowListView)
	{
		OnBtnTreeView();
	}
	else
	{
		UpdateView();
	}

	m_ED_Find.SetFocus();
}


BOOL CUIPaneTemplateExplorer::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN;

	return CUIPaneBase::PreCreateWindow(cs);
}

void CUIPaneTemplateExplorer::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	switch( pInfo->m_Event)
	{
		case I3_TDK_UPDATE_SET :
			{
				SetUILibrary( (i3UILibrary *) pInfo->m_pObject);
			}
			break;
	}
}

LRESULT CUIPaneTemplateExplorer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CUIPaneBase::WindowProc(message, wParam, lParam);
}
