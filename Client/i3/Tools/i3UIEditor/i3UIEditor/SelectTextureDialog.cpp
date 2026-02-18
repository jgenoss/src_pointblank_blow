// SelectTextureDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "SelectTextureDialog.h"
#include "i3UIEditorDef.h"


// CSelectTextureDialog dialog

IMPLEMENT_DYNAMIC(CSelectTextureDialog, CDialog)
CSelectTextureDialog::CSelectTextureDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectTextureDialog::IDD, pParent)
{
	m_pSelectedTexture  = NULL;
	m_pTextureList		= NULL;
}

CSelectTextureDialog::~CSelectTextureDialog()
{
	m_pSelectedTexture = NULL;
	m_pTextureList = NULL;
}

void CSelectTextureDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELTEX, m_CtrlTexList);
	DDX_Control(pDX, IDOK, m_CtrlOK);
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
}


BEGIN_MESSAGE_MAP(CSelectTextureDialog, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_SELTEX, OnNMClickListSeltex)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SELTEX, OnNMDblclkListSeltex)
END_MESSAGE_MAP()


// CSelectTextureDialog message handlers

BOOL CSelectTextureDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_PrevCtrl.Create( WS_CHILD | WS_VISIBLE, this, 0);

	m_CtrlTexList.InsertColumn( 0, "Number", LVCFMT_CENTER, 64);
	m_CtrlTexList.InsertColumn( 1, "Texture Name", LVCFMT_LEFT, 250);
	m_CtrlTexList.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	if( m_pTextureList != NULL)
	{
		_SetTextureList( m_pTextureList);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void	CSelectTextureDialog::AddItem( i3Texture * pTexture )
{
	char szName[1024];
	INT32	ItemIndex;

	sprintf( szName, "%d", m_CtrlTexList.GetItemCount());
	ItemIndex = m_CtrlTexList.InsertItem( m_CtrlTexList.GetItemCount(),szName, 0);
	
	sprintf( szName, "%s", pTexture->GetName());
	if( szName[0] == 0 )	strcpy( szName, "No Named Texture");
	
	m_CtrlTexList.SetItemText( ItemIndex, 1, szName);
	m_CtrlTexList.SetItemData( ItemIndex, (DWORD_PTR)pTexture);
}

void	CSelectTextureDialog::_SetTextureList( i3List * pList)
{
	if( pList == NULL )	return ;

	INT32	nCount = pList->GetCount();
	for( INT32	i = 0;i < nCount; ++i)
	{
		i3GuiToolImageElement * pElm = ( i3GuiToolImageElement*)pList->GetItem(i);

		i3Texture * pTexture = pElm->getTexture();
		if( pTexture != NULL )
		{
			AddItem(pTexture );
		}
	}
}

void CSelectTextureDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here

	INT32	ListX = (INT32)cx/3;
	INT32	PrevX = cx - ListX;

	if( ::IsWindow( m_CtrlTexList.m_hWnd))
	{
		m_CtrlTexList.SetWindowPos( NULL, 0, 0, ListX, cy - 45, SWP_NOZORDER| SWP_NOACTIVATE);
	}

	if( ::IsWindow( m_PrevCtrl.m_hWnd))
	{
		m_PrevCtrl.SetWindowPos( NULL, ListX + 5, 5, PrevX - 10, cy - 10, SWP_NOZORDER| SWP_NOACTIVATE);
	}

	if( ::IsWindow( m_CtrlOK.m_hWnd))
	{
		m_CtrlOK.SetWindowPos( NULL, 3, cy- 40, (INT32)(ListX/2) - 5, 35, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	if( ::IsWindow( m_CtrlCancel.m_hWnd))
	{
		m_CtrlCancel.SetWindowPos( NULL, (INT32)(ListX/2) +2 , cy-40, (INT32)(ListX/2) - 5, 35, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void CSelectTextureDialog::OnNMClickListSeltex(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_CtrlTexList.GetFirstSelectedItemPosition();

	if( pos != NULL )
	{
		INT32	idx = m_CtrlTexList.GetNextSelectedItem( pos );

		i3Texture * pTex = (i3Texture*)m_CtrlTexList.GetItemData( idx );

		m_pSelectedTexture  = pTex;

		m_PrevCtrl.SetImage( pTex );
	}
	
	*pResult = 0;
}

void CSelectTextureDialog::OnNMDblclkListSeltex(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_CtrlTexList.GetFirstSelectedItemPosition();

	if( pos != NULL )
	{
		INT32	idx = m_CtrlTexList.GetNextSelectedItem( pos );

		i3Texture * pTex = (i3Texture*)m_CtrlTexList.GetItemData( idx );

		if( pTex != m_pSelectedTexture )
		{
			m_pSelectedTexture  = pTex;

			m_PrevCtrl.SetImage( pTex );
		}
	}

	*pResult = 0;

	if( m_pSelectedTexture != NULL)
	{
		CDialog::OnOK();
	}
}
