// SingleSelectMetaDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "Dlg_SelTemplateType.h"
#include "UIGlobalRes.h"

// CDlg_SelTemplateType dialog

IMPLEMENT_DYNAMIC(CDlg_SelTemplateType, CDialog)

CDlg_SelTemplateType::CDlg_SelTemplateType(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_SelTemplateType::IDD, pParent)
{
	m_szSelectedMeta[0] = 0;
}

CDlg_SelTemplateType::~CDlg_SelTemplateType()
{
}

void CDlg_SelTemplateType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEMPLATE_LIST, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CDlg_SelTemplateType, CDialog)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_TEMPLATE_LIST, &CDlg_SelTemplateType::OnNMDblclkTemplateList)
END_MESSAGE_MAP()


// CDlg_SelTemplateType message handlers

BOOL CDlg_SelTemplateType::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pBaseMeta = i3UITemplate::static_meta();

	{
		INT32 idx;
		i3::vector<i3ClassMeta*> List;
		i3ClassMeta * pMeta;

		m_ListCtrl.SetImageList( UIGlobalRes::getDefBigImageList(), LVSIL_NORMAL);

		m_pBaseMeta->get_all_derived_metas( List, true);

		for(size_t i = 0; i < List.size(); i++)
		{
			pMeta = List[i];

			//쓰지도 않는 더블텍스트 건너띄자..
			if (i3::generic_is_iequal(pMeta->class_name().c_str(),"i3UITemplate_DoubleText") )
				continue;

			if( pMeta->get_class_meta_style() == i3::cms_abstract) // IsAbstractClass())
				continue;

			i3UITemplate * pTemplate = (i3UITemplate *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);

			{
				const char * pszName;
				INT32 idxIcon;

				pszName = pTemplate->getTemplateName();
				idxIcon = UIGlobalRes::getCategoryImageIndex( pTemplate->getTemplateCategory());

				idx = m_ListCtrl.InsertItem( i, pszName, idxIcon);

				m_ListCtrl.SetItemData( idx, (DWORD_PTR) pTemplate);
				
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_SelTemplateType::OnOK()
{
	i3UITemplate * pSelTemplate;

	{
		POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();

		if( pos == NULL)
			return;

		INT32 idx = m_ListCtrl.GetNextSelectedItem( pos);
		
		pSelTemplate = (i3UITemplate *) m_ListCtrl.GetItemData( idx);
	}

	{
		i3ClassMeta * pMeta = pSelTemplate->meta();

		i3::string_ncopy_nullpad( m_szSelectedMeta, pMeta->class_name().c_str(), sizeof(m_szSelectedMeta));
	}

	CDialog::OnOK();
}

void CDlg_SelTemplateType::OnDestroy()
{
	// List에 있는 Template Class들을 제거해야 한다.
	{
		INT32 i;
		INT32 cnt = m_ListCtrl.GetItemCount();

		for( i = 0; i < cnt; i++)
		{
			i3UITemplate * pTemplate;

			pTemplate = (i3UITemplate *) m_ListCtrl.GetItemData( i);
			
			i3::destroy_instance(pTemplate);
//			delete pTemplate;
		}
	}

	CDialog::OnDestroy();
}

void CDlg_SelTemplateType::OnNMDblclkTemplateList(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();

	if( pos != NULL)
	{
		OnOK();
	}

	*pResult = 0;
}
