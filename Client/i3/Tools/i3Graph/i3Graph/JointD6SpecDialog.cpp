// JointD6SpecDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "JointD6SpecDialog.h"
#include ".\jointd6specdialog.h"


// CJointD6SpecDialog dialog

IMPLEMENT_DYNAMIC(CJointD6SpecDialog, CDialog)
CJointD6SpecDialog::CJointD6SpecDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CJointD6SpecDialog::IDD, pParent)
{
}

CJointD6SpecDialog::~CJointD6SpecDialog()
{
}

void CJointD6SpecDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_JOINTNAME, m_EdtJointNameCtrl);
	DDX_Control(pDX, IDC_EDIT_MOTION_X, m_EdtMotionXCtrl);
	DDX_Control(pDX, IDC_EDIT_MOTION_Y, m_EdtMotionYCtrl);
	DDX_Control(pDX, IDC_EDIT_MOTION_Z, m_EdtMotionZCtrl);
	DDX_Control(pDX, IDC_EDIT_MOTION_SWING1, m_EdtSwing1MotionCtrl);
	DDX_Control(pDX, IDC_EDIT_MOTION_SWING2, m_EdtSwing2MotionCtrl);
	DDX_Control(pDX, IDC_EDIT_MOTION_TWIST, m_EdtTwistMotionCtrl);
	DDX_Control(pDX, IDC_EDIT_LINEARLIMIT_VALUE, m_EdtLinearLimitValueCtrl);
	DDX_Control(pDX, IDC_EDIT_LINEARLIMIT_RESTITUTION, m_EdtLinearLimitRestitutionCtrl);
	DDX_Control(pDX, IDC_EDIT_LINEARLIMIT_SPRING, m_EdtLinearLimitSpringCtrl);
	DDX_Control(pDX, IDC_EDIT_LINEARLIMIT_DAMPING, m_EdtLinearLimitDampingCtrl);
	DDX_Control(pDX, IDC_EDIT_SWING1LIMIT_VALUE, m_EdtSwing1LimitValueCtrl);
	DDX_Control(pDX, IDC_EDIT_SWING1LIMIT_RESTITUTION, m_EdtSwing1LimitRestitutionCtrl);
	DDX_Control(pDX, IDC_EDIT_SWING1LIMIT_SPRING, m_EdtSwing1LimitSpringCtrl);
	DDX_Control(pDX, IDC_EDIT_SWING1LIMIT_DAMPING, m_EdtSwing1LimitDampingCtrl);
	DDX_Control(pDX, IDC_EDIT_SWING2LIMIT_VALUE, m_EdtSwing2LimitValueCtrl);
	DDX_Control(pDX, IDC_EDIT_SWING2LIMIT_RESTITUTION, m_EdtSwing2LimitRestitutionCtrl);
	DDX_Control(pDX, IDC_EDIT_SWING2LIMIT_SPRING, m_EdtSwing2LimitSpringCtrl);
	DDX_Control(pDX, IDC_EDIT_SWING2LIMIT_DAMPING, m_EdtSwing2LimitDampingCtrl);
	DDX_Control(pDX, IDC_EDIT_GEARRATIO, m_EdtGearRatioCtrl);
	DDX_Control(pDX, IDC_EDIT_TWISTLIMIT_HIGH_VALUE, m_EdtTwistLimitHighValueCtrl);
	DDX_Control(pDX, IDC_EDIT_TWISTLIMIT_HIGH_RESTITUTION, m_EdtTwistLimitHighRestitutionCtrl);
	DDX_Control(pDX, IDC_EDIT_TWISTLIMIT_HIGH_SPRING, m_EdtTwistLimitHighSpringCtrl);
	DDX_Control(pDX, IDC_EDIT_TWISTLIMIT_HIGH_DAMPING, m_EdtTwistLimitHighDampingCtrl);
	DDX_Control(pDX, IDC_EDIT_TWISTLIMIT_LOW_VALUE, m_EdtTwistLimitLowValueCtrl);
	DDX_Control(pDX, IDC_EDIT_TWISTLIMIT_LOW_RESTITUTION, m_EdtTwistLimitLowRestitutionCtrl);
	DDX_Control(pDX, IDC_EDIT_TWISTLIMIT_LOW_SPRING, m_EdtTwistLimitLowSpringCtrl);
	DDX_Control(pDX, IDC_EDIT_TWISTLIMIT_LOW_DAMPING, m_EdtTwistLimitLowDampingCtrl);
}


