// PaneTree.cpp : implementation file
//
#include "stdafx.h"
#include "i3RSCGen.h"
#include "PaneTree.h"

#include "GlobalVar.h"
#include ".\panetree.h"

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

static int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
  // lParamSort contains a pointer to the tree control.
  // The lParam of an item is just its handle.
  CTreeCtrl* pmyTreeCtrl = (CTreeCtrl*) lParamSort;
  CString    strItem1 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam1);
  CString    strItem2 = pmyTreeCtrl->GetItemText((HTREEITEM) lParam2);

  return strcmp(strItem2, strItem1);
}

// CPaneTree dialog
IMPLEMENT_DYNAMIC(CPaneTree, CDialog)
CPaneTree::CPaneTree(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneTree::IDD, pParent)
{
	m_pIImageList			= NULL;
	m_pImageList			= NULL;
	m_nIndexOfFirstTDKImage	= 0;
}

CPaneTree::~CPaneTree()
{
	I3_SAFE_RELEASE_NODBG( m_pIImageList);
	I3_SAFE_DELETE( m_pImageList);
}

void CPaneTree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
}


BEGIN_MESSAGE_MAP(CPaneTree, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE1, OnTvnGetdispinfoTree1)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, OnNMRclickTree)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnNMClickTree1)
	ON_NOTIFY(TVN_BEGINLABELEDIT, IDC_TREE1, OnTvnBeginlabeleditTree1)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CPaneTree message handlers
BOOL CPaneTree::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pImageList			= i3TDK::GetSystemImageList();
	m_nIndexOfFirstTDKImage = i3TDK::AddTDKImageListTo( m_pImageList);

	m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);

	TVSORTCB tvs;

	tvs.hParent		= TVI_ROOT;
	tvs.lpfnCompare = MyCompareProc;
	tvs.lParam		= (LPARAM)&m_TreeCtrl;

	m_TreeCtrl.SortChildrenCB( &tvs);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneTree::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPaneTree::SetTreeInfo( TVITEM * pInfo, RSCNode * pNode)
{
	memset( pInfo, 0, sizeof( TVITEM));

	pInfo->mask	= TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;

	if( pNode->GetStyle() & I3RSC_ATTR_FOLDER)
		pInfo->cChildren = 1;
	else
		pInfo->cChildren = 0;

	{
		i3::string full_path;
		SHFILEINFO fi;
		INT32	nStyle = 0;
		INT32	nImageIndex = m_nIndexOfFirstTDKImage;

		pNode->GetFullPath(g_pProfile->m_strWorkDir, full_path);

		SHGetFileInfo( full_path.c_str(), FILE_ATTRIBUTE_NORMAL, &fi, sizeof(SHFILEINFO), 
			SHGFI_SYSICONINDEX);

		if( pNode->GetStyle() & I3RSC_ATTR_FOLDER)
		{
			nImageIndex = fi.iIcon;
		}
		else
		{
			nStyle = pNode->GetStyle() & I3RSC_TYPE_MASK;

			if( nStyle == I3RSC_TYPE_SCENE)				nImageIndex += I3ICON_NODE;
			else if( nStyle == I3RSC_TYPE_ANIMATION)	nImageIndex += I3ICON_ANIM;
			else if( nStyle == I3RSC_TYPE_SOUND)		nImageIndex += I3ICON_SOUND;
			else if( nStyle == I3RSC_TYPE_TEXTURE)		nImageIndex += I3ICON_TEXTURE;
			else										nImageIndex = fi.iIcon;
		}

		pInfo->iImage = nImageIndex;
		pInfo->iSelectedImage = nImageIndex + 1;
	}

	pInfo->lParam = ( LPARAM) pNode;
	pInfo->pszText= LPSTR_TEXTCALLBACK;
}

void CPaneTree::UpdateTree( HTREEITEM hItem, RSCNode * pNode)
{
	m_TreeCtrl.UpdateData( TRUE);
}

HTREEITEM CPaneTree::AddTree( HTREEITEM hParent, RSCNode * pNode)
{
	TVINSERTSTRUCT	data;
	HTREEITEM		hItem;

	data.hParent		= hParent;
	data.hInsertAfter	= TVI_LAST;

	SetTreeInfo( &data.item, pNode);

	hItem	= m_TreeCtrl.InsertItem( &data);

	m_TreeCtrl.SortChildren( hParent);

	return hItem;
}

HTREEITEM	CPaneTree::GetRootItem() const
{
	return m_TreeCtrl.GetRootItem();
}



void CPaneTree::Rec_AddTree( HTREEITEM hParent, RSCNode * pNode)
{
	HTREEITEM hItem;
	INT32		i;
	RSCNode * pChild;

	hItem	= AddTree( hParent, pNode);

	for ( i =0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		Rec_AddTree( hItem, pChild);
	}
}

