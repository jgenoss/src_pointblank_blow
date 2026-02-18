// PaneEnvironment.cpp : implementation file
//

#include "stdafx.h"
#include "i3ClothSim.h"
#include "GlobalVariable.h"
#include "PaneEnvironment.h"
#include ".\paneenvironment.h"


// CPaneEnvironment dialog

IMPLEMENT_DYNAMIC(CPaneEnvironment, i3TDKDialogBase)
CPaneEnvironment::CPaneEnvironment(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneEnvironment::IDD, pParent)
{
	m_pCloth = NULL;
}

CPaneEnvironment::~CPaneEnvironment()
{
}

void CPaneEnvironment::DoDataExchange( CDataExchange* pDX )
{
	i3TDKDialogBase::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_ED_SIM_WINDF, m_WindFactorCtrl );
	DDX_Control( pDX, IDC_ED_SIM_WINDFMIN, m_WindMinCtrl );
	DDX_Control( pDX, IDC_ED_SIM_WINDFMAX, m_WindMaxCtrl );

	DDX_Control( pDX, IDC_ED_SIM_GRAVITY, m_GravityCtrl );
	DDX_Check( pDX, IDC_CHK_SIM_GRAVITYAXIS, m_bGravityAxisY );
	DDX_Check( pDX, IDC_CHK_SIM_COLLISION, m_bCollisionTest );
	DDX_Check( pDX, IDC_CHK_SIM_IMPULSE, m_bImpulseTest );
}


BEGIN_MESSAGE_MAP(CPaneEnvironment, i3TDKDialogBase)
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_ED_SIM_GRAVITY, OnEnChangeEdSimGravity)
END_MESSAGE_MAP()


// CPaneEnvironment message handlers

int CPaneEnvironment::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText( "Environments");

	setCloth( g_pFramework->getCloth() );

	return 0;
}

void CPaneEnvironment::OnEnChangeEdSimGravity()
{
	char conv[256];
	VEC3D GVec;

	i3Vector::Set( &GVec, 0.0f, 1.0f, 0.0f );

	m_GravityCtrl.GetWindowText( conv, sizeof( conv ) - 1 );
	if( m_bGravityAxisY )
		i3Vector::Set( &GVec, 0.0f, (REAL32)( atof(conv) ), 0.0f );
	else
		i3Vector::Scale( &GVec, &GVec, (REAL32)( atof(conv) ) );
	getCloth()->setGravity( &GVec );
}