BEGIN_MESSAGE_MAP(CJointD6SpecDialog, CDialog)
	ON_EN_CHANGE(IDC_EDIT_MOTION_X, OnEnChangeEditMotionX)
	ON_EN_CHANGE(IDC_EDIT_MOTION_Y, OnEnChangeEditMotionY)
	ON_EN_CHANGE(IDC_EDIT_MOTION_Z, OnEnChangeEditMotionZ)
	ON_EN_CHANGE(IDC_EDIT_MOTION_SWING1, OnEnChangeEditMotionSwing1)
	ON_EN_CHANGE(IDC_EDIT_MOTION_SWING2, OnEnChangeEditMotionSwing2)
	ON_EN_CHANGE(IDC_EDIT_MOTION_TWIST, OnEnChangeEditMotionTwist)
	ON_EN_CHANGE(IDC_EDIT_LINEARLIMIT_VALUE, OnEnChangeEditLinearlimitValue)
	ON_EN_CHANGE(IDC_EDIT_LINEARLIMIT_RESTITUTION, OnEnChangeEditLinearlimitRestitution)
	ON_EN_CHANGE(IDC_EDIT_LINEARLIMIT_SPRING, OnEnChangeEditLinearlimitSpring)
	ON_EN_CHANGE(IDC_EDIT_LINEARLIMIT_DAMPING, OnEnChangeEditLinearlimitDamping)
	ON_EN_CHANGE(IDC_EDIT_SWING1LIMIT_VALUE, OnEnChangeEditSwing1limitValue)
	ON_EN_CHANGE(IDC_EDIT_SWING1LIMIT_RESTITUTION, OnEnChangeEditSwing1limitRestitution)
	ON_EN_CHANGE(IDC_EDIT_SWING1LIMIT_SPRING, OnEnChangeEditSwing1limitSpring)
	ON_EN_CHANGE(IDC_EDIT_SWING1LIMIT_DAMPING, OnEnChangeEditSwing1limitDamping)
	ON_EN_CHANGE(IDC_EDIT_SWING2LIMIT_VALUE, OnEnChangeEditSwing2limitValue)
	ON_EN_CHANGE(IDC_EDIT_SWING2LIMIT_RESTITUTION, OnEnChangeEditSwing2limitRestitution)
	ON_EN_CHANGE(IDC_EDIT_SWING2LIMIT_SPRING, OnEnChangeEditSwing2limitSpring)
	ON_EN_CHANGE(IDC_EDIT_SWING2LIMIT_DAMPING, OnEnChangeEditSwing2limitDamping)
	ON_EN_CHANGE(IDC_EDIT_TWISTLIMIT_HIGH_VALUE, OnEnChangeEditTwistlimitHighValue)
	ON_EN_CHANGE(IDC_EDIT_TWISTLIMIT_HIGH_RESTITUTION, OnEnChangeEditTwistlimitHighRestitution)
	ON_EN_CHANGE(IDC_EDIT_TWISTLIMIT_HIGH_SPRING, OnEnChangeEditTwistlimitHighSpring)
	ON_EN_CHANGE(IDC_EDIT_TWISTLIMIT_HIGH_DAMPING, OnEnChangeEditTwistlimitHighDamping)
	ON_EN_CHANGE(IDC_EDIT_TWISTLIMIT_LOW_VALUE, OnEnChangeEditTwistlimitLowValue)
	ON_EN_CHANGE(IDC_EDIT_TWISTLIMIT_LOW_RESTITUTION, OnEnChangeEditTwistlimitLowRestitution)
	ON_EN_CHANGE(IDC_EDIT_TWISTLIMIT_LOW_SPRING, OnEnChangeEditTwistlimitLowSpring)
	ON_EN_CHANGE(IDC_EDIT_TWISTLIMIT_LOW_DAMPING, OnEnChangeEditTwistlimitLowDamping)
	ON_EN_CHANGE(IDC_EDIT_GEARRATIO, OnEnChangeEditGearratio)
END_MESSAGE_MAP()


// CJointD6SpecDialog message handlers

BOOL CJointD6SpecDialog::Execute( i3PhysixShapeSet * pShapeSet )
{
	m_pShapeSet = pShapeSet;

	if( DoModal() == IDCANCEL )
		return FALSE;

	return TRUE;
}

BOOL CJointD6SpecDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	AssignJointInfo();

	return TRUE;
}

