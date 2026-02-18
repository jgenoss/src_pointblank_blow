// i3TDKDlg_NodeFlag.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlg_NodeFlag.h"


// i3TDKDlg_NodeFlag dialog

IMPLEMENT_DYNAMIC(i3TDKDlg_NodeFlag, CDialog)

bool i3TDKDlg_NodeFlag::Execute( i3Node * pNode)
{
	m_pNode = pNode;

	i3TDK::SetResInstance();
	INT32 rv = DoModal();
	i3TDK::RestoreResInstance();

	if( rv == IDOK)
		return true;

	return false;
}

void i3TDKDlg_NodeFlag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CHK_DISABLE, m_Disable);
	DDX_Control(pDX, IDC_TDK_CHK_INVISIBLE, m_Invisible);
	DDX_Control(pDX, IDC_TDK_CHK_MUSTBEPROCESS, m_MustBeProcess);
	DDX_Control(pDX, IDC_TDK_CHK_TRANSPARENT, m_Transparent);
	DDX_Control(pDX, IDC_TDK_CHK_STATIC, m_Static);
	DDX_Control(pDX, IDC_TDK_CHK_VOLATILE, m_Volatile);
	DDX_Control(pDX, IDC_TDK_CHK_CONTROL, m_Control);
	DDX_Control(pDX, IDC_TDK_CHK_MANUALUPDATE, m_ManualUpdate);
	DDX_Control(pDX, IDC_TDK_CHK_DYNAMICUPDATE, m_Dynamic);
	DDX_Control(pDX, IDC_TDK_CHK_DISABLESHADOWCASTER, m_DisableShadowCasting);
	DDX_Control(pDX, IDC_TDK_CHK_DISABLESHADOWRECEIVE, m_DisableShadowReceive);
	DDX_Control(pDX, IDC_TDK_CHK_NOWORK, m_NoWork);
	DDX_Control(pDX, IDC_TDK_CHK_ATTACH_POINT, m_AttachPoint);
}


BEGIN_MESSAGE_MAP(i3TDKDlg_NodeFlag, CDialog)
END_MESSAGE_MAP()

void i3TDKDlg_NodeFlag::setCheck( INT32 flag, CButton * pBtn)
{
	if( m_pNode->IsFlag( flag))
		pBtn->SetCheck( BST_CHECKED);
	else
		pBtn->SetCheck( BST_UNCHECKED);
}

void i3TDKDlg_NodeFlag::getCheck( INT32 flag, CButton * pBtn)
{
	if( pBtn->GetCheck() == BST_CHECKED)
		m_pNode->AddFlag( flag);
	else
		m_pNode->RemoveFlag( flag);
}


// i3TDKDlg_NodeFlag message handlers

BOOL i3TDKDlg_NodeFlag::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_pNode != nullptr)
	{
		setCheck( I3_NODEFLAG_DISABLE,					& m_Disable);
		setCheck( I3_NODEFLAG_INVISIBLE,				& m_Invisible);
		setCheck( I3_NODEFLAG_MUSTBEPROCESS,			& m_MustBeProcess);
		setCheck( I3_NODEFLAG_TRANSPARENT,				& m_Transparent);
		setCheck( I3_NODEFLAG_STATIC,					& m_Static);
		setCheck( I3_NODEFLAG_VOLATILE,					& m_Volatile);
		setCheck( I3_NODEFLAG_CONTROL,					& m_Control);
		setCheck( I3_NODEFLAG_MANUAL_UPDATE,			& m_ManualUpdate);
		setCheck( I3_NODEFLAG_DYNAMIC_UPDATE,			& m_Dynamic);
		setCheck( I3_NODEFLAG_DISABLE_SHADOW_CASTER,	& m_DisableShadowCasting);
		setCheck( I3_NODEFLAG_DISABLE_SHADOW_RECEIVER,	& m_DisableShadowReceive);
		setCheck( I3_NODEFLAG_NO_WORK,					& m_NoWork);
		setCheck( I3_NODEFLAG_ATTACH_POINT,				& m_AttachPoint);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKDlg_NodeFlag::OnOK()
{
	if( m_pNode != nullptr)
	{
		getCheck( I3_NODEFLAG_DISABLE,					& m_Disable);
		getCheck( I3_NODEFLAG_INVISIBLE,				& m_Invisible);
		getCheck( I3_NODEFLAG_MUSTBEPROCESS,			& m_MustBeProcess);
		getCheck( I3_NODEFLAG_TRANSPARENT,				& m_Transparent);
		getCheck( I3_NODEFLAG_STATIC,					& m_Static);
		getCheck( I3_NODEFLAG_VOLATILE,					& m_Volatile);
		getCheck( I3_NODEFLAG_CONTROL,					& m_Control);
		getCheck( I3_NODEFLAG_MANUAL_UPDATE,			& m_ManualUpdate);
		getCheck( I3_NODEFLAG_DYNAMIC_UPDATE,			& m_Dynamic);
		getCheck( I3_NODEFLAG_DISABLE_SHADOW_CASTER,	& m_DisableShadowCasting);
		getCheck( I3_NODEFLAG_DISABLE_SHADOW_RECEIVER,	& m_DisableShadowReceive);
		getCheck( I3_NODEFLAG_NO_WORK,					& m_NoWork);
		getCheck( I3_NODEFLAG_ATTACH_POINT,				& m_AttachPoint);
	}

	CDialog::OnOK();
}
