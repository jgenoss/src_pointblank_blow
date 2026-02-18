// ParticleSpecObject.cpp : implementation file
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "ParticleSpecObject.h"
#include "GlobalVariable.h"
#include "Util.h"
#include "i3EffectEditorDlg.h"

// CParticleSpecObject dialog

IMPLEMENT_DYNAMIC(CParticleSpecObject, i3TDKDialogBase)
CParticleSpecObject::CParticleSpecObject(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CParticleSpecObject::IDD, pParent)
{
	m_pInfo = NULL;
}

CParticleSpecObject::~CParticleSpecObject()
{
}

void CParticleSpecObject::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_OBJECT_LIFETIME, m_LifetimeCtrl);
	DDX_Control(pDX, IDC_SPIN_OBJECT_LIFETIME, m_LifetimeSpinCtrl);
	DDX_Control(pDX, IDC_ED_MAXCOUNT, m_MaxCountCtrl);
	DDX_Control(pDX, IDC_SPIN_MAXCOUNT, m_MaxCountSpinCtrl);
	DDX_Control(pDX, IDC_ED_INITCOUNT, m_InitCountCtrl);
	DDX_Control(pDX, IDC_ED_CAPACITYCOUNT, m_CapacityCountCtrl);
	DDX_Control(pDX, IDC_SPIN_INITCOUNT, m_InitCountSpinCtrl);
	DDX_Control(pDX, IDC_SPIN_CAPACITYCOUNT, m_CapacityCountSpinCtrl);
	DDX_Control(pDX, IDC_ED_MASSMIN, m_MassMinCtrl);
	DDX_Control(pDX, IDC_ED_MASSMAX, m_MassMaxCtrl);
}

void CParticleSpecObject::SetParticle( i3ParticleInfo * pInfo)
{
	char conv[245];

	m_pInfo = pInfo;
	if( m_pInfo == NULL)
	{
		m_LifetimeCtrl.SetWindowText( "");
		m_MassMinCtrl.SetWindowText( "");
		m_MassMaxCtrl.SetWindowText( "");
		m_MaxCountCtrl.SetWindowText( "");
		m_InitCountCtrl.SetWindowText( "");
		m_CapacityCountCtrl.SetWindowText("");
	}
	else
	{
		REAL2STR( pInfo->GetObjectLifeTime(), conv);
		m_LifetimeCtrl.SetWindowText( conv);

		REAL2STR( pInfo->GetMassMin(), conv);
		m_MassMinCtrl.SetWindowText( conv);
		
		REAL2STR( pInfo->GetMassMax(), conv);
		m_MassMaxCtrl.SetWindowText( conv);

		sprintf( conv, "%d", pInfo->GetMaxObjectCount());
		m_MaxCountCtrl.SetWindowText( conv);

		sprintf( conv, "%d", pInfo->GetInitialObjectCount());
		m_InitCountCtrl.SetWindowText( conv);

		sprintf( conv, "%d", pInfo->GetMaxObjectCapacity());
		m_CapacityCountCtrl.SetWindowText( conv);
	}

	m_LifetimeCtrl.EnableWindow( (m_pInfo != NULL));
	m_MassMinCtrl.EnableWindow( (m_pInfo != NULL));
	m_MassMaxCtrl.EnableWindow( (m_pInfo != NULL));
	m_MaxCountCtrl.EnableWindow( (m_pInfo != NULL));
	m_InitCountCtrl.EnableWindow( (m_pInfo != NULL));
	m_CapacityCountCtrl.EnableWindow( (m_pInfo != NULL));

	UpdateData( FALSE);
}

BEGIN_MESSAGE_MAP(CParticleSpecObject, i3TDKDialogBase)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INITCOUNT,  &CParticleSpecObject::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CAPACITYCOUNT,  &CParticleSpecObject::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MAXCOUNT,  &CParticleSpecObject::OnDeltaPosSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OBJECT_LIFETIME,  &CParticleSpecObject::OnDeltaPosSpin)

	ON_EN_KILLFOCUS(IDC_ED_OBJECT_LIFETIME, &CParticleSpecObject::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_MASSMIN, &CParticleSpecObject::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_MASSMAX, &CParticleSpecObject::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_MAXCOUNT, &CParticleSpecObject::OnEnKillFocus)
	ON_EN_KILLFOCUS(IDC_ED_INITCOUNT, &CParticleSpecObject::OnEnKillFocus)
END_MESSAGE_MAP()


// CParticleSpecObject message handlers