void CPaneTree::UpdateAll( void)
{
	m_TreeCtrl.DeleteAllItems();

	if( g_pRoot != NULL)
	{
		Rec_AddTree( TVI_ROOT, g_pRoot);
	}
}

void CPaneTree::OnDelete( HTREEITEM hItem)
{
	RSCNode * pNode = (RSCNode*) m_TreeCtrl.GetItemData( hItem);
	RSCNode * pParent = NULL;

	pParent = pNode->GetParent();
	// 2011-05-06 김재진
	// 재귀호출 함수 : 자식노드가 있으면 자식노드 삭제함수
	ChildDelete(hItem);

	if(m_FileList.empty())
		m_FileList.clear();

	pParent->RemoveChild( pNode);

	m_TreeCtrl.DeleteItem( hItem);
	I3PRINTLOG(I3LOG_NOTICE, "Delete Item");
}

// 2011-05-06 김재진
void CPaneTree::ChildDelete(HTREEITEM hItem)
{
	if( hItem == NULL) return;
	
	HTREEITEM child = m_TreeCtrl.GetChildItem(hItem);
	RSCNode * pNode = (RSCNode*) m_TreeCtrl.GetItemData( hItem);
	
	INT32 cnt = pNode->GetChildCount();

	if(cnt > 0 && child != NULL)
	{
		// 자식노드 삭제를 위한 반복문
		// 자식노드 갯수만큼 반복
		for( ; cnt > 0 ; cnt--)
		{
			ChildDelete(child);
			child = m_TreeCtrl.GetNextSiblingItem(child);
		}
	}

	FileNameList::iterator pos = m_FileList.find(pNode->GetName());
	if( pos != m_FileList.end() )
		m_FileList.erase(pos); 
}  
///////////////////////////////////

void CPaneTree::OnTvnGetdispinfoTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	RSCNode * pNode	= ( RSCNode *)pTVDispInfo->item.lParam;
	if( pNode != NULL)
	{
		pTVDispInfo->item.pszText = (char*)pNode->GetName();
	}

	*pResult = 0;
}


void CPaneTree::OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	UINT uFlags;
	HTREEITEM hItem;
	CPoint point;

	GetCursorPos( &point);

	m_TreeCtrl.ScreenToClient( &point);
	
	hItem = m_TreeCtrl.HitTest( point, &uFlags);

	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		m_TreeCtrl.SelectItem( hItem);
	}

	{
		CMenu Menu, * pPopup;
		POINT pt;
		UINT cmd;

		GetCursorPos( &pt);

		Menu.LoadMenu( MAKEINTRESOURCE( IDR_POPUP_NODE_COMMON));

		pPopup = Menu.GetSubMenu( 0);

		cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
					pt.x, pt.y, (CWnd *) this, NULL);

		switch( cmd)
		{
			case ID_NODE_COMMON_DELETE :
				{
					HTREEITEM hItem2 = m_TreeCtrl.GetSelectedItem();

					if( hItem2 != NULL)
					{
						HTREEITEM child = m_TreeCtrl.GetChildItem(hItem2);
						if(child != NULL)
						{
							if ( AfxMessageBox("하위에 데이터가 존재합니다. 삭제할 수 없습니다.") == IDOK)
								return ;
						}

						if( AfxMessageBox("삭제하시겠습니까?", MB_YESNO) == IDYES )
							OnDelete( hItem2);
					}
				}
				break;
		}
	}

	*pResult = 0;
}

void CPaneTree::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	UINT uFlags;
	HTREEITEM hItem;
	CPoint point;

	GetCursorPos( &point);

	m_TreeCtrl.ScreenToClient( &point);
	
	hItem = m_TreeCtrl.HitTest( point, &uFlags);

	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		m_TreeCtrl.SelectItem( hItem);

		RSCNode * pNode	= ( RSCNode *)m_TreeCtrl.GetItemData( hItem);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pNode);
	}

	*pResult = 0;
}

void CPaneTree::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
}

void CPaneTree::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}

void CPaneTree::OnTvnBeginlabeleditTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

BOOL CPaneTree::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		switch( pMsg->wParam )
		{
		case VK_DELETE:
			{
				HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
				if( hItem != NULL )
				{
					HTREEITEM child = m_TreeCtrl.GetChildItem(hItem);
					if(child != NULL)
					{
						if ( AfxMessageBox("하위에 데이터가 존재합니다. 삭제할 수 없습니다.") == IDOK)
							return 0;
					}

					if( AfxMessageBox("삭제하시겠습니까?", MB_YESNO) == IDYES )
						OnDelete( hItem);
				}
			}
			
			break;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
