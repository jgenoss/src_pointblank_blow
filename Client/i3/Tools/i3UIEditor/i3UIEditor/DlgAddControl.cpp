// DlgAddControl.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "DlgAddControl.h"
#include ".\dlgaddcontrol.h"


// CDlgAddControl dialog

IMPLEMENT_DYNAMIC(CDlgAddControl, CDialog)
CDlgAddControl::CDlgAddControl(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddControl::IDD, pParent)
{
	m_pTemplate = NULL;
	m_pCtrl = NULL;
}

CDlgAddControl::~CDlgAddControl()
{
	I3_SAFE_RELEASE( m_pTemplate);
	I3_SAFE_RELEASE( m_pCtrl);
}

void CDlgAddControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_TYPE, m_CB_Type);
	DDX_Control(pDX, IDC_TREE_TEMPLATE, m_TREE_Template);
	DDX_Control(pDX, IDC_LIST_CANDIDATES, m_LV_Candidates);
	DDX_Control(pDX, IDOK, m_BTN_Ok);
}


BEGIN_MESSAGE_MAP(CDlgAddControl, CDialog)
	ON_CBN_SELCHANGE(IDC_CB_TYPE, OnCbnSelchangeCbType)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TEMPLATE, OnTvnSelchangedTreeTemplate)
END_MESSAGE_MAP()


// CDlgAddControl message handlers

BOOL CDlgAddControl::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Control Type 설정
	{
		i3List	list;
		INT32 i;
		i3ClassMeta * pMeta = i3UIControl::GetClassMeta();

		pMeta->GetAllDerivedClasses( &list);

		for( i = 0; i < list.GetCount(); i++)
		{
			pMeta = (i3ClassMeta *) list.Items[i];

			if( pMeta->IsAbstractClass() == FALSE)
			{
				m_CB_Type.AddString( pMeta->GetClassName() + 4);
			}
		}
	}

	{
		m_LV_Candidates.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LV_Candidates.InsertColumn( 0, "Name", LVCFMT_LEFT, 120);
	}

	{
		m_BTN_Ok.EnableWindow( FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddControl::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

void CDlgAddControl::_UpdateTemplate( HTREEITEM hItem, i3UITemplate * pTemplate, i3UITemplate * pParentTemplate, INT32 idxChild)
{
	char conv[512];
	const char * pszComment;

	I3ASSERT( pTemplate != NULL);

	m_TREE_Template.SetItemData( hItem, (DWORD_PTR) pTemplate);

	if( pParentTemplate != NULL)
		pszComment = pParentTemplate->getSubTemplateComment( idxChild);
	else
		pszComment = pTemplate->getComment();

	// Name
	if( pTemplate->hasName())
	{
		i3String::Format( conv, sizeof(conv), "%s (%s)", pTemplate->GetName(), pszComment);	
	}
	else
	{
		i3String::Format( conv, sizeof(conv), "[-] (%s)", pszComment);
	}

	m_TREE_Template.SetItemText( hItem, conv);
}

void CDlgAddControl::_SetTemplate( i3UITemplate * pTemplate, i3UITemplate * pParentTemplate, INT32 idxChild, HTREEITEM hParent)
{
	HTREEITEM hItem;

	I3ASSERT( pTemplate->IsTypeOf( i3UITemplate::GetClassMeta()));

	{
		hItem = m_TREE_Template.InsertItem( "", 0, 0, hParent);

		_UpdateTemplate( hItem, pTemplate, pParentTemplate, idxChild);
	}

	INT32 i;
	i3UITemplate * pSub;

	for( i = 0; i < pTemplate->getSubTemplateCount(); i++)
	{
		pSub = pTemplate->getSubTemplate( i);

		_SetTemplate( pSub, pTemplate, i, hItem);
	}
}

void CDlgAddControl::OnCbnSelchangeCbType()
{
	char conv[256], temp[256];

	// 현재 선택된 Control Type에 대한 초기화

	// Template Tree
	{
		i3ClassMeta * pMeta;

		m_CB_Type.GetWindowText( temp, sizeof(temp));

		sprintf( conv, "i3UITemplate_%s", temp);

		pMeta = i3ClassMeta::FindClassMetaByName( conv);

		m_TREE_Template.DeleteAllItems();
		m_LV_Candidates.DeleteAllItems();

		I3_SAFE_RELEASE( m_pTemplate);

		if( pMeta != NULL)
		{
			m_pTemplate = (i3UITemplate *) CREATEINSTANCE( pMeta);

			m_pTemplate->AddRef();

			_SetTemplate( m_pTemplate, NULL, 0);
		}
	}
}

void CDlgAddControl::OnTvnSelchangedTreeTemplate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pInfo = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	HTREEITEM hNewItem = pInfo->itemNew.hItem;

	if( hNewItem != NULL)
	{
		i3UITemplate * pTemplate = (i3UITemplate *) m_TREE_Template.GetItemData( hNewItem);
	}
	else
	{
	}

	*pResult = 0;
}
