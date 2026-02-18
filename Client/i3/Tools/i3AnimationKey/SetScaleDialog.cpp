// SetScaleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3AnimationKey.h"
#include "SetScaleDialog.h"
#include ".\setscaledialog.h"
#include "i3Base/string/ext/ftoa.h"

// CSetScaleDialog dialog

IMPLEMENT_DYNAMIC(CSetScaleDialog, CDialog)
CSetScaleDialog::CSetScaleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetScaleDialog::IDD, pParent)
{
}

CSetScaleDialog::~CSetScaleDialog()
{
}

void CSetScaleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_X, m_XCtrl);
	DDX_Control(pDX, IDC_ED_X2, m_YCtrl);
	DDX_Control(pDX, IDC_ED_X3, m_ZCtrl);
}

bool CSetScaleDialog::Execute( VEC3D * pScale)
{
	i3Vector::Copy( &m_Scale, pScale);

	if( DoModal() == IDCANCEL)
		return false;

	return true;
}


BEGIN_MESSAGE_MAP(CSetScaleDialog, CDialog)
END_MESSAGE_MAP()


// CSetScaleDialog message handlers

BOOL CSetScaleDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		char conv[128];

		i3::ftoa( i3Vector::GetX( &m_Scale), conv);
		m_XCtrl.SetWindowText( conv);

		i3::ftoa( i3Vector::GetY( &m_Scale), conv);
		m_YCtrl.SetWindowText( conv);

		i3::ftoa( i3Vector::GetZ( &m_Scale), conv);
		m_ZCtrl.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetScaleDialog::OnOK()
{
	char conv[128];
	REAL32 x, y, z;

	m_XCtrl.GetWindowText( conv, sizeof(conv) - 1);
	x = (REAL32) atof( conv);

	m_YCtrl.GetWindowText( conv, sizeof(conv) - 1);
	y = (REAL32) atof( conv);

	m_ZCtrl.GetWindowText( conv, sizeof(conv) - 1);
	z = (REAL32) atof( conv);

	i3Vector::Set( &m_Scale, x, y, z);

	CDialog::OnOK();
}
