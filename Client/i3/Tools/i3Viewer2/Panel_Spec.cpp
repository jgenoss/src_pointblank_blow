// Panel_Spec.cpp : implementation file
//

#include "stdafx.h"
#include "i3Viewer2.h"
#include "Panel_Spec.h"
#include "i3TDKSpec_Transform.h"
#include "i3TDKSpec_Node.h"
#include "GlobalVar.h"

// Panel_Spec dialog

IMPLEMENT_DYNAMIC(Panel_Spec, i3TDKSpecDialogContainer)

Panel_Spec::Panel_Spec(CWnd* pParent /*=NULL*/)
	: i3TDKSpecDialogContainer(Panel_Spec::IDD, pParent), m_pSpec_Body(NULL)
{
}

Panel_Spec::~Panel_Spec()
{
}

void Panel_Spec::DoDataExchange(CDataExchange* pDX)
{
	i3TDKSpecDialogContainer::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Panel_Spec, i3TDKSpecDialogContainer)
END_MESSAGE_MAP()


// Panel_Spec message handlers

BOOL Panel_Spec::OnInitDialog()
{
	i3TDKSpecDialogContainer::OnInitDialog();

	i3TDK::SetResInstance();

	AddBuiltInSpecDialogs();

	i3TDK::RestoreResInstance();

	i3::vector<I3TDK_SPECDLG_INFO*> list;
	i3TDKSpecDialogContainer::FindSpecDialog( i3Transform::static_meta(), I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, list);

	for( UINT32 i = 0; i < list.size(); i++)
	{
		I3TDK_SPECDLG_INFO * pInfo = list.at(i);
		if( i3::same_of<i3Transform*>(pInfo->m_pMeta))
		{
			//i3TDKSpec_Transform * pTrans = static_cast<i3TDKSpec_Transform*>(pInfo->m_pDlg);
			//pTrans->m_pCB_setTransPos = (TDK_CB_TRANSFORM_POS)getTransViewPosFunc();
		}
		else if( i3::kind_of<i3Node*>(pInfo->m_pMeta))
		{
			i3TDKSpec_Node * pNode = static_cast<i3TDKSpec_Node*>(pInfo->m_pDlg);
			if(pNode)
			{
				pNode->m_pCB_ResetBoundMat = (TDK_CB_BOUND_MAT)getResetBoundMatFunc();
			}
		}
	}

	i3::vector<I3TDK_SPECDLG_INFO*> Dialoglist;
	i3TDKSpecDialogContainer::FindSpecDialog(i3Body::static_meta(),  I3_TDK_UPDATE_SELECT | I3_TDK_UPDATE_POPUP_SPEC, Dialoglist);

	for( UINT32 i = 0; i < Dialoglist.size(); i++)
	{
		I3TDK_SPECDLG_INFO * pInfo = Dialoglist.at(i);
		if( i3::same_of<i3Body*>(pInfo->m_pMeta))
		{
			m_pSpec_Body = static_cast<i3GameObjPropertyDialog*>(pInfo->m_pDlg);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Panel_Spec::OnOK()
{
}

void Panel_Spec::OnCancel()
{
}