void CJointD6SpecDialog::AssignJointInfo()
{
	char szTemp[256];

	if( m_pShapeSet != NULL && m_pShapeSet->getJointInfo() != NULL )
	{
		i3PhysixJointInfoD6 * pJointInfo = (i3PhysixJointInfoD6*)m_pShapeSet->getJointInfo();
		sprintf( szTemp, "%p", pJointInfo);
		m_EdtJointNameCtrl.SetWindowText( szTemp);
		m_EdtJointNameCtrl.EnableWindow( FALSE );

		switch( pJointInfo->xMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		strcpy( szTemp, "Locked" );	break;
		case NX_D6JOINT_MOTION_LIMITED :	strcpy( szTemp, "Limited");	break;
		case NX_D6JOINT_MOTION_FREE :		strcpy( szTemp, "Free" );	break;
		default : strcpy( szTemp, "unknown");	break;
		}
		m_EdtMotionXCtrl.SetWindowText( szTemp);

		switch( pJointInfo->xMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		strcpy( szTemp, "Locked" );	break;
		case NX_D6JOINT_MOTION_LIMITED :	strcpy( szTemp, "Limited");	break;
		case NX_D6JOINT_MOTION_FREE :		strcpy( szTemp, "Free" );	break;
		default : strcpy( szTemp, "unknown");	break;
		}
		m_EdtMotionXCtrl.SetWindowText( szTemp);

		switch( pJointInfo->yMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		strcpy( szTemp, "Locked" );	break;
		case NX_D6JOINT_MOTION_LIMITED :	strcpy( szTemp, "Limited");	break;
		case NX_D6JOINT_MOTION_FREE :		strcpy( szTemp, "Free" );	break;
		default : strcpy( szTemp, "unknown");	break;
		}
		m_EdtMotionYCtrl.SetWindowText( szTemp);

		switch( pJointInfo->zMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		strcpy( szTemp, "Locked" );	break;
		case NX_D6JOINT_MOTION_LIMITED :	strcpy( szTemp, "Limited");	break;
		case NX_D6JOINT_MOTION_FREE :		strcpy( szTemp, "Free" );	break;
		default : strcpy( szTemp, "unknown");	break;
		}
		m_EdtMotionZCtrl.SetWindowText( szTemp);

		switch( pJointInfo->swing1Motion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		strcpy( szTemp, "Locked" );	break;
		case NX_D6JOINT_MOTION_LIMITED :	strcpy( szTemp, "Limited");	break;
		case NX_D6JOINT_MOTION_FREE :		strcpy( szTemp, "Free" );	break;
		default : strcpy( szTemp, "unknown");	break;
		}
		m_EdtSwing1MotionCtrl.SetWindowText( szTemp);

		switch( pJointInfo->swing2Motion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		strcpy( szTemp, "Locked" );	break;
		case NX_D6JOINT_MOTION_LIMITED :	strcpy( szTemp, "Limited");	break;
		case NX_D6JOINT_MOTION_FREE :		strcpy( szTemp, "Free" );	break;
		default : strcpy( szTemp, "unknown");	break;
		}
		m_EdtSwing2MotionCtrl.SetWindowText( szTemp);

		switch( pJointInfo->twistMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		strcpy( szTemp, "Locked" );	break;
		case NX_D6JOINT_MOTION_LIMITED :	strcpy( szTemp, "Limited");	break;
		case NX_D6JOINT_MOTION_FREE :		strcpy( szTemp, "Free" );	break;
		default : strcpy( szTemp, "unknown");	break;
		}
		m_EdtTwistMotionCtrl.SetWindowText( szTemp);
		// linear limit
		i3String::ftoa( pJointInfo->linearLimit.value, szTemp);
		m_EdtLinearLimitValueCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->linearLimit.restitution, szTemp);
		m_EdtLinearLimitRestitutionCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->linearLimit.spring, szTemp);
		m_EdtLinearLimitSpringCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->linearLimit.damping, szTemp);
		m_EdtLinearLimitDampingCtrl.SetWindowText( szTemp);
		// swing1 limit
		i3String::ftoa( pJointInfo->swing1Limit.value, szTemp);
		m_EdtSwing1LimitValueCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->swing1Limit.restitution, szTemp);
		m_EdtSwing1LimitRestitutionCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->swing1Limit.spring, szTemp);
		m_EdtSwing1LimitSpringCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->swing1Limit.damping, szTemp);
		m_EdtSwing1LimitDampingCtrl.SetWindowText( szTemp);
		// swing2 limit
		i3String::ftoa( pJointInfo->swing2Limit.value, szTemp);
		m_EdtSwing2LimitValueCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->swing2Limit.restitution, szTemp);
		m_EdtSwing2LimitRestitutionCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->swing2Limit.spring, szTemp);
		m_EdtSwing2LimitSpringCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->swing2Limit.damping, szTemp);
		m_EdtSwing2LimitDampingCtrl.SetWindowText( szTemp);
		// twist high limit
		i3String::ftoa( pJointInfo->twistLimit.high.value, szTemp);
		m_EdtTwistLimitHighValueCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->twistLimit.high.restitution, szTemp);
		m_EdtTwistLimitHighRestitutionCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->twistLimit.high.spring, szTemp);
		m_EdtTwistLimitHighSpringCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->twistLimit.high.damping, szTemp);
		m_EdtTwistLimitHighDampingCtrl.SetWindowText( szTemp);

		// twist low limit
		i3String::ftoa( pJointInfo->twistLimit.low.value, szTemp);
		m_EdtTwistLimitLowValueCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->twistLimit.low.restitution, szTemp);
		m_EdtTwistLimitLowRestitutionCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->twistLimit.low.spring, szTemp);
		m_EdtTwistLimitLowSpringCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->twistLimit.low.damping, szTemp);
		m_EdtTwistLimitLowDampingCtrl.SetWindowText( szTemp);

		i3String::ftoa( pJointInfo->gearRatio, szTemp);
		m_EdtGearRatioCtrl.SetWindowText( "0.0");
	}
	else
	{
		m_EdtJointNameCtrl.SetWindowText( "(none)");
		m_EdtJointNameCtrl.EnableWindow( FALSE );
		m_EdtMotionXCtrl.SetWindowText( "unknown");
		m_EdtMotionYCtrl.SetWindowText( "unknown");
		m_EdtMotionZCtrl.SetWindowText( "unknown");
		m_EdtSwing1MotionCtrl.SetWindowText( "unknown");
		m_EdtSwing2MotionCtrl.SetWindowText( "unknown");
		m_EdtTwistMotionCtrl.SetWindowText( "unknown");

		m_EdtLinearLimitValueCtrl.SetWindowText( "0.0");
		m_EdtLinearLimitRestitutionCtrl.SetWindowText( "0.0");
		m_EdtLinearLimitSpringCtrl.SetWindowText( "0.0");
		m_EdtLinearLimitDampingCtrl.SetWindowText( "0.0");
		m_EdtSwing1LimitValueCtrl.SetWindowText( "0.0");
		m_EdtSwing1LimitRestitutionCtrl.SetWindowText( "0.0");
		m_EdtSwing1LimitSpringCtrl.SetWindowText( "0.0");
		m_EdtSwing1LimitDampingCtrl.SetWindowText( "0.0");
		m_EdtSwing2LimitValueCtrl.SetWindowText( "0.0");
		m_EdtSwing2LimitRestitutionCtrl.SetWindowText( "0.0");
		m_EdtSwing2LimitSpringCtrl.SetWindowText( "0.0");
		m_EdtSwing2LimitDampingCtrl.SetWindowText( "0.0");
		
		m_EdtGearRatioCtrl.SetWindowText( "0.0");
	}

}

