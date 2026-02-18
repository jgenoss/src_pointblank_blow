// SelectTemplateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "SelectTemplateDialog.h"
#include "GlobalVariable.h"

// CSelectTemplateDialog dialog

IMPLEMENT_DYNAMIC(CSelectTemplateDialog, CDialog)
CSelectTemplateDialog::CSelectTemplateDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectTemplateDialog::IDD, pParent)
{
	m_pSelectedTemplate = NULL;
	m_pTemplate		= NULL;
}

CSelectTemplateDialog::~CSelectTemplateDialog()
{
	m_pSelectedTemplate = NULL;
	m_pTemplate		= NULL;
}

void CSelectTemplateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SELTEMPLATE, m_CtrlTemplateListView);
	DDX_Control(pDX, IDOK, m_CtrlOK);
	DDX_Control(pDX, IDCANCEL, m_CtrlCancel);
}


BEGIN_MESSAGE_MAP(CSelectTemplateDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_SELTEMPLATE, OnNMClickListSeltemplate)
END_MESSAGE_MAP()


// CSelectTemplateDialog message handlers

void CSelectTemplateDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

void CSelectTemplateDialog::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

BOOL CSelectTemplateDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_CtrlTemplateListView.InsertColumn( 0, "Number", LVCFMT_CENTER, 64);
	m_CtrlTemplateListView.InsertColumn( 1, "Template Name", LVCFMT_LEFT, 250);
	m_CtrlTemplateListView.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	_SetTemplateList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectTemplateDialog::_OnDrawTemplate(HDC hdc)
{
	INT32	nShapeCount = m_pSelectedTemplate->getShapeCount();
	INT32	i = 0;

	for(i = 0; i < nShapeCount; ++i)
	{
		i3UIImage * pImage = m_pSelectedTemplate->getShape(i);
		I3ASSERT( pImage != NULL );
		if( pImage->m_pTexture != NULL)
		{
			i3GuiToolImageElement * pElm = 	g_pResList->FindElementByTexture( pImage->m_pTexture);
			HBITMAP	hBitmap	= pElm->getBitmap();
			I3ASSERT( hBitmap != NULL);

			i3TDK::AlphaBlend(hdc,
							(INT32)(m_RectDrawRect.left + pImage->m_nX),
							(INT32)(m_RectDrawRect.top + pImage->m_nY),
							(INT32)pImage->m_nWidth,
							(INT32)pImage->m_nHeight,
							hBitmap,
							pImage->m_u,
							pImage->m_v,
							pImage->m_w,
							pImage->m_h
						);
		}
	}	
}


void CSelectTemplateDialog::_SetTemplateList()
{
	if( m_pTemplate == NULL)	
		return ;

	i3List * pTempList = m_pTemplate->getTemplateList();
	if( pTempList != NULL )
	{
		INT32	nCount = pTempList->GetCount();
		INT32	i;

		for( i = 0;i < nCount; ++i)
		{
			i3UITemplate * pElement = (i3UITemplate*)pTempList->GetItem(i);
			if( pElement != NULL )
			{
				_AddItem( pElement);
			}
		}
	}
}

void	CSelectTemplateDialog::_AddItem( i3UITemplate * pElement )
{
	char szName[1024];
	INT32	ItemIndex;

	sprintf( szName, "%d", m_CtrlTemplateListView.GetItemCount());
	ItemIndex = m_CtrlTemplateListView.InsertItem( m_CtrlTemplateListView.GetItemCount(),szName, 0);
	
	sprintf( szName, "%s", pElement->GetName());
	if( szName[0] == 0 )	strcpy( szName, "No Named Template");
	
	m_CtrlTemplateListView.SetItemText( ItemIndex, 1, szName);
	m_CtrlTemplateListView.SetItemData( ItemIndex, (DWORD_PTR)pElement);
}

BOOL CSelectTemplateDialog::_IsValidTemplateElement( i3UITemplate * pElement)
{
	if( pElement == NULL )
	{
		AfxMessageBox("선택된 템플릿이 없습니다");
		return FALSE;
	}

	//Check Valid Texture Image
	INT32	i;
	INT32	nCount = pElement->getShapeCount();
	for( i = 0; i< nCount ; ++i)
	{
		i3UIImage * pImage = (i3UIImage*)pElement->getShape(i);
		
		if( pImage == NULL )
		{
			AfxMessageBox("초기화가 잘못된 템플릿입니다. 지우고 새로 만드삼");
			return FALSE;
		}

		if( pImage->m_pTexture == NULL )
		{
			AfxMessageBox("Texture가 지정되지 않은 Shape가 존재합니다.");
			return FALSE;
		}
	}

	return TRUE;
}

void CSelectTemplateDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	if( m_pTemplate == NULL )	return ;

	dc.FillSolidRect( &m_RectDrawRect, RGB(0,0,0));

	if( m_pSelectedTemplate != NULL )
	{
		_OnDrawTemplate(dc.m_hDC);
	}
}

void CSelectTemplateDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	INT32	X = (INT32)cx/3;
	m_RectDrawRect.left = X + 5;
	m_RectDrawRect.top  = 5;
	m_RectDrawRect.right= cx - 5;
	m_RectDrawRect.bottom = cy - 5;

	// TODO: Add your message handler code here
	if( ::IsWindow( m_CtrlTemplateListView.m_hWnd))
	{
		m_CtrlTemplateListView.SetWindowPos( NULL, 0, 0, X, cy - 45, SWP_NOZORDER| SWP_NOACTIVATE);
	}

	if( ::IsWindow( m_CtrlOK.m_hWnd))
	{
		m_CtrlOK.SetWindowPos( NULL, 3, cy - 40, (INT32)(X/2) - 5, 35, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	if( ::IsWindow( m_CtrlCancel.m_hWnd))
	{
		m_CtrlCancel.SetWindowPos( NULL, (INT32)(X/2) +2 , cy-40, (INT32)(X/2) - 5, 35, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	Invalidate();
}

void CSelectTemplateDialog::OnNMClickListSeltemplate(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_CtrlTemplateListView.GetFirstSelectedItemPosition();

	if( pos != NULL )
	{
		INT32	idx = m_CtrlTemplateListView.GetNextSelectedItem( pos );

		i3UITemplate * pElement = (i3UITemplate*)m_CtrlTemplateListView.GetItemData( idx );

		if( pElement != m_pSelectedTemplate)
		{
			m_pSelectedTemplate = pElement;

			Invalidate();
		}

	}

	*pResult = 0;
}