BOOL CParticleSpecObject::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	InitSpinCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CParticleSpecObject::InitSpinCtrl()
{
	m_LifetimeSpinCtrl.SetBuddy( &m_LifetimeCtrl);
	m_LifetimeSpinCtrl.SetRange(0, 100);

	m_MaxCountSpinCtrl.SetBuddy( &m_MaxCountCtrl);
	m_MaxCountSpinCtrl.SetRange(0, SPINCTRL_MAX_COUNT);

	m_InitCountSpinCtrl.SetBuddy( &m_InitCountCtrl);
	m_InitCountSpinCtrl.SetRange(0, SPINCTRL_MAX_COUNT);

	m_CapacityCountSpinCtrl.SetBuddy( &m_CapacityCountCtrl);
	m_CapacityCountSpinCtrl.SetRange(0, SPINCTRL_MAX_COUNT);
}


//void CParticleSpecObject::OnEnChangeEdObjectLifetime()
//{
//	char conv[256];
//
//	if( m_pInfo == NULL)
//		return;
//
//	//UpdateData( TRUE);
//	m_LifetimeCtrl.GetWindowText( conv, sizeof(conv) - 1);
//
//	m_pInfo->SetObjectLifeTime( (REAL32) atof( conv));
//}


//void CParticleSpecObject::OnEnChangeEdMassmin()
//{
//	if( m_pInfo == NULL)
//		return;
//
//	UpdateData( TRUE);
//
//	m_pInfo->SetMassMin( m_MassMin);
//}


//void CParticleSpecObject::OnEnChangeEdMassmax()
//{
//	if( m_pInfo == NULL)
//		return;
//
//	UpdateData( TRUE);
//
//	m_pInfo->SetMassMax( m_MassMax);
//}


//void CParticleSpecObject::OnEnChangeEdMaxcount()
//{
//	char conv[256];
//
//	if( m_pInfo == NULL)
//		return;
//
//	m_MaxCountCtrl.GetWindowText( conv, sizeof(conv) - 1);
//
//	m_pInfo->SetMaxObjectCount( atoi( conv));
//}


//void CParticleSpecObject::OnEnChangeEdInitcount()
//{
//	char conv[256];
//
//	if( m_pInfo == NULL)
//		return;
//
//	m_InitCountCtrl.GetWindowText( conv, sizeof(conv) - 1);
//
//	m_pInfo->SetInitialObjectCount( atoi( conv));
//}


void CParticleSpecObject::OnCancel()
{
}

void CParticleSpecObject::OnOK()
{
	char conv[256];

	if( m_pInfo == NULL)
		return;

	UpdateData( TRUE);

	EnterRenderCS();

	m_LifetimeCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetObjectLifeTime( (REAL32) atof( conv));

	m_MassMinCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetMassMin( (REAL32) atof( conv));

	m_MassMaxCtrl.GetWindowText( conv, sizeof(conv) - 1);
	m_pInfo->SetMassMax( (REAL32) atof( conv));

	INT32 maxCount = 0;
	INT32 initCount = 0;
	INT32 capacityCount = 0;

	m_MaxCountCtrl.GetWindowText( conv, sizeof(conv) - 1);
	maxCount = atoi(conv);

	m_InitCountCtrl.GetWindowText( conv, sizeof(conv) - 1);
	initCount = atoi(conv);

	m_CapacityCountCtrl.GetWindowText( conv, sizeof(conv) - 1);
	capacityCount = atoi(conv);

	//무조건 maxCount or initCount 둘 중에 하나가 변경되어 들어온다. KillFocus 이벤트로 인해 둘 다 변경되어 들어올 수는 없음
	char ctrlStr[MAX_PATH];
	if(maxCount < 0)
	{
		maxCount = initCount;

		i3::snprintf(ctrlStr, MAX_PATH, "%d", maxCount);
		m_MaxCountCtrl.SetWindowText(ctrlStr);
	}
	else if (initCount < 0)
	{
		initCount = maxCount;
		
		i3::snprintf(ctrlStr, MAX_PATH, "%d", initCount);
		m_InitCountCtrl.SetWindowText(ctrlStr);
	}
	else if (capacityCount < 0)
	{
		capacityCount = maxCount;

		i3::snprintf(ctrlStr, MAX_PATH, "%d", capacityCount);
		m_CapacityCountCtrl.SetWindowText(ctrlStr);
	}
	if(maxCount < initCount)
	{
		maxCount = initCount;

		i3::snprintf(ctrlStr, MAX_PATH, "%d", maxCount);
		m_MaxCountCtrl.SetWindowText(ctrlStr);
	}

	m_pInfo->SetMaxObjectCount(maxCount);
	m_pInfo->SetInitialObjectCount(initCount);
	m_pInfo->SetMaxObjectCapacity(capacityCount);
	
	g_bNeedToCreate = TRUE;
	g_pMainWnd->StopRender();

	LeaveRenderCS();
}

void CParticleSpecObject::OnEnKillFocus()
{
	OnOK();
}


void CParticleSpecObject::OnDeltaPosSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnOK();
}

