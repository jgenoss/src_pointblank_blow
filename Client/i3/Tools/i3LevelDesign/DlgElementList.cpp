// DlgElementList.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgElementList.h"
#include "DlgRename.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "GlobalVariable.h"
// CDlgElementList dialog

static INT32	s_FilterCtrlID[] =
{
	IDC_CHK_VISIBLE_BOX,
	IDC_CHK_VISIBLE_SPHERE,
	IDC_CHK_RESPAWN,
	IDC_CHK_VISIBLE_TRIGGER,
	IDC_CHK_VISIBLE_WORLD,
	IDC_CHK_VISIBLE_CHARA,
	IDC_CHK_VISIBLE_OBJECT,
	IDC_CHK_VISIBLE_LIGHT,
	IDC_CHK_VISIBLE_CAMERA,
	IDC_CHK_VISIBLE_PATH,
	IDC_CHK_VISIBLE_STATICOBJECT,
	IDC_CHK_CAPSULE,
	-1
};

IMPLEMENT_DYNAMIC(CDlgElementList, CDialog)
CDlgElementList::CDlgElementList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgElementList::IDD, pParent) 
{
}

CDlgElementList::~CDlgElementList()
{
}

void CDlgElementList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_SEARCHNAME, m_SearchNameCtrl);
	DDX_Control(pDX, IDC_ELM_LIST, m_ElmListCtrl);
	DDX_Control(pDX, IDC_BTN_SELALL, m_SelAllCtrl);
	DDX_Control(pDX, IDC_BTN_UNSELALL, m_UnselAllCtrl);
	DDX_Control(pDX, IDC_BTN_SELINVERT, m_SelInvertCtrl);
	DDX_Control(pDX, IDOK, m_OKCtrl);
	DDX_Control(pDX, IDCANCEL, m_CancelCtrl);
	DDX_Control(pDX, IDC_ELMTYPE_GROUP, m_ListGroupCtrl);
}

bool	CDlgElementList::IsVisibleObject( i3LevelElement3D * pElm, UINT32 filter)
{
	UINT32 mask = 0;

	if( pElm->IsStyle( I3_LEVEL_STYLE_VOLATILE))
		return false;

	if( i3::kind_of<i3LevelPrimitive_Box*>(pElm)) //->IsTypeOf( i3LevelPrimitive_Box::static_meta()))
		mask = MSK_PRIM_BOX;
	else if( i3::kind_of<i3LevelPrimitive_Sphere*>(pElm)) //->IsTypeOf( i3LevelPrimitive_Sphere::static_meta()))
		mask = MSK_PRIM_SPHERE;
	else if( i3::kind_of<i3LevelPrimitive_Capsule*>(pElm)) //->IsTypeOf( i3LevelPrimitive_Sphere::static_meta()))
		mask = MSK_PRIM_CAPSULE;
	else if( i3::kind_of<i3LevelRespawn*>(pElm)) //->IsTypeOf( i3LevelRespawn::static_meta()))
		mask = MSK_RESPAWN;
	else if( i3::kind_of<i3LevelTrigger*>(pElm)) //->IsTypeOf( i3LevelTrigger::static_meta()))
		mask = MSK_TRIGGER;
	else if( i3::kind_of<i3LevelWorld*>(pElm)) //->IsTypeOf( i3LevelWorld::static_meta()))
		mask = MSK_WORLD;
	else if( i3::kind_of<i3LevelChara*>(pElm)) //->IsTypeOf( i3LevelChara::static_meta()))
		mask = MSK_CHARA;
	else if( i3::kind_of<i3LevelObject*>(pElm)) //->IsTypeOf( i3LevelObject::static_meta()))
		mask = MSK_OBJECT;
	else if( i3::kind_of<i3LevelLight*>(pElm)) //->IsTypeOf( i3LevelLight::static_meta()))
		mask = MSK_LIGHT;
	else if( i3::kind_of<i3LevelCamera*>(pElm)) //->IsTypeOf( i3LevelCamera::static_meta()))
		mask = MSK_CAMERA;
	else if( i3::kind_of<i3LevelPath*>(pElm)) //->IsTypeOf( i3LevelPath::static_meta()))
		mask = MSK_PATH;
	else if (i3::kind_of<i3LevelStaticObject*>(pElm))
		mask = MSK_STATIC_OBJECT;

	return (mask & filter) != 0;
}

