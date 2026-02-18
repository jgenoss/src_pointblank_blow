// PaneSpring.cpp : implementation file
//

#include "stdafx.h"
#include "i3ClothSim.h"
#include "GlobalVariable.h"
#include "PaneSpring.h"
#include ".\panespring.h"


// CPaneSpring dialog

IMPLEMENT_DYNAMIC(CPaneSpring, i3TDKDialogBase)
CPaneSpring::CPaneSpring(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneSpring::IDD, pParent)
{
}

CPaneSpring::~CPaneSpring()
{
}

void CPaneSpring::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_CB_S_TYPE,		m_TypeCbCtrl );
	DDX_Control( pDX, IDC_ED_S_DAMPER,		m_DamperEdCtrl );
	DDX_Control( pDX, IDC_SPIN_S_DAMPER,	m_DamperSpinCtrl );
	DDX_Control( pDX, IDC_ED_S_CONSTANT,	m_ConstantEdCtrl );
	DDX_Control( pDX, IDC_SPIN_S_CONSTANT,	m_ConstantSpinCtrl );
	DDX_Control( pDX, IDC_BT_S_LOAD,		m_LoadButtonCtrl);
}

void CPaneSpring::SetSpring( i3Spring * pSpring )
{
	m_pSpring = pSpring;
}


BEGIN_MESSAGE_MAP(CPaneSpring, i3TDKDialogBase)
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_ED_S_DAMPER, OnEnChangeEdSDamper)
	ON_EN_CHANGE(IDC_ED_S_CONSTANT, OnEnChangeEdSConstant)
END_MESSAGE_MAP()


// CPaneSpring message handlers

int CPaneSpring::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText( "Spring");

	i3TDK::RegisterUpdate( m_hWnd, i3Spring::GetClassMeta(), I3_TDK_UPDATE_ALL);
	
	return 0;
}

void CPaneSpring::OnEnChangeEdSDamper()
{
	INT32 i;
	char conv[256];
	i3Spring * pSpring;

	m_DamperEdCtrl.GetWindowText( conv, sizeof(conv) - 1 );
	for( i = 0; i < g_pFramework->getCloth()->getSpringCount(); i++ )
	{
		pSpring = g_pFramework->getCloth()->getSpring( i );
		pSpring->setDamp( (REAL32)( atof(conv) ) );
	}
}

void CPaneSpring::OnEnChangeEdSConstant()
{
	INT32 i;
	char conv[256];
	i3Spring * pSpring;

	m_ConstantEdCtrl.GetWindowText( conv, sizeof(conv) - 1 );

	for( i = 0; i < g_pFramework->getCloth()->getSpringCount(); i++ )
	{
		pSpring = g_pFramework->getCloth()->getSpring( i );
		pSpring->setConstant( (REAL32)( atof(conv) ) );
	}
}

void CPaneSpring::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	INT32 i;
	i3List SelList;
	char conv[256];
	REAL32 constf, dampf;

	m_ConstantEdCtrl.GetWindowText( conv, sizeof(conv) - 1);
	constf = (REAL32)atof( conv );

	m_DamperEdCtrl.GetWindowText( conv, sizeof(conv) - 1);
	dampf = (REAL32)atof( conv );

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		g_pFramework->GetSelectedSpring( &SelList);

		for( i = 0; i < SelList.GetCount(); i++)
		{
			i3Spring * pSpring = (i3Spring *) SelList.Items[i];

			pSpring->setDamp(dampf);
			pSpring->setConstant(constf);
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

LRESULT CPaneSpring::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}
