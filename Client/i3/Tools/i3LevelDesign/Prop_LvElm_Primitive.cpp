// Prop_LvElm_Primitive.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvElm_Primitive.h"
#include "DlgTextureSelect.h"
#include "DlgActionSelect.h"
#include ".\prop_lvelm_primitive.h"


// CProp_LvElm_Primitive dialog

IMPLEMENT_DYNAMIC(CProp_LvElm_Primitive, i3TDKDialogBase)
CProp_LvElm_Primitive::CProp_LvElm_Primitive(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvElm_Primitive::IDD, pParent)
{
}

CProp_LvElm_Primitive::~CProp_LvElm_Primitive()
{
}

void CProp_LvElm_Primitive::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_VISIBLE, m_VisibleCtrl);
	DDX_Control(pDX, IDC_CHK_TRIGGER, m_TriggerCtrl);
	DDX_Control(pDX, IDC_ED_TEXTURE, m_TextureCtrl);
	DDX_Control(pDX, IDC_ED_UTILE, m_UTileCtrl);
	DDX_Control(pDX, IDC_ED_VTILE, m_VTileCtrl);
	DDX_Control(pDX, IDC_COLOR_PREVIEW_PLACEHOLDER, m_ST_ColorPreviewPH);
	DDX_Control(pDX, IDC_CB_TRIGGER_CGROUP, m_CB_TriggerCGroup);
}

void CProp_LvElm_Primitive::SetObject( i3ElementBase * pObj)
{
	char conv[128];

	if( !i3::kind_of<i3LevelPrimitive*>(pObj)) //->IsTypeOf( i3LevelPrimitive::static_meta()) == FALSE)
		return;

	i3TDKDialogBase::SetObject( pObj);

	i3LevelPrimitive * pPrim = (i3LevelPrimitive *) pObj;

	// Visible
	if( pPrim->isVisible())
		m_VisibleCtrl.SetCheck( BST_CHECKED);
	else
		m_VisibleCtrl.SetCheck( BST_UNCHECKED);

	// Texture
	if( pPrim->getTextureRes() != NULL)
	{
		i3LevelResTexture * pTexRes = pPrim->getTextureRes();

		m_TextureCtrl.SetWindowText( pTexRes->GetName());

		// U Tiling
		sprintf( conv, "%d", pPrim->getHorzTiling());
		m_UTileCtrl.SetWindowText( conv);
	
		// V Tiling
		sprintf( conv, "%d", pPrim->getVertTiling());
		m_VTileCtrl.SetWindowText( conv);
	}
	else
	{
		m_TextureCtrl.SetWindowText( "");
		m_UTileCtrl.SetWindowText( "");
		m_VTileCtrl.SetWindowText( "");
	}

	// Color
	{
		m_ColorCtrl.setColor( pPrim->getMaterialColor());
	}

	// Trigger
	if( pPrim->isTrigger() && (pPrim->getEventRes() != NULL))
	{
		i3TimeEventGen * pEventGen = pPrim->getEventRes()->getTimeEvent();

		if( pEventGen != NULL)
		{
			pEventGen->SetName( pPrim->getEventRes()->GetName());
		}

		m_TimeEventCtrl.SetTimeSequence( pEventGen);

		m_TriggerCtrl.SetCheck( BST_CHECKED);
		m_TimeEventCtrl.ShowWindow( SW_SHOW);

		// Trigger Collision Group
		m_CB_TriggerCGroup.EnableWindow( TRUE);
	}
	else
	{
		m_TimeEventCtrl.ShowWindow( SW_HIDE);
		m_TriggerCtrl.SetCheck( BST_UNCHECKED);

		m_CB_TriggerCGroup.EnableWindow( FALSE);
	}

	// Trigger Collision Group
	{
		switch( pPrim->getTriggerCollisionGroup())
		{
			case 30 :
				m_CB_TriggerCGroup.SetCurSel( 1);
				break;

			default :
				m_CB_TriggerCGroup.SetCurSel( 0);
				break;
		}
	}
}

BEGIN_MESSAGE_MAP(CProp_LvElm_Primitive, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_BTN_TEXTURE, OnBnClickedBtnTexture)
	ON_BN_CLICKED(IDC_CHK_TRIGGER, OnBnClickedChkTrigger)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHK_VISIBLE, OnBnClickedChkVisible)
ON_CBN_SELCHANGE(IDC_CB_TRIGGER_CGROUP, OnCbnSelchangeCbTriggerCgroup)
END_MESSAGE_MAP()


// CProp_LvElm_Primitive message handlers

BOOL CProp_LvElm_Primitive::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rect;

		m_ST_ColorPreviewPH.GetWindowRect( &rect);

		ScreenToClient( &rect);

		m_ColorCtrl.Create( WS_VISIBLE | WS_CHILD, rect, this, 100);

		GetClientRect( &rect);

		rect.top += 150;
		m_TimeEventCtrl.Create( WS_VISIBLE | WS_CHILD, rect , this, 101);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvElm_Primitive::OnOK()
{

}

