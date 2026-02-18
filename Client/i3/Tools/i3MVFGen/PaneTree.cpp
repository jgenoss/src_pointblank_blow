// PaneTree.cpp : implementation file
//

#include "stdafx.h"
#include "i3MVFGen.h"
#include "PaneTree.h"
#include ".\panetree.h"
#include "GlobVar.h"

// CPaneTree dialog

IMPLEMENT_DYNAMIC(CPaneTree, CDialog)
CPaneTree::CPaneTree(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneTree::IDD, pParent)
{
	m_hModule = INVALID_HANDLE_VALUE;
	m_pIImageList = NULL;
	m_pImageList = NULL;
}

CPaneTree::~CPaneTree()
{
	if( m_hModule != INVALID_HANDLE_VALUE)
	{
		FreeLibrary( (HMODULE) m_hModule);
		m_hModule = INVALID_HANDLE_VALUE;
	}

	if( m_pIImageList != NULL)
	{
		m_pIImageList->Release();
		m_pIImageList = NULL;
	}

	if( m_pImageList != NULL)
	{
		delete m_pImageList;
		m_pImageList = NULL;
	}
}

void CPaneTree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_TreeCtrl);
}

void CPaneTree::SetTreeInfo( TVITEM * pInfo, MVFNode * pNode)
{
	memset( pInfo, 0, sizeof(TVITEM));

	pInfo->mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE;

	if( pNode->GetStyle() & I3MVF_ATTR_FOLDER)
		pInfo->cChildren = 1;
	else
		pInfo->cChildren = 0;

	{
		char szPath[MAX_PATH];
		SHFILEINFO fi;

		pNode->GetFullPath( szPath);

		I3TRACE( "%s\n", szPath);
		SHGetFileInfo( szPath, FILE_ATTRIBUTE_NORMAL, &fi, sizeof(SHFILEINFO), 
			SHGFI_ICON | SHGFI_SYSICONINDEX);

		pInfo->iImage = fi.iIcon;
	}

	pInfo->lParam = (LPARAM) pNode;
	pInfo->pszText = LPSTR_TEXTCALLBACK; //pNode->GetName();
}

void CPaneTree::UpdateTree( HTREEITEM hItem, MVFNode * pNode)
{
	

}

HTREEITEM CPaneTree::AddTree( HTREEITEM hParent, MVFNode * pNode)
{
	TVINSERTSTRUCT data;
	HTREEITEM hItem;

	data.hParent = hParent;
	data.hInsertAfter = TVI_LAST;

	SetTreeInfo( &data.item, pNode);

	hItem = m_TreeCtrl.InsertItem( &data);

	return hItem;
}

void CPaneTree::Rec_AddTree( HTREEITEM hParent, MVFNode * pNode)
{
	HTREEITEM hItem;
	INT32 i;
	MVFNode * pChild;

	hItem = AddTree( hParent, pNode);

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		Rec_AddTree( hItem, pChild);
	}
}

void CPaneTree::UpdateAll(void)
{
	m_TreeCtrl.DeleteAllItems();

	Rec_AddTree( TVI_ROOT, g_pRoot);
}

BEGIN_MESSAGE_MAP(CPaneTree, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_GETDISPINFO, IDC_TREE, OnTvnGetdispinfoTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnNMRclickTree)
END_MESSAGE_MAP()


// CPaneTree message handlers

typedef HRESULT (FAR WINAPI *SHPROC)( int, REFIID, void **);


BOOL CPaneTree::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		HRESULT rv;
		SHPROC _SHGetImageList;

		{
			m_hModule = LoadLibrary( "Shell32.dll");
		}
		
		_SHGetImageList = (SHPROC) GetProcAddress( (HMODULE) m_hModule, (LPCTSTR) "SHGetImageList");
		I3ASSERT( _SHGetImageList != NULL);

		rv = _SHGetImageList( SHIL_SMALL, IID_IImageList, (void **) &m_pIImageList);
		if( FAILED( rv))
		{
			AfxMessageBox( "Could not get the System Image List interface object.");
			return FALSE;
		}

		{
			m_pImageList = new CImageList;

			m_pImageList->Attach( (HIMAGELIST) m_pIImageList);
		}

		m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPaneTree::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

static char temp[512];

void CPaneTree::OnTvnGetdispinfoTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	char conv[128];
	MVFNode * pNode = (MVFNode *) pTVDispInfo->item.lParam;

	if( pNode->GetStyle() & I3MVF_ATTR_FOLDER)
	{
		pTVDispInfo->item.pszText = (char *) pNode->GetName();
	}
	else
	{
		i3String::ValueToStr( pNode->GetSize(), conv, sizeof(conv) - 1);
		
		sprintf( temp, "%s (%s)\n", pNode->GetName(), conv);

		pTVDispInfo->item.pszText = temp;
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

		Menu.LoadMenu( MAKEINTRESOURCE( IDR_POPUP_NODE));

		pPopup = Menu.GetSubMenu( 0);

		cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
					pt.x, pt.y, (CWnd *) this, NULL);

		switch( cmd)
		{
			case ID_NODE_DELETE :
				{
					HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();

					if( hItem != NULL)
					{
						OnDelete( hItem);
					}
				}
				break;
		}
	}

	*pResult = 0;
}

void CPaneTree::OnDelete( HTREEITEM hItem)
{
	MVFNode * pNode = (MVFNode *) m_TreeCtrl.GetItemData( hItem);
	MVFNode * pParent;

	pParent = pNode->GetParent();
	pParent->RemoveChild( pNode);

	m_TreeCtrl.DeleteItem( hItem);
}
