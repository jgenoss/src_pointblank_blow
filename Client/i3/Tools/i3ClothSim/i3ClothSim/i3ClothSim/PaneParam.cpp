// PaneParam.cpp : implementation file
//

#include "stdafx.h"
#include "GlobalVariable.h"
#include "i3ClothSim.h"
#include "PaneParam.h"
#include ".\paneparam.h"


// CPaneParam dialog

IMPLEMENT_DYNAMIC(CPaneParam, i3TDKDialogBase)
CPaneParam::CPaneParam(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneParam::IDD, pParent)
{
}

CPaneParam::~CPaneParam()
{
}

void CPaneParam::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_ED_P_MASS, m_MassEditCtrl );
	DDX_Control( pDX, IDC_BT_P_LOAD, m_LoadBtCtrl );

	DDX_Check( pDX, IDC_CHK_P_LOCK, m_bLock );
	DDX_Check( pDX, IDC_CHK_P_G, m_bGravity );
	DDX_Check( pDX, IDC_CHK_P_V, m_bVelocity );

	DDX_Check( pDX, IDC_CHK_P_COLSELF, m_bCollisionSelf );
	DDX_Check( pDX, IDC_CHK_P_COLOBJ, m_bCollisionObject );
	DDX_Check( pDX, IDC_CHK_P_COLWLD, m_bCollisionWorld );

	DDX_Check( pDX, IDC_CHK_P_IMPULSE, m_bImpulse );
	DDX_Control( pDX, IDC_ED_P_IMPULSE, m_ImpulseEditCtrl );
	DDX_Control( pDX, IDC_SPIN_IMPULSE, m_ImpulseSpinCtrl );
	DDX_Check( pDX, IDC_CHK_P_FRICTION, m_bFriction );
	DDX_Control( pDX, IDC_ED_P_FRICTION, m_FrictionEditCtrl );
	DDX_Control( pDX, IDC_SPIN_FRICTION, m_FrictionSpinCtrl );
	DDX_Check( pDX, IDC_CHK_P_DRAG, m_bDrag );
	DDX_Control( pDX, IDC_ED_P_DRAG, m_DragEditCtrl );
	DDX_Control( pDX, IDC_SPIN_DRAG, m_DragSpinCtrl );
}

void CPaneParam::SetParticle( i3PhysicsParticle * pParticle )
{
	char conv[256];

	if( pParticle == NULL)
	{
		m_MassEditCtrl.SetWindowText( "" );
		
		m_bLock = FALSE;
		m_bGravity = FALSE;
		m_bVelocity = FALSE;

		m_bCollisionSelf = FALSE;
		m_bCollisionObject = FALSE;
		m_bCollisionWorld = FALSE;

		m_bImpulse = FALSE;
		m_ImpulseEditCtrl.SetWindowText( "" );

		m_bFriction = FALSE;
		m_FrictionEditCtrl.SetWindowText( "" );

		m_bDrag = FALSE;
		m_DragEditCtrl.SetWindowText( "" );
	}
	else
	{
		i3String::ftoa( pParticle->getMass(), conv );
		m_MassEditCtrl.SetWindowText( conv );
		if( pParticle->getStyle() & I3_PHYSICS_PARAM_LOCKED )
			m_bLock = TRUE;
		else
			m_bLock = FALSE;

		if( pParticle->getStyle() & I3_PHYSICS_PARAM_GRAVITY )
			m_bGravity = TRUE;
		else
			m_bGravity = FALSE;

		if( pParticle->getStyle() & I3_PHYSICS_PARAM_VELOCITY )
			m_bVelocity = TRUE;
		else
			m_bVelocity = FALSE;

		if( pParticle->getStyle() & I3_PHYSICS_COLLISION_SELF)
			m_bCollisionSelf = TRUE;
		else
			m_bCollisionSelf = FALSE;

		if( pParticle->getStyle() & I3_PHYSICS_COLLISION_OBJECT )
			m_bCollisionObject = TRUE;
		else
			m_bCollisionObject = FALSE;

		if( pParticle->getStyle() & I3_PHYSICS_COLLISION_WORLD )
			m_bCollisionWorld = TRUE;
		else
			m_bCollisionWorld = FALSE;

		if( pParticle->getStyle() & I3_PHYSICS_COLLISION_IMPULSE )
		{
			m_bImpulse = TRUE;
			i3String::ftoa( pParticle->getRestitution(), conv );
			m_ImpulseEditCtrl.SetWindowText( conv );
		}
		else
			m_bImpulse = FALSE;

		if( pParticle->getStyle() & I3_PHYSICS_COLLISION_FRICTION )
		{
			m_bFriction = TRUE;
			i3String::ftoa( pParticle->getFriction(), conv );
			m_FrictionEditCtrl.SetWindowText( conv );
		}
		else
			m_bFriction = FALSE;
	}
}

BEGIN_MESSAGE_MAP(CPaneParam, i3TDKDialogBase)
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_ED_P_MASS, OnEnChangeEdPMass)
	ON_EN_CHANGE(IDC_ED_P_IMPULSE, OnEnChangeEdPImpulse)
END_MESSAGE_MAP()


// CPaneParam message handlers

int CPaneParam::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText( "Particle");

	//SetParticle( NULL );

	return 0;
}

void CPaneParam::OnEnChangeEdPMass()
{
	char conv[256];

	m_MassEditCtrl.GetWindowText( conv, sizeof(conv) - 1 );

	m_pParticle->setMass( (REAL32)( atof(conv) ) );
}

void CPaneParam::OnEnChangeEdPImpulse()
{
	char conv[256];

	m_ImpulseEditCtrl.GetWindowText( conv, sizeof(conv) - 1 );

	m_pParticle->setRestitution( (REAL32)( atof(conv) ) );
}

void CPaneParam::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	INT32 i;
	i3List SelList;
	char conv[256];
	REAL32 f1, f2, f3, f4;
	
	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		g_pFramework->GetSelectedSpring( &SelList);

		// 질량
		m_MassEditCtrl.GetWindowText( conv, sizeof(conv) - 1 );
		f1 = (REAL32)atof( conv );
		// 반동량
		m_ImpulseEditCtrl.GetWindowText( conv, sizeof(conv) - 1 );
		f2 = (REAL32)atof( conv );
		// 마찰량
		m_FrictionEditCtrl.GetWindowText( conv, sizeof(conv) - 1 );
		f3 = (REAL32)atof( conv );
		// 점성항력
		m_DragEditCtrl.GetWindowText( conv, sizeof(conv) - 1 );
		f4 = (REAL32)atof( conv );

		for( i = 0; i < SelList.GetCount(); i++)
		{
			i3PhysicsParticle * pParticle = (i3PhysicsParticle *) SelList.Items[i];

			pParticle->setMass( f1 );
			pParticle->setRestitution( f2 );
			pParticle->setFriction( f3 );
			pParticle->setDragFactor( f4 );
		}

	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		g_pFramework->GetSelectedSpring( &SelList);

		if( SelList.GetCount() <= 0)
		{
			//  모든 Window를 Disable하는 등의 처리..
		}
	}
}

LRESULT CPaneParam::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}