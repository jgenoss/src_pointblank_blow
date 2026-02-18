// DlgSelectSg.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgSelectSg.h"
#include ".\dlgselectsg.h"


// CDlgSelectSg dialog

IMPLEMENT_DYNAMIC(CDlgSelectSg, CDialog)
CDlgSelectSg::CDlgSelectSg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectSg::IDD, pParent)
{
	m_pRes = NULL;
}

CDlgSelectSg::~CDlgSelectSg()
{
}

void CDlgSelectSg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SGLIST, m_SgListCtrl);
}

bool CDlgSelectSg::Execute(void)
{
	if( DoModal() == IDCANCEL)
		return false;

	return true;
}


BEGIN_MESSAGE_MAP(CDlgSelectSg, CDialog)
END_MESSAGE_MAP()


// CDlgSelectSg message handlers

BOOL CDlgSelectSg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_SgListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_SgListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 160);
		m_SgListCtrl.InsertColumn( 1, "Type", LVCFMT_LEFT, 90);
		m_SgListCtrl.InsertColumn( 2, "Path", LVCFMT_LEFT, 200);
	}

	{
		i3::vector<i3LevelRes*> list;
		INT32 idx;
		i3LevelResSceneGraph * pRes;

		g_pResDB->FindResByType( i3LevelResSceneGraph::static_meta(), list);

		for(size_t i = 0; i < list.size(); i++)
		{
			pRes = static_cast<i3LevelResSceneGraph *>(list[i]);

			idx = m_SgListCtrl.InsertItem( 0, pRes->GetName(), 0);

			// Data
			m_SgListCtrl.SetItemData( idx, (DWORD_PTR) pRes);

			// Type
			switch( pRes->getSgType())
			{
				default :						m_SgListCtrl.SetItemText( idx, 1, "Normal");	break;
				case I3LV_SG_TYPE_BODY :		m_SgListCtrl.SetItemText( idx, 1, "Body");		break;
				case I3LV_SG_TYPE_EFFECT :		m_SgListCtrl.SetItemText( idx, 1, "Fx");		break;
			}

			// Path
			m_SgListCtrl.SetItemText( idx, 2, pRes->getPath());
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSelectSg::OnOK()
{
	POSITION pos = m_SgListCtrl.GetFirstSelectedItemPosition();

	if( pos != NULL)
	{
		INT32 idx = m_SgListCtrl.GetNextSelectedItem( pos);

		m_pRes = (i3LevelResSceneGraph *) m_SgListCtrl.GetItemData( idx);
	}

	CDialog::OnOK();
}
