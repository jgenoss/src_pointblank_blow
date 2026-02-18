// SetNodeFlagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "SetNodeFlagDlg.h"
#include ".\setnodeflagdlg.h"


// CSetNodeFlagDlg dialog

IMPLEMENT_DYNAMIC(CSetNodeFlagDlg, CDialog)
CSetNodeFlagDlg::CSetNodeFlagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetNodeFlagDlg::IDD, pParent)
{ 
}

CSetNodeFlagDlg::~CSetNodeFlagDlg()
{
}

void CSetNodeFlagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_NODE_DISABLE, m_bDisable);
	DDX_Control(pDX, IDC_CHECK_NODE_INVISIBLE, m_bInvisible);
	DDX_Control(pDX, IDC_CHECK_NODE_COLLIDABLE, m_bCollidable);
	DDX_Control(pDX, IDC_CHECK_NODE_TRANSFORM, m_bTransform);
	DDX_Control(pDX, IDC_CHECK_NODE_TRANSPARENT, m_bTransparent);
	DDX_Control(pDX, IDC_CHECK_NODE_STATIC, m_bStatic);
	DDX_Control(pDX, IDC_CHECK_NODE_DELETEORIGINAL, m_bDeleteOriginalNode);
	DDX_Control(pDX, IDC_CHECK_NODE_USER, m_bUser);
	DDX_Control(pDX, IDC_CHK_CONTROL, m_ControlCtrl);
	DDX_Control(pDX, IDC_CHK_VOLATILE, m_CHK_Volatile);
	DDX_Control(pDX, IDC_CHK_MANUAL_UPDATE, m_CHK_ManualUpdate);
	DDX_Control(pDX, IDC_CHK_SHADOW_CASTER, m_CHK_DisableShadowCaster);
	DDX_Control(pDX, IDC_CHK_SHADOW_RECEIVER, m_CHK_DisableShadowReceiver);
	DDX_Control(pDX, IDC_CHK_ATTACH_POINT, m_CHK_AttachPoint);
}


BEGIN_MESSAGE_MAP(CSetNodeFlagDlg, CDialog)
END_MESSAGE_MAP()

BOOL CSetNodeFlagDlg::OnInitDialog()
{
	/*
	BST_UNCHECKED : BST_CHECKED
	//*/
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	if(m_nFlag & I3_NODEFLAG_DISABLE)					m_bDisable.SetCheck(BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_INVISIBLE)					m_bInvisible.SetCheck(BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_TRANSFORM)					m_bTransform.SetCheck(BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_TRANSPARENT)				m_bTransparent.SetCheck(BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_STATIC)					m_bStatic.SetCheck(BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_DELETEORIGINALNODE)		m_bDeleteOriginalNode.SetCheck(BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_USER)						m_bUser.SetCheck(BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_CONTROL)					m_ControlCtrl.SetCheck( BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_VOLATILE)					m_CHK_Volatile.SetCheck( BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_MANUAL_UPDATE)				m_CHK_ManualUpdate.SetCheck( BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_DISABLE_SHADOW_CASTER)		m_CHK_DisableShadowCaster.SetCheck( BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_DISABLE_SHADOW_RECEIVER)	m_CHK_DisableShadowReceiver.SetCheck( BST_CHECKED);
	if(m_nFlag & I3_NODEFLAG_ATTACH_POINT)				m_CHK_AttachPoint.SetCheck( BST_CHECKED);
	//*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CSetNodeFlagDlg::Update(void)
{
	m_nFlag = 0x0;

	if(m_bDisable.GetCheck() == BST_CHECKED)					m_nFlag |= I3_NODEFLAG_DISABLE;
	if(m_bInvisible.GetCheck() == BST_CHECKED)					m_nFlag |= I3_NODEFLAG_INVISIBLE;
	if(m_bTransform.GetCheck() == BST_CHECKED)					m_nFlag |= I3_NODEFLAG_TRANSFORM;
	if(m_bTransparent.GetCheck() == BST_CHECKED)				m_nFlag |= I3_NODEFLAG_TRANSPARENT;
	if(m_bStatic.GetCheck() == BST_CHECKED)						m_nFlag |= I3_NODEFLAG_STATIC;
	if(m_bDeleteOriginalNode.GetCheck() == BST_CHECKED)			m_nFlag |= I3_NODEFLAG_DELETEORIGINALNODE;
	if(m_bUser.GetCheck() == BST_CHECKED)						m_nFlag |= I3_NODEFLAG_USER;
	if(m_ControlCtrl.GetCheck() == BST_CHECKED)					m_nFlag |= I3_NODEFLAG_CONTROL;
	if(m_CHK_Volatile.GetCheck() == BST_CHECKED)				m_nFlag |= I3_NODEFLAG_VOLATILE;
	if(m_CHK_ManualUpdate.GetCheck() == BST_CHECKED)			m_nFlag |= I3_NODEFLAG_MANUAL_UPDATE;
	if(m_CHK_DisableShadowCaster.GetCheck() == BST_CHECKED)		m_nFlag |= I3_NODEFLAG_DISABLE_SHADOW_CASTER;
	if(m_CHK_DisableShadowReceiver.GetCheck() == BST_CHECKED)	m_nFlag |= I3_NODEFLAG_DISABLE_SHADOW_RECEIVER;
	if(m_CHK_AttachPoint.GetCheck() == BST_CHECKED)				m_nFlag |= I3_NODEFLAG_ATTACH_POINT;

	return false;
}

void CSetNodeFlagDlg::OnOK()
{
	Update();
	CDialog::OnOK();
}

void CSetNodeFlagDlg::OnCancel()
{
	CDialog::OnCancel();
}