void CDlgElementList::_AssignList(void)
{
	INT32 i, idx;
	i3LevelElement3D * pElm;
	UINT32		filter = GetFilter();

	m_ElmListCtrl.DeleteAllItems();

	for( i = 0; i < g_pScene->GetElementCount(); i++)
	{
		pElm = g_pScene->GetElement( i);

		if( IsVisibleObject( pElm, filter) == false)
			continue;

		idx = m_ElmListCtrl.InsertItem( 0, pElm->GetName(), 0);

		const char* netSyncName = GetNetSyncModeName(pElm->getNetworkSyncMode());
		if(!netSyncName) netSyncName = "ERR!";

		const char* dynStateName = GetDynStateName(pElm->getDynamicState());
		if(!dynStateName) dynStateName = "ERR!";

		m_ElmListCtrl.SetItemText(idx, 1, netSyncName);
		m_ElmListCtrl.SetItemText(idx, 2, dynStateName);

		_UpdateElement( idx, pElm);
	}
}

void	CDlgElementList::_UpdateElement( INT32 idx, i3LevelElement3D * pElm)
{
	m_ElmListCtrl.SetItemData( idx, (DWORD_PTR) pElm);
}

void	CDlgElementList::GetSelectedItems( i3::vector<i3LevelElement3D*>& List)
{
	INT32 total;

	total = m_ElmListCtrl.GetItemCount();

	for( INT32 i = 0; i < total; i++)
	{
		UINT32 state = m_ElmListCtrl.GetItemState( i, LVIS_SELECTED);

		if( state & LVIS_SELECTED)
		{
			List.push_back( reinterpret_cast<i3LevelElement3D*>(m_ElmListCtrl.GetItemData(i)) );
		}
	}
}

void	CDlgElementList::MakeNewName( char * pszDest, const char * pszFormat, INT32 idx)
{
	char conv[256];
	INT32 len;

	i3::snprintf( conv, _countof( conv), "%d", idx);

	len = i3::generic_string_size( conv);

	while( *pszFormat != 0)
	{
		if( *pszFormat == '#')
		{
			*pszDest = 0;
			i3::generic_string_cat( pszDest, conv);
			pszDest += len;
		}
		else
		{
			*pszDest = *pszFormat;
			pszDest++;
		}

		pszFormat++;
	}

	*pszDest = 0;
}

UINT32 CDlgElementList::GetFilter(void)
{
	INT32 i, mask = 1, filter = 0;

	for( i = 0; s_FilterCtrlID[i] > 0; i++)
	{
		CButton * pButton = (CButton *) GetDlgItem( s_FilterCtrlID[ i]);

		if( pButton->GetCheck() == BST_CHECKED)
			filter |= mask;

		mask = mask << 1;
	}

	return filter;
}

void	CDlgElementList::SelectAllFilters( FILTER_SELECT_MODE mode)
{
	INT32 i;
			
	for( i = 0; s_FilterCtrlID[i] > 0; i++)
	{
		CButton * pBtn = (CButton *) GetDlgItem( s_FilterCtrlID[i]);

		if( pBtn != NULL)
		{
			switch( mode)
			{
				case FILTER_SELECT :	
					pBtn->SetCheck( BST_CHECKED);
					break;

				case FILTER_UNSELECT :
					pBtn->SetCheck( BST_UNCHECKED);
					break;

				default :
					if( pBtn->GetCheck() == BST_CHECKED)
						pBtn->SetCheck( BST_UNCHECKED);
					else
						pBtn->SetCheck( BST_CHECKED);
					break;
			}
		}
	}
}

BEGIN_MESSAGE_MAP(CDlgElementList, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_SELALL, OnBnClickedBtnSelall)
	ON_BN_CLICKED(IDC_BTN_SELINVERT, OnBnClickedBtnSelinvert)
	ON_BN_CLICKED(IDC_BTN_UNSELALL, OnBnClickedBtnUnselall)
	ON_EN_CHANGE(IDC_ED_SEARCHNAME, &CDlgElementList::OnEnChangeEdSearchname)
	ON_BN_CLICKED(IDC_BTN_RENAME, &CDlgElementList::OnBnClickedBtnRename)

	ON_BN_CLICKED( IDC_CHK_VISIBLE_BOX, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_VISIBLE_SPHERE, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_CAPSULE, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_RESPAWN, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_VISIBLE_TRIGGER, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_VISIBLE_WORLD, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_VISIBLE_CHARA, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_VISIBLE_OBJECT, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_VISIBLE_LIGHT, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_VISIBLE_CAMERA, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_VISIBLE_PATH, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED( IDC_CHK_VISIBLE_STATICOBJECT, &CDlgElementList::OnBnClickedChkVisible)
	ON_BN_CLICKED(IDC_BTN_VISIBLE_SELALL, &CDlgElementList::OnBnClickedBtnVisibleSelall)
	ON_BN_CLICKED(IDC_BTN_VISIBLE_UNSELALL, &CDlgElementList::OnBnClickedBtnVisibleUnselall)
	ON_BN_CLICKED(IDC_BTN_VISIBLE_SELINVERT, &CDlgElementList::OnBnClickedBtnVisibleSelinvert)
END_MESSAGE_MAP()


// CDlgElementList message handlers

BOOL CDlgElementList::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_ElmListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_ElmListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 280);
		m_ElmListCtrl.InsertColumn( 1, "NetSync", LVCFMT_LEFT, 100);
		m_ElmListCtrl.InsertColumn( 2, "DynState", LVCFMT_LEFT, 100);

		SelectAllFilters( FILTER_SELECT);
	}

	_AssignList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgElementList::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	_ReplaceControls( cx, cy);
}

