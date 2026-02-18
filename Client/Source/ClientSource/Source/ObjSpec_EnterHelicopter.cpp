// Prop_LvRes_SceneGraph.cpp : implementation file
//

#include "pch.h"
#if defined(USE_EDITDLG)// CObjSpec_EnterHelicopter dialog
#include "ObjSpec_EnterHelicopter.h"
#include "GameObject.h"
#include ".\objspec_enterhelicopter.h"
#include "i3Level/i3LevelElement3D.h"

IMPLEMENT_DYNAMIC(CObjSpec_EnterHelicopter, i3GameObjPropertyDialog)

CObjSpec_EnterHelicopter::CObjSpec_EnterHelicopter(CWnd* pParent /*=nullptr*/)
	: i3GameObjPropertyDialog( CObjSpec_EnterHelicopter::IDD, pParent)
{
}

CObjSpec_EnterHelicopter::~CObjSpec_EnterHelicopter()
{
}

void CObjSpec_EnterHelicopter::DoDataExchange(CDataExchange* pDX)
{
	i3GameObjPropertyDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_LINKOBJ_LIST, m_ObjListCtrl);
	DDX_Control( pDX, IDC_BTN_SEL_LINKOBJ, m_BTN_SelTarget);
}


BEGIN_MESSAGE_MAP(CObjSpec_EnterHelicopter, i3GameObjPropertyDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_SEL_LINKOBJ, OnBnClickedBtnSeltarget)
	ON_NOTIFY(NM_DBLCLK, IDC_LINKOBJ_LIST, OnNMDblclkObjList)
END_MESSAGE_MAP()

void CObjSpec_EnterHelicopter::_UpdateLinkObj( INT32 idx, i3GameObj * pObj)
{
	m_ObjListCtrl.SetItemData( idx, (DWORD_PTR) pObj);

	// Name
}

void CObjSpec_EnterHelicopter::_UpdateAllLinkObj(void)
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

		_UpdateLinkObj( idx, pObj);
	}

	I3_MUST_RELEASE(pTrig);
}

// CObjSpec_EnterHelicopter message handlers
void CObjSpec_EnterHelicopter::SetObject( i3ElementBase * pObj)
{
	i3GameObjPropertyDialog::SetObject( pObj);
	
	// Objects
	_UpdateAllLinkObj();
}

BOOL CObjSpec_EnterHelicopter::OnInitDialog()
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

void CObjSpec_EnterHelicopter::OnSize(UINT nType, int cx, int cy)
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

int CObjSpec_EnterHelicopter::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3GameObjPropertyDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CObjSpec_EnterHelicopter::OnOK()
{
}

void CObjSpec_EnterHelicopter::OnCancel()
{
}

void CObjSpec_EnterHelicopter::OnBnClickedBtnSeltarget()
{
	i3ViewerFramework * pFramework = (i3ViewerFramework *) i3Framework::getCurrentFramework();

	if(!i3::kind_of<i3ViewerFramework*>(pFramework))
		return;

	pFramework->SelectTarget( m_hWnd);

	m_BTN_SelTarget.SetCheck( BST_CHECKED);
}

void CObjSpec_EnterHelicopter::_EndSelectTarget(void)
{
	i3ViewerFramework * pFramework = (i3ViewerFramework *) i3Framework::getCurrentFramework();

	if(!i3::kind_of<i3ViewerFramework*>(pFramework))
		return;

	INT32 i;
	i3GameObj * pObj;
	i3LevelElement3D * pTargetElm;
	i3LevelElement3D * pElm = (i3LevelElement3D *) getObject();
	CLinkObjTrigger * pTrig = (CLinkObjTrigger *) pElm->CreateGameInstance();

	pTrig->removeAllLinkObj();

	for( i = 0; i < pElm->getTargetCount(); i++)
	{
		pTargetElm = (i3LevelElement3D *) pElm->getTarget( i);

		pObj = pTargetElm->CreateGameInstance();

		if( pObj != nullptr)
		{
			if( !i3::kind_of<i3Object*>(pObj))
			{
				NotifyBox( m_hWnd, "%s °´Ã¼´Â Object Class°¡ ¾Æ´Õ´Ï´Ù.", pObj->GetName());
			}
			else
			{
				pTrig->addLinkObj( (i3Object *)pObj);
			}

			I3_MUST_RELEASE(pObj);
		}
	}

	I3_MUST_RELEASE(pTrig);

	_UpdateAllLinkObj();
}

void CObjSpec_EnterHelicopter::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( getObject() == nullptr)
		return;

	if((pInfo->m_Event & I3_TDK_UPDATE_EDIT) && (pInfo->m_pObject == getObject()))
	{
		_EndSelectTarget();
	}
}

LRESULT CObjSpec_EnterHelicopter::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3GameObjPropertyDialog::WindowProc(message, wParam, lParam);
}

void CObjSpec_EnterHelicopter::OnNMDblclkObjList(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}

#endif