void CJointD6SpecDialog::CheckMotion( NxD6JointMotion * pMotion, char * pStr )
{
	if( pStr == NULL )
		return;

	if( i3String::Compare( pStr, "Locked" ) == 0 )
	{
		(*pMotion) = NX_D6JOINT_MOTION_LOCKED;
	}
	else if( i3String::Compare( pStr, "Limited") == 0 )
	{
		(*pMotion) = NX_D6JOINT_MOTION_LIMITED;
	}
	else if( i3String::Compare( pStr, "Free") == 0 )
	{
		(*pMotion) = NX_D6JOINT_MOTION_FREE;
	}
	else
	{
		// 잘못입력
		CDialog dlg;
		dlg.SetWindowText( "Invalid element. You must select next word -> (Locked, Limited, Free)" );
	}
}

void CJointD6SpecDialog::OnEnChangeEditMotionX()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtMotionXCtrl.GetWindowText( conv, 256 );

	CheckMotion( &pInfo->xMotion, conv );
}

void CJointD6SpecDialog::OnEnChangeEditMotionY()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtMotionYCtrl.GetWindowText( conv, 256 );

	CheckMotion( &pInfo->yMotion, conv );
}

void CJointD6SpecDialog::OnEnChangeEditMotionZ()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtMotionZCtrl.GetWindowText( conv, 256 );

	CheckMotion( &pInfo->zMotion, conv );
}

