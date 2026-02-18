// Prop_LvElm_PathLink.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "Prop_LvElm_PathLink.h"
#include ".\prop_lvelm_pathlink.h"
#include "i3Base/string/ext/ftoa.h"

// CProp_LvElm_PathLink dialog

IMPLEMENT_DYNAMIC(CProp_LvElm_PathLink, i3TDKDialogBase)
CProp_LvElm_PathLink::CProp_LvElm_PathLink(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CProp_LvElm_PathLink::IDD, pParent)
{
}

CProp_LvElm_PathLink::~CProp_LvElm_PathLink()
{
}

void CProp_LvElm_PathLink::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_BIDIR, m_CHK_BiDir);
	DDX_Control(pDX, IDC_ED_COST, m_ED_Cost);
}

void CProp_LvElm_PathLink::SetObject( i3ElementBase * pObj)
{
	char conv[256];

	if( pObj != NULL)
	{
		I3ASSERT( i3::kind_of<i3LevelPathLink*>(pObj)); //->IsTypeOf( i3LevelPathLink::static_meta()));

		m_pLink = (i3LevelPathLink *) pObj;

		// Bi-Directional
		if( m_pLink->getPathStyle() & i3PathLine::STYLE_BIDIRECTION)
			m_CHK_BiDir.SetCheck( BST_CHECKED);
		else
			m_CHK_BiDir.SetCheck( BST_UNCHECKED);

		// Cost
		i3::ftoa( m_pLink->getCost(), conv);
		m_ED_Cost.SetWindowText( conv);
	}
	else
	{
		m_pLink = NULL;
		m_CHK_BiDir.SetCheck( BST_UNCHECKED);
		m_ED_Cost.SetWindowText( "");
	}
}

BEGIN_MESSAGE_MAP(CProp_LvElm_PathLink, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_CHK_BIDIR, OnBnClickedChkBidir)
	ON_EN_KILLFOCUS(IDC_ED_COST, OnEnKillfocusEdCost)
END_MESSAGE_MAP()


// CProp_LvElm_PathLink message handlers

BOOL CProp_LvElm_PathLink::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProp_LvElm_PathLink::OnOK()
{
}

void CProp_LvElm_PathLink::OnCancel()
{
}

void CProp_LvElm_PathLink::OnBnClickedChkBidir()
{
	if( m_pLink == NULL)
		return;

	if( m_CHK_BiDir.GetCheck() == BST_CHECKED)
	{
		m_pLink->addPathStyle( i3PathLine::STYLE_BIDIRECTION);
	}
	else
	{
		m_pLink->removePathStyle( i3PathLine::STYLE_BIDIRECTION);
	}

	m_pLink->UpdateAll();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLink);
}

void CProp_LvElm_PathLink::OnEnKillfocusEdCost()
{
	if( m_pLink == NULL)
		return;

	char conv[256];

	m_ED_Cost.GetWindowText( conv, sizeof(conv) - 1);

	m_pLink->setCost( (REAL32) atof( conv));

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pLink);
}
