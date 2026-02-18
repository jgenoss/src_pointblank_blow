// i3TDKOptSpec_RemoveVertexComponent.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKOptSpec_RemoveVertexComponent.h"

IMPLEMENT_DYNAMIC(i3TDKOptSpec_RemoveVertexComponent, i3TDKOptSpecBase)

void i3TDKOptSpec_RemoveVertexComponent::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CHK_NORMAL, m_CHK_Normal);
	DDX_Control(pDX, IDC_TDK_CHK_COLOR, m_CHK_Color);
	DDX_Control(pDX, IDC_TDK_CHK_TEX, m_CHK_Tex);
	DDX_Control(pDX, IDC_TDK_CHK_TANGNET, m_CHK_Tangent);
	DDX_Control(pDX, IDC_TDK_CHK_ALL_TEX, m_CHK_AllTex);
}


BEGIN_MESSAGE_MAP(i3TDKOptSpec_RemoveVertexComponent, i3TDKOptSpecBase)
END_MESSAGE_MAP()


// i3TDKOptSpec_RemoveVertexComponent message handlers

BOOL i3TDKOptSpec_RemoveVertexComponent::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	if( m_pOpt != nullptr)
	{
		i3OptRemoveVertexComponent * pOpt = (i3OptRemoveVertexComponent *) m_pOpt;

		if( pOpt->getRemoveNormal())
			m_CHK_Normal.SetCheck( BST_CHECKED);
		else
			m_CHK_Normal.SetCheck( BST_UNCHECKED);

		if( pOpt->getRemoveColor())
			m_CHK_Color.SetCheck( BST_CHECKED);
		else
			m_CHK_Color.SetCheck( BST_UNCHECKED);

		if( pOpt->getRemoveTexcoord())
			m_CHK_Tex.SetCheck( BST_CHECKED);
		else
			m_CHK_Tex.SetCheck( BST_UNCHECKED);

		if( pOpt->getRemoveTangent())
			m_CHK_Tangent.SetCheck( BST_CHECKED);
		else
			m_CHK_Tangent.SetCheck( BST_UNCHECKED);

		if (pOpt->getRemoveAllTexcoord())
			m_CHK_AllTex.SetCheck(BST_CHECKED);
		else
			m_CHK_AllTex.SetCheck(BST_UNCHECKED);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpec_RemoveVertexComponent::OnOK()
{
	if( m_pOpt != nullptr)
	{
		i3OptRemoveVertexComponent * pOpt = (i3OptRemoveVertexComponent *) m_pOpt;

		pOpt->SetRemoveNormal( m_CHK_Normal.GetCheck() == BST_CHECKED);
		pOpt->SetRemoveColor( m_CHK_Color.GetCheck() == BST_CHECKED);
		pOpt->SetRemoveTexcoord( m_CHK_Tex.GetCheck() == BST_CHECKED);
		pOpt->SetRemoveTangent( m_CHK_Tangent.GetCheck() == BST_CHECKED);
		pOpt->SetRemoveAllTexcoord(m_CHK_AllTex.GetCheck() == BST_CHECKED);
	}

	i3TDKOptSpecBase::OnOK();
}