void CProp_LvElm_Primitive::OnCancel()
{
}

void CProp_LvElm_Primitive::OnBnClickedBtnTexture()
{
	CDlgTextureSelect dlg;
	i3LevelPrimitive * pPrim = (i3LevelPrimitive *) getObject();

	if( dlg.Execute( pPrim->getTextureRes()))
	{
		i3LevelResTexture * pNewTex = dlg.getSelectedTexture();
		i3LevelResTexture * pCurTex = pPrim->getTextureRes();

		if( pNewTex != pCurTex)
		{
			i3LevelUndo::ADD_Edit( pPrim);

			m_TextureCtrl.SetWindowText( pNewTex->GetName());

			pPrim->setTextureRes( pNewTex);
		}
	}
}

LRESULT CProp_LvElm_Primitive::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_SETCOLOR)
	{
		i3LevelPrimitive * pPrim = (i3LevelPrimitive *) getObject();

		pPrim->setMaterialColor( m_ColorCtrl.getColor());
	}
	else if( message == WM_USER_TIME_EVENT_EDITED)
	{
		i3LevelPrimitive * pPrim = (i3LevelPrimitive *) getObject();

		if( pPrim->getEventRes() != NULL)
		{
			// 내부에서 Action이 변경되는 등의 변경이 발생했으므로 저장해야 한다.
			i3LevelRes * pRes = pPrim->getEventRes();

			pRes->addResState( I3LV_RES_STATE_MODIFIED);
		}
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CProp_LvElm_Primitive::OnBnClickedChkTrigger()
{
	// TODO: Add your control notification handler code here
	i3LevelPrimitive * pPrim = (i3LevelPrimitive *) m_pObject;
	if( pPrim == NULL)
		return;

	bool isTrigger = pPrim->isTrigger();
	bool bTrigger = (bool)(m_TriggerCtrl.GetCheck() == BST_CHECKED);

	if( isTrigger != bTrigger)
	{
		i3LevelUndo::ADD_Edit( pPrim);
	}

	pPrim->setTrigger( bTrigger);

	if( pPrim->isTrigger())
	{
		CDlgActionSelect dlg;

		m_CB_TriggerCGroup.EnableWindow( TRUE);

		if( dlg.Execute( pPrim->getEventRes()))
		{
			i3LevelResEvent * pSelEvent = dlg.getSelectedEvent();
			if( pSelEvent != NULL)
			{
				//validate가 되지 않은 리소스라면 validate한다
				if( !pSelEvent->isResState( I3LV_RES_STATE_LOADED))
				{
					pSelEvent->OnValidate( g_pScene, true);
				}

				if(pSelEvent->getTimeEvent() != NULL)
				{
					pPrim->setEventRes( pSelEvent);

					m_TimeEventCtrl.SetTimeSequence( pSelEvent->getTimeEvent());
					m_TimeEventCtrl.ShowWindow( SW_SHOW);
					return;
				}
			}
		}
	}
	else
	{
		m_TimeEventCtrl.SetTimeSequence( NULL);
		m_TimeEventCtrl.ShowWindow( SW_HIDE);

		m_CB_TriggerCGroup.EnableWindow( FALSE);
	}

	pPrim->setEventRes( NULL);
	pPrim->setTrigger( false);
	m_TriggerCtrl.SetCheck( BST_UNCHECKED);
}

void CProp_LvElm_Primitive::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_TimeEventCtrl.m_hWnd))
	{
		CRect rt;

		m_CB_TriggerCGroup.GetWindowRect( &rt);

		ScreenToClient( &rt);

		rt.left = 4;
		rt.top = rt.bottom + 8;
		rt.right = cx - 8;
		rt.bottom = cy - rt.top - 4;

		m_TimeEventCtrl.SetWindowPos( NULL, rt.left, rt.top, rt.right, rt.bottom, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CProp_LvElm_Primitive::OnBnClickedChkVisible()
{
	// TODO: Add your control notification handler code here
	i3LevelPrimitive * pPrim = (i3LevelPrimitive *) m_pObject;
	if( pPrim == NULL)
		return;

	bool isVisible = pPrim->isVisible();
	bool bVisible = (bool)(m_VisibleCtrl.GetCheck() == BST_CHECKED);

	if( isVisible != bVisible)
	{
		i3LevelUndo::ADD_Edit( pPrim);

		pPrim->setVisible( bVisible);
	}
}

void CProp_LvElm_Primitive::OnCbnSelchangeCbTriggerCgroup()
{
	i3LevelPrimitive * pPrim = (i3LevelPrimitive *) m_pObject;

	INT32 nCurColGroup = pPrim->getTriggerCollisionGroup();
	INT32 nColGroup = -1;

	switch( m_CB_TriggerCGroup.GetCurSel())
	{
		case 1 :	nColGroup = 30;		break;
		default :	nColGroup = 25;		break;
	}

	if( nCurColGroup != nColGroup)
	{
		i3LevelUndo::ADD_Edit( pPrim);

		pPrim->setTriggerCollisionGroup( nColGroup);
	}
}