void CJointD6SpecDialog::OnEnChangeEditMotionSwing1()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing1MotionCtrl.GetWindowText( conv, 256 );

	CheckMotion( &pInfo->swing1Motion, conv );
}

void CJointD6SpecDialog::OnEnChangeEditMotionSwing2()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing2MotionCtrl.GetWindowText( conv, 256 );

	CheckMotion( &pInfo->swing2Motion, conv );
}

void CJointD6SpecDialog::OnEnChangeEditMotionTwist()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtTwistMotionCtrl.GetWindowText( conv, 256 );

	CheckMotion( &pInfo->twistMotion, conv );
}

void CJointD6SpecDialog::OnEnChangeEditLinearlimitValue()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtLinearLimitValueCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->linearLimit.value = value;
}

void CJointD6SpecDialog::OnEnChangeEditLinearlimitRestitution()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtLinearLimitRestitutionCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->linearLimit.restitution = value;
}

void CJointD6SpecDialog::OnEnChangeEditLinearlimitSpring()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtLinearLimitSpringCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->linearLimit.spring = value;
}

void CJointD6SpecDialog::OnEnChangeEditLinearlimitDamping()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtLinearLimitDampingCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->linearLimit.damping = value;
}

void CJointD6SpecDialog::OnEnChangeEditSwing1limitValue()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing1LimitValueCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->swing1Limit.value = value;
}

void CJointD6SpecDialog::OnEnChangeEditSwing1limitRestitution()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing1LimitRestitutionCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->swing1Limit.restitution = value;
}

void CJointD6SpecDialog::OnEnChangeEditSwing1limitSpring()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing1LimitSpringCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->swing1Limit.spring = value;
}

void CJointD6SpecDialog::OnEnChangeEditSwing1limitDamping()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing1LimitDampingCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->swing1Limit.damping = value;
}

void CJointD6SpecDialog::OnEnChangeEditSwing2limitValue()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing2LimitValueCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->swing2Limit.value = value;
}

void CJointD6SpecDialog::OnEnChangeEditSwing2limitRestitution()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing2LimitRestitutionCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->swing2Limit.restitution = value;
}

void CJointD6SpecDialog::OnEnChangeEditSwing2limitSpring()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing2LimitSpringCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->swing2Limit.spring = value;
}

void CJointD6SpecDialog::OnEnChangeEditSwing2limitDamping()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtSwing2LimitDampingCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->swing2Limit.damping = value;
}

void CJointD6SpecDialog::OnEnChangeEditTwistlimitHighValue()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtTwistLimitHighValueCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->twistLimit.high.value = value;
}

void CJointD6SpecDialog::OnEnChangeEditTwistlimitHighRestitution()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtTwistLimitHighRestitutionCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->twistLimit.high.restitution = value;
}

void CJointD6SpecDialog::OnEnChangeEditTwistlimitHighSpring()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtTwistLimitHighSpringCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->twistLimit.high.spring = value;
}

void CJointD6SpecDialog::OnEnChangeEditTwistlimitHighDamping()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtTwistLimitHighDampingCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->twistLimit.high.damping = value;
}

void CJointD6SpecDialog::OnEnChangeEditTwistlimitLowValue()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtTwistLimitLowValueCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->twistLimit.low.value = value;
}

void CJointD6SpecDialog::OnEnChangeEditTwistlimitLowRestitution()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtTwistLimitLowRestitutionCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->twistLimit.low.restitution = value;
}

void CJointD6SpecDialog::OnEnChangeEditTwistlimitLowSpring()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtTwistLimitLowSpringCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->twistLimit.low.spring = value;
}

void CJointD6SpecDialog::OnEnChangeEditTwistlimitLowDamping()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtTwistLimitLowDampingCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->twistLimit.low.damping = value;
}

void CJointD6SpecDialog::OnEnChangeEditGearratio()
{
	if( m_pShapeSet == NULL )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == NULL )
		return;

	char conv[256];

	m_EdtGearRatioCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) atof( conv);
	pInfo->gearRatio = value;
}
