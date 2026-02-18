// SpecAI.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "SpecAI.h"
#include ".\specai.h"


// CSpecAI dialog

IMPLEMENT_DYNAMIC(CSpecAI, i3TDKDialogBase)
CSpecAI::CSpecAI(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CSpecAI::IDD, pParent)
{
}

CSpecAI::~CSpecAI()
{
}

void CSpecAI::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_NAME, m_NameCtrl);
	DDX_Control(pDX, IDC_CHK_ENABLE, m_EnableCtrl);
	DDX_Control(pDX, IDC_CB_INITSTATE, m_InitStateCtrl);
	DDX_Control(pDX, IDC_ED_DRIVERMETA, m_DriverMetaCtrl);
}

void CSpecAI::SetObject( i3ElementBase * pObj)
{
	INT32 i, selIdx = -1, idx;
	AI * pAI = (AI *) pObj;
	i3AIState * pState;

	I3ASSERT( i3::kind_of<AI*>(pObj)); //->IsTypeOf( AI::static_meta()));
	
	I3_REF_CHANGE(m_pObject, pObj);
	m_InitStateCtrl.ResetContent();

	if( pAI->getAIContext() != NULL)
	{
		// Name
		if( pAI->hasName())
			m_NameCtrl.SetWindowText( pAI->GetName());
		else
			m_NameCtrl.SetWindowText( "");

		// Enabled State
		if( pAI->getAIContext()->isEnable() == false)
			m_EnableCtrl.SetCheck( BST_UNCHECKED);
		else
			m_EnableCtrl.SetCheck( BST_CHECKED);

		// Initial State
		{
			m_InitStateCtrl.InsertString( 0, "<NONE>");

			for( i = 0; i < pAI->getAI()->getAIStateCount(); i++)
			{
				pState = pAI->getAI()->getAIState( i);

				idx = m_InitStateCtrl.AddString( pState->GetName());
				m_InitStateCtrl.SetItemData( idx, (DWORD_PTR) pState);
			}
		}

		if( pAI->getAIContext()->getInitialAIState() == NULL)
		{
			selIdx = 0;
		}
		else
		{
			for( i = 0; i < m_InitStateCtrl.GetCount(); i++)
			{
				pState = (i3AIState *) m_InitStateCtrl.GetItemData( i);

				if( pState == pAI->getAIContext()->getInitialAIState())
				{
					selIdx = i;
				}
			}
		}

		m_InitStateCtrl.SetCurSel( selIdx);

		m_DriverMetaCtrl.SetWindowText( pAI->getDriverName());
	}
	else
	{
		m_NameCtrl.SetWindowText( "");
		m_EnableCtrl.SetCheck( BST_UNCHECKED);
		m_InitStateCtrl.ResetContent();
		m_DriverMetaCtrl.SetWindowText( "");
	}
}

BEGIN_MESSAGE_MAP(CSpecAI, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_CHK_ENABLE, OnBnClickedChkEnable)
	ON_CBN_SELCHANGE(IDC_CB_INITSTATE, OnCbnSelchangeCbInitstate)
	ON_EN_KILLFOCUS(IDC_ED_DRIVERMETA, OnEnKillfocusEdDrivermeta)
END_MESSAGE_MAP()


// CSpecAI message handlers

BOOL CSpecAI::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSpecAI::OnCancel()
{
}

void CSpecAI::OnOK()
{
	AI * pAAI = (AI *) getObject();
	i3AI * pAI = (i3AI *) pAAI->getAI();
	char conv[256];

	if( pAI == NULL)
		return;

	m_NameCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pAI->SetName( conv);
	pAAI->SetName( conv);
	if( pAAI->getAIContext() != NULL)
		pAAI->getAIContext()->SetName( conv);

	m_DriverMetaCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pAAI->setDriverName( conv);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pAAI);
}

void CSpecAI::OnBnClickedChkEnable()
{
	AI * pAAI = (AI *) getObject();
	i3AIContext * pCtx = pAAI->getAIContext();

	if( m_EnableCtrl.GetCheck() == BST_CHECKED)
		pCtx->removeGNodeState( I3_GAMENODE_STATE_DISABLE);
	else
		pCtx->addGNodeState( I3_GAMENODE_STATE_DISABLE);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pAAI);
}

void CSpecAI::OnCbnSelchangeCbInitstate()
{
	AI * pAAI = (AI *) getObject();
	i3AIContext * pCtx = pAAI->getAIContext();
	INT32 idx;

	idx = m_InitStateCtrl.GetCurSel();

	if( idx != -1)
	{
		i3AIState * pState;

		pState = (i3AIState *) m_InitStateCtrl.GetItemData( idx);

		pCtx->setInitialAIState( pState);
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pAAI);
}

void CSpecAI::OnEnKillfocusEdDrivermeta()
{
	char conv[256];
	AI * pAAI = (AI *) getObject();

	m_DriverMetaCtrl.GetWindowText( conv, sizeof(conv) - 1);
	pAAI->setDriverName( conv);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, pAAI);
}