void CDlgElementList::_ReplaceControls( int cx, int cy)
{

}

void CDlgElementList::OnBnClickedBtnSelall()
{
	INT32 i;

	for( i = 0; i < m_ElmListCtrl.GetItemCount(); i++)
	{
		m_ElmListCtrl.SetItemState( i, 0xFFFFFFFF, LVIS_SELECTED);
	}

	//m_ElmListCtrl.Invalidate();
}

void CDlgElementList::OnBnClickedBtnSelinvert()
{
	INT32 i;
	UINT32 state;

	for( i = 0; i < m_ElmListCtrl.GetItemCount(); i++)
	{
		state = m_ElmListCtrl.GetItemState( i, LVIS_SELECTED);

		state = state ^ LVIS_SELECTED;

		m_ElmListCtrl.SetItemState( i, state, LVIS_SELECTED);
	}

	//m_ElmListCtrl.Invalidate();
}

void CDlgElementList::OnBnClickedBtnUnselall()
{
	INT32 i;

	for( i = 0; i < m_ElmListCtrl.GetItemCount(); i++)
	{
		m_ElmListCtrl.SetItemState( i, 0, LVIS_SELECTED);
	}

	m_ElmListCtrl.Invalidate();
}

void CDlgElementList::OnOK()
{
	INT32 i;
	i3LevelElement3D * pElm;

	g_pScene->UnselectAll();

	for( i = 0; i < m_ElmListCtrl.GetItemCount(); i++)
	{
		if( m_ElmListCtrl.GetItemState( i, LVIS_SELECTED))
		{
			pElm = (i3LevelElement3D *) m_ElmListCtrl.GetItemData( i);

			g_pScene->SelectElement( pElm);
		}
	}

	CDialog::OnOK();
}

void CDlgElementList::OnEnChangeEdSearchname()
{
	INT32 i;
	char szStr[512];
	i3LevelElement3D * pElm;
	bool bSearch;
	bool bFirst = true;

	m_SearchNameCtrl.GetWindowText( szStr, sizeof(szStr));

	bSearch = ( szStr[0] != 0);

	for( i = 0; i < m_ElmListCtrl.GetItemCount(); i++)
	{
		pElm = (i3LevelElement3D *) m_ElmListCtrl.GetItemData( i);

		if( (bSearch) && (i3::icontain_string( pElm->GetName(), szStr) != -1))
		{
			m_ElmListCtrl.SetItemState( i, 0xFFFFFFFF, LVIS_SELECTED);

			if( bFirst)
				m_ElmListCtrl.EnsureVisible( i, FALSE);

			bFirst = false;
		}
		else
		{
			m_ElmListCtrl.SetItemState( i, 0, LVIS_SELECTED);
		}
	}

	m_ElmListCtrl.Invalidate();
}

void CDlgElementList::OnBnClickedBtnRename()
{
	CDlgRename dlg;
	char szFormat[MAX_PATH];

	if( dlg.Execute( szFormat, _countof( szFormat)) == false)
		return;

	INT32 idx = 0;
	i3::vector<i3LevelElement3D*>	selList;
	char szNewName[256];

	GetSelectedItems( selList);

	for(size_t i = 0; i < selList.size(); i++, idx++)
	{
		i3LevelElement3D * pElm = selList[i];

		MakeNewName( szNewName, szFormat, idx);		

		pElm->SetName( szNewName);
	}

	_AssignList();
}

void CDlgElementList::OnBnClickedChkVisible()
{
	_AssignList();
}

void CDlgElementList::OnBnClickedBtnVisibleSelall()
{
	SelectAllFilters( FILTER_SELECT);
}

void CDlgElementList::OnBnClickedBtnVisibleUnselall()
{
	SelectAllFilters( FILTER_UNSELECT);
}

void CDlgElementList::OnBnClickedBtnVisibleSelinvert()
{
	SelectAllFilters( FILTER_INVERT);
}
