// Prop_LvRes_SceneGraph.cpp : implementation file
//

#include "pch.h"
#if defined(USE_EDITDLG)//CObjSpec_TargetTrigger dialog
#include "ObjSpec_TargetTrigger.h"
#include "GameObject.h"
#include ".\objspec_targettrigger.h"


IMPLEMENT_DYNAMIC(CObjSpec_TargetTrigger, i3GameObjPropertyDialog)

CObjSpec_TargetTrigger::CObjSpec_TargetTrigger(CWnd* pParent /*=nullptr*/)
	: i3GameObjPropertyDialog( CObjSpec_TargetTrigger::IDD, pParent)
{
}

CObjSpec_TargetTrigger::~CObjSpec_TargetTrigger()
{
}

void CObjSpec_TargetTrigger::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_CB_TEAM, m_CB_Team);
	DDX_Control( pDX, IDC_OBJ_LIST, m_ObjListCtrl);
	DDX_Control( pDX, IDC_BTN_SELTARGET, m_BTN_SelTarget);
}


BEGIN_MESSAGE_MAP(CObjSpec_TargetTrigger, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_SELTARGET, OnBnClickedBtnSeltarget)
	ON_CBN_SELCHANGE(IDC_CB_TEAM, OnCbnSelchangeCbTeam)
	ON_NOTIFY(NM_DBLCLK, IDC_OBJ_LIST, OnNMDblclkObjList)
END_MESSAGE_MAP()

void CObjSpec_TargetTrigger::_UpdateTarget( INT32 idx, i3GameObj * pObj)
{
	m_ObjListCtrl.SetItemData( idx, (DWORD_PTR) pObj);

	// Name
}

void CObjSpec_TargetTrigger::_UpdateAllTarget(void)
{
	INT32 i, idx;
	char conv[256];
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	TargetTrigger * pTrig = (TargetTrigger *) pElm->CreateGameInstance();

	m_ObjListCtrl.DeleteAllItems();

	for( i = 0; i < pTrig->getTargetCount(); i++)
	{
		i3GameObj * pObj = pTrig->getTarget( i);

		if( pObj->hasName())
			i3::safe_string_copy( conv, pObj->GetName(), 256 );
		else
			sprintf_s( conv, "<Nonamed>");

		idx = m_ObjListCtrl.InsertItem( 0, conv, 0);

		_UpdateTarget( idx, pObj);
	}

	I3_MUST_RELEASE(pTrig);
}

// CObjSpec_TargetTrigger message handlers
void CObjSpec_TargetTrigger::SetObject( i3ElementBase * pObj)
{
	i3GameObjPropertyDialog::SetObject( pObj);

	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	TargetTrigger * pTrig = (TargetTrigger *) pElm->CreateGameInstance();
	//char conv[128];

	// Team
	{
		switch( pTrig->getTeam())
		{
			case TEAM_RED :		m_CB_Team.SetCurSel( 0);	break;
			case TEAM_BLUE :	m_CB_Team.SetCurSel( 1);	break;
			default :			m_CB_Team.SetCurSel( 2);	break;
		}
	}

	// Targets
	_UpdateAllTarget();

	I3_MUST_RELEASE(pTrig);
}

BOOL CObjSpec_TargetTrigger::OnInitDialog()
{
	i3GameObjPropertyDialog::OnInitDialog();

	{
		m_ObjListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 120);
		m_ObjListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	{
		m_BTN_SelTarget.SetButtonStyle( m_BTN_SelTarget.GetButtonStyle() | BS_PUSHLIKE);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3LevelElement3D::static_meta(), I3_TDK_UPDATE_EDIT);

	return true;  // return true unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return false
}

void CObjSpec_TargetTrigger::OnSize(UINT nType, int cx, int cy)
{
	i3GameObjPropertyDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ObjListCtrl.m_hWnd))
	{
		CRect rt;

		m_ObjListCtrl.GetWindowRect( &rt);

		ScreenToClient( &rt);

		m_ObjListCtrl.SetWindowPos( nullptr, 0, 0, cx - rt.left - 3, cy - rt.top - 3, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}
}

int CObjSpec_TargetTrigger::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3GameObjPropertyDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CObjSpec_TargetTrigger::OnOK()
{
}

void CObjSpec_TargetTrigger::OnCancel()
{
}

void CObjSpec_TargetTrigger::OnBnClickedBtnSeltarget()
{
	i3ViewerFramework * pFramework = (i3ViewerFramework *) i3Framework::getCurrentFramework();

	if(!i3::kind_of<i3ViewerFramework*>(pFramework))
		return;

	pFramework->SelectTarget( m_hWnd);

	m_BTN_SelTarget.SetCheck( BST_CHECKED);
}

void CObjSpec_TargetTrigger::_EndSelectTarget(void)
{
	i3ViewerFramework * pFramework = (i3ViewerFramework *) i3Framework::getCurrentFramework();

	if(!i3::kind_of<i3ViewerFramework*>(pFramework))
		return;

	INT32 i;
	i3GameObj * pObj;
	i3LevelElement3D * pTargetElm;
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	TargetTrigger * pTrig = (TargetTrigger *) pElm->CreateGameInstance();

	pTrig->removeAllTarget();

	for( i = 0; i < pElm->getTargetCount(); i++)
	{
		pTargetElm = (i3LevelElement3D *) pElm->getTarget( i);

		pObj = pTargetElm->CreateGameInstance();

		if( pObj != nullptr)
		{
			I3_MUST_RELEASE(pObj);

			if( !i3::same_of<TargetObject*>(pObj))
			{
				NotifyBox( m_hWnd, "%s °´Ã¼´Â TargetObject Class°¡ ¾Æ´Õ´Ï´Ù.", pObj->GetName());
			}
			else
			{
				pTrig->addTarget( (TargetObject *)pObj);
			}
		}
	}

	_UpdateAllTarget();

	I3_MUST_RELEASE(pTrig);
}

void CObjSpec_TargetTrigger::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( getObject() == nullptr)
		return;

	if((pInfo->m_Event & I3_TDK_UPDATE_EDIT) && (pInfo->m_pObject == getObject()))
	{
		_EndSelectTarget();
	}
}

LRESULT CObjSpec_TargetTrigger::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3GameObjPropertyDialog::WindowProc(message, wParam, lParam);
}

void CObjSpec_TargetTrigger::OnCbnSelchangeCbTeam()
{
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	TargetTrigger * pTrig = (TargetTrigger *) pElm->CreateGameInstance();

	switch( m_CB_Team.GetCurSel())
	{
		case 0 :		pTrig->setTeam( TEAM_RED);	break;
		case 1 :		pTrig->setTeam( TEAM_BLUE);	break;
		default :		pTrig->setTeam( (TEAM_TYPE) 2);	break;
	}

	I3_MUST_RELEASE(pTrig);
}

void CObjSpec_TargetTrigger::OnNMDblclkObjList(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}

#endif
