// i3TDKSpec_Joint.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Joint.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKSpec_Joint dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Joint, CDialog)


void i3TDKSpec_Joint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_JOINTNAME, m_EdtJointNameCtrl);
	DDX_Control(pDX, IDC_TDK_ED_MOTION_X, m_EdtMotionXCtrl);
	DDX_Control(pDX, IDC_TDK_ED_MOTION_Y, m_EdtMotionYCtrl);
	DDX_Control(pDX, IDC_TDK_ED_MOTION_Z, m_EdtMotionZCtrl);
	DDX_Control(pDX, IDC_TDK_ED_MOTION_SWING1, m_EdtSwing1MotionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_MOTION_SWING2, m_EdtSwing2MotionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_MOTION_TWIST, m_EdtTwistMotionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_LINEARLIMIT_VALUE, m_EdtLinearLimitValueCtrl);
	DDX_Control(pDX, IDC_TDK_ED_LINEARLIMIT_RESTITUTION, m_EdtLinearLimitRestitutionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_LINEARLIMIT_SPRING, m_EdtLinearLimitSpringCtrl);
	DDX_Control(pDX, IDC_TDK_ED_LINEARLIMIT_DAMPING, m_EdtLinearLimitDampingCtrl);
	DDX_Control(pDX, IDC_TDK_ED_SWING1LIMIT_VALUE, m_EdtSwing1LimitValueCtrl);
	DDX_Control(pDX, IDC_TDK_ED_SWING1LIMIT_RESTITUTION, m_EdtSwing1LimitRestitutionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_SWING1LIMIT_SPRING, m_EdtSwing1LimitSpringCtrl);
	DDX_Control(pDX, IDC_TDK_ED_SWING1LIMIT_DAMPING, m_EdtSwing1LimitDampingCtrl);
	DDX_Control(pDX, IDC_TDK_ED_SWING2LIMIT_VALUE, m_EdtSwing2LimitValueCtrl);
	DDX_Control(pDX, IDC_TDK_ED_SWING2LIMIT_RESTITUTION, m_EdtSwing2LimitRestitutionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_SWING2LIMIT_SPRING, m_EdtSwing2LimitSpringCtrl);
	DDX_Control(pDX, IDC_TDK_ED_SWING2LIMIT_DAMPING, m_EdtSwing2LimitDampingCtrl);
	DDX_Control(pDX, IDC_TDK_ED_GEARRATIO, m_EdtGearRatioCtrl);
	DDX_Control(pDX, IDC_TDK_ED_TWISTLIMIT_HIGH_VALUE, m_EdtTwistLimitHighValueCtrl);
	DDX_Control(pDX, IDC_TDK_ED_TWISTLIMIT_HIGH_RESTITUTION, m_EdtTwistLimitHighRestitutionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_TWISTLIMIT_HIGH_SPRING, m_EdtTwistLimitHighSpringCtrl);
	DDX_Control(pDX, IDC_TDK_ED_TWISTLIMIT_HIGH_DAMPING, m_EdtTwistLimitHighDampingCtrl);
	DDX_Control(pDX, IDC_TDK_ED_TWISTLIMIT_LOW_VALUE, m_EdtTwistLimitLowValueCtrl);
	DDX_Control(pDX, IDC_TDK_ED_TWISTLIMIT_LOW_RESTITUTION, m_EdtTwistLimitLowRestitutionCtrl);
	DDX_Control(pDX, IDC_TDK_ED_TWISTLIMIT_LOW_SPRING, m_EdtTwistLimitLowSpringCtrl);
	DDX_Control(pDX, IDC_TDK_ED_TWISTLIMIT_LOW_DAMPING, m_EdtTwistLimitLowDampingCtrl);
}


BEGIN_MESSAGE_MAP(i3TDKSpec_Joint, CDialog)
	ON_EN_CHANGE(IDC_TDK_ED_MOTION_X, OnEnChangeEditMotionX)
	ON_EN_CHANGE(IDC_TDK_ED_MOTION_Y, OnEnChangeEditMotionY)
	ON_EN_CHANGE(IDC_TDK_ED_MOTION_Z, OnEnChangeEditMotionZ)
	ON_EN_CHANGE(IDC_TDK_ED_MOTION_SWING1, OnEnChangeEditMotionSwing1)
	ON_EN_CHANGE(IDC_TDK_ED_MOTION_SWING2, OnEnChangeEditMotionSwing2)
	ON_EN_CHANGE(IDC_TDK_ED_MOTION_TWIST, OnEnChangeEditMotionTwist)
	ON_EN_CHANGE(IDC_TDK_ED_LINEARLIMIT_VALUE, OnEnChangeEditLinearlimitValue)
	ON_EN_CHANGE(IDC_TDK_ED_LINEARLIMIT_RESTITUTION, OnEnChangeEditLinearlimitRestitution)
	ON_EN_CHANGE(IDC_TDK_ED_LINEARLIMIT_SPRING, OnEnChangeEditLinearlimitSpring)
	ON_EN_CHANGE(IDC_TDK_ED_LINEARLIMIT_DAMPING, OnEnChangeEditLinearlimitDamping)
	ON_EN_CHANGE(IDC_TDK_ED_SWING1LIMIT_VALUE, OnEnChangeEditSwing1limitValue)
	ON_EN_CHANGE(IDC_TDK_ED_SWING1LIMIT_RESTITUTION, OnEnChangeEditSwing1limitRestitution)
	ON_EN_CHANGE(IDC_TDK_ED_SWING1LIMIT_SPRING, OnEnChangeEditSwing1limitSpring)
	ON_EN_CHANGE(IDC_TDK_ED_SWING1LIMIT_DAMPING, OnEnChangeEditSwing1limitDamping)
	ON_EN_CHANGE(IDC_TDK_ED_SWING2LIMIT_VALUE, OnEnChangeEditSwing2limitValue)
	ON_EN_CHANGE(IDC_TDK_ED_SWING2LIMIT_RESTITUTION, OnEnChangeEditSwing2limitRestitution)
	ON_EN_CHANGE(IDC_TDK_ED_SWING2LIMIT_SPRING, OnEnChangeEditSwing2limitSpring)
	ON_EN_CHANGE(IDC_TDK_ED_SWING2LIMIT_DAMPING, OnEnChangeEditSwing2limitDamping)
	ON_EN_CHANGE(IDC_TDK_ED_TWISTLIMIT_HIGH_VALUE, OnEnChangeEditTwistlimitHighValue)
	ON_EN_CHANGE(IDC_TDK_ED_TWISTLIMIT_HIGH_RESTITUTION, OnEnChangeEditTwistlimitHighRestitution)
	ON_EN_CHANGE(IDC_TDK_ED_TWISTLIMIT_HIGH_SPRING, OnEnChangeEditTwistlimitHighSpring)
	ON_EN_CHANGE(IDC_TDK_ED_TWISTLIMIT_HIGH_DAMPING, OnEnChangeEditTwistlimitHighDamping)
	ON_EN_CHANGE(IDC_TDK_ED_TWISTLIMIT_LOW_VALUE, OnEnChangeEditTwistlimitLowValue)
	ON_EN_CHANGE(IDC_TDK_ED_TWISTLIMIT_LOW_RESTITUTION, OnEnChangeEditTwistlimitLowRestitution)
	ON_EN_CHANGE(IDC_TDK_ED_TWISTLIMIT_LOW_SPRING, OnEnChangeEditTwistlimitLowSpring)
	ON_EN_CHANGE(IDC_TDK_ED_TWISTLIMIT_LOW_DAMPING, OnEnChangeEditTwistlimitLowDamping)
	ON_EN_CHANGE(IDC_TDK_ED_GEARRATIO, OnEnChangeEditGearratio)
END_MESSAGE_MAP()


BOOL i3TDKSpec_Joint::OnInitDialog()
{
	CDialog::OnInitDialog();

	AssignJointInfo();

	return TRUE;
}


// i3TDKSpec_Joint message handlers

BOOL i3TDKSpec_Joint::Execute( i3PhysixShapeSet * pShapeSet )
{
	INT32 rv;

	m_pShapeSet = pShapeSet;

	i3TDK::SetResInstance();

	rv = DoModal();

	i3TDK::RestoreResInstance();

	if( rv == IDCANCEL )
		return FALSE;

	return TRUE;
}


void i3TDKSpec_Joint::AssignJointInfo()
{
	TCHAR szTemp[256];

	if( m_pShapeSet != nullptr && m_pShapeSet->getJointInfo() != nullptr )
	{
		i3PhysixJointInfoD6 * pJointInfo = (i3PhysixJointInfoD6*)m_pShapeSet->getJointInfo();
		i3::sprintf( szTemp, _T("%p"), pJointInfo);
		m_EdtJointNameCtrl.SetWindowText( szTemp);
		m_EdtJointNameCtrl.EnableWindow( FALSE );

		switch( pJointInfo->xMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		i3::safe_string_copy( szTemp, _T("Locked"), _countof(szTemp));	break;
		case NX_D6JOINT_MOTION_LIMITED :	i3::safe_string_copy( szTemp, _T("Limited"), _countof(szTemp));	break;
		case NX_D6JOINT_MOTION_FREE :		i3::safe_string_copy( szTemp, _T("Free"), _countof(szTemp));	break;
		default : i3::safe_string_copy( szTemp, _T("unknown"), _countof(szTemp) );	break;
		}
		m_EdtMotionXCtrl.SetWindowText( szTemp);

		switch( pJointInfo->xMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		i3::safe_string_copy( szTemp, _T("Locked"), _countof(szTemp) );	break;
		case NX_D6JOINT_MOTION_LIMITED :	i3::safe_string_copy( szTemp, _T("Limited"), _countof(szTemp) );	break;
		case NX_D6JOINT_MOTION_FREE :		i3::safe_string_copy( szTemp, _T("Free") , _countof(szTemp) );	break;
		default : i3::safe_string_copy( szTemp, _T("unknown"), _countof(szTemp) );	break;
		}
		m_EdtMotionXCtrl.SetWindowText( szTemp);

		switch( pJointInfo->yMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		i3::safe_string_copy( szTemp, _T("Locked") , _countof(szTemp) );	break;
		case NX_D6JOINT_MOTION_LIMITED :	i3::safe_string_copy( szTemp, _T("Limited"), _countof(szTemp) );	break;
		case NX_D6JOINT_MOTION_FREE :		i3::safe_string_copy( szTemp, _T("Free") , _countof(szTemp) );	break;
		default : i3::safe_string_copy( szTemp, _T("unknown"), _countof(szTemp) );	break;
		}
		m_EdtMotionYCtrl.SetWindowText( szTemp);

		switch( pJointInfo->zMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		i3::safe_string_copy( szTemp, _T("Locked"), _countof(szTemp));	break;
		case NX_D6JOINT_MOTION_LIMITED :	i3::safe_string_copy( szTemp, _T("Limited"), _countof(szTemp));	break;
		case NX_D6JOINT_MOTION_FREE :		i3::safe_string_copy( szTemp, _T("Free"), _countof(szTemp) );	break;
		default : i3::safe_string_copy( szTemp, _T("unknown"), _countof(szTemp));	break;
		}
		m_EdtMotionZCtrl.SetWindowText( szTemp);

		switch( pJointInfo->swing1Motion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		i3::safe_string_copy( szTemp, _T("Locked"), _countof(szTemp) );	break;
		case NX_D6JOINT_MOTION_LIMITED :	i3::safe_string_copy( szTemp, _T("Limited"), _countof(szTemp));	break;
		case NX_D6JOINT_MOTION_FREE :		i3::safe_string_copy( szTemp, _T("Free"), _countof(szTemp) );	break;
		default : i3::safe_string_copy( szTemp, _T("unknown"), _countof(szTemp));	break;
		}
		m_EdtSwing1MotionCtrl.SetWindowText( szTemp);

		switch( pJointInfo->swing2Motion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		i3::safe_string_copy( szTemp, _T("Locked"), _countof(szTemp) );	break;
		case NX_D6JOINT_MOTION_LIMITED :	i3::safe_string_copy( szTemp, _T("Limited"), _countof(szTemp));	break;
		case NX_D6JOINT_MOTION_FREE :		i3::safe_string_copy( szTemp, _T("Free"), _countof(szTemp) );	break;
		default : i3::safe_string_copy( szTemp, _T("unknown"), _countof(szTemp));	break;
		}
		m_EdtSwing2MotionCtrl.SetWindowText( szTemp);

		switch( pJointInfo->twistMotion )
		{
		case NX_D6JOINT_MOTION_LOCKED :		i3::safe_string_copy( szTemp, _T("Locked"), _countof(szTemp) );	break;
		case NX_D6JOINT_MOTION_LIMITED :	i3::safe_string_copy( szTemp, _T("Limited"), _countof(szTemp));	break;
		case NX_D6JOINT_MOTION_FREE :		i3::safe_string_copy( szTemp, _T("Free"), _countof(szTemp) );	break;
		default : i3::safe_string_copy( szTemp, _T("unknown"), _countof(szTemp));	break;
		}
		m_EdtTwistMotionCtrl.SetWindowText( szTemp);
		// linear limit
		i3::ftoa( pJointInfo->linearLimit.value, szTemp);
		m_EdtLinearLimitValueCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->linearLimit.restitution, szTemp);
		m_EdtLinearLimitRestitutionCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->linearLimit.spring, szTemp);
		m_EdtLinearLimitSpringCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->linearLimit.damping, szTemp);
		m_EdtLinearLimitDampingCtrl.SetWindowText( szTemp);
		// swing1 limit
		i3::ftoa( pJointInfo->swing1Limit.value, szTemp);
		m_EdtSwing1LimitValueCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->swing1Limit.restitution, szTemp);
		m_EdtSwing1LimitRestitutionCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->swing1Limit.spring, szTemp);
		m_EdtSwing1LimitSpringCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->swing1Limit.damping, szTemp);
		m_EdtSwing1LimitDampingCtrl.SetWindowText( szTemp);
		// swing2 limit
		i3::ftoa( pJointInfo->swing2Limit.value, szTemp);
		m_EdtSwing2LimitValueCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->swing2Limit.restitution, szTemp);
		m_EdtSwing2LimitRestitutionCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->swing2Limit.spring, szTemp);
		m_EdtSwing2LimitSpringCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->swing2Limit.damping, szTemp);
		m_EdtSwing2LimitDampingCtrl.SetWindowText( szTemp);
		// twist high limit
		i3::ftoa( pJointInfo->twistLimit.high.value, szTemp);
		m_EdtTwistLimitHighValueCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->twistLimit.high.restitution, szTemp);
		m_EdtTwistLimitHighRestitutionCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->twistLimit.high.spring, szTemp);
		m_EdtTwistLimitHighSpringCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->twistLimit.high.damping, szTemp);
		m_EdtTwistLimitHighDampingCtrl.SetWindowText( szTemp);

		// twist low limit
		i3::ftoa( pJointInfo->twistLimit.low.value, szTemp);
		m_EdtTwistLimitLowValueCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->twistLimit.low.restitution, szTemp);
		m_EdtTwistLimitLowRestitutionCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->twistLimit.low.spring, szTemp);
		m_EdtTwistLimitLowSpringCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->twistLimit.low.damping, szTemp);
		m_EdtTwistLimitLowDampingCtrl.SetWindowText( szTemp);

		i3::ftoa( pJointInfo->gearRatio, szTemp);
		m_EdtGearRatioCtrl.SetWindowText( _T("0.0") );
	}
	else
	{
		m_EdtJointNameCtrl.SetWindowText( _T("(none)"));
		m_EdtJointNameCtrl.EnableWindow( FALSE );
		m_EdtMotionXCtrl.SetWindowText( _T("unknown"));
		m_EdtMotionYCtrl.SetWindowText( _T("unknown"));
		m_EdtMotionZCtrl.SetWindowText( _T("unknown"));
		m_EdtSwing1MotionCtrl.SetWindowText( _T("unknown"));
		m_EdtSwing2MotionCtrl.SetWindowText( _T("unknown"));
		m_EdtTwistMotionCtrl.SetWindowText( _T("unknown"));

		m_EdtLinearLimitValueCtrl.SetWindowText( _T("0.0"));
		m_EdtLinearLimitRestitutionCtrl.SetWindowText( _T("0.0"));
		m_EdtLinearLimitSpringCtrl.SetWindowText( _T("0.0"));
		m_EdtLinearLimitDampingCtrl.SetWindowText( _T("0.0"));
		m_EdtSwing1LimitValueCtrl.SetWindowText( _T("0.0"));
		m_EdtSwing1LimitRestitutionCtrl.SetWindowText( _T("0.0"));
		m_EdtSwing1LimitSpringCtrl.SetWindowText( _T("0.0"));
		m_EdtSwing1LimitDampingCtrl.SetWindowText( _T("0.0"));
		m_EdtSwing2LimitValueCtrl.SetWindowText( _T("0.0"));
		m_EdtSwing2LimitRestitutionCtrl.SetWindowText( _T("0.0"));
		m_EdtSwing2LimitSpringCtrl.SetWindowText( _T("0.0"));
		m_EdtSwing2LimitDampingCtrl.SetWindowText( _T("0.0"));

		m_EdtGearRatioCtrl.SetWindowText( _T("0.0") );
	}

}

void i3TDKSpec_Joint::CheckMotion( NxD6JointMotion * pMotion, const TCHAR* pStr )
{
	if( pStr == nullptr )
		return;

	if( i3::generic_is_iequal( pStr, _T("Locked") )  )
	{
		(*pMotion) = NX_D6JOINT_MOTION_LOCKED;
	}
	else if( i3::generic_is_iequal( pStr, _T("Limited") ) )
	{
		(*pMotion) = NX_D6JOINT_MOTION_LIMITED;
	}
	else if( i3::generic_is_iequal( pStr, _T("Free") ) )
	{
		(*pMotion) = NX_D6JOINT_MOTION_FREE;
	}
	else
	{
		// 잘못입력
		CDialog dlg;
		dlg.SetWindowText( _T("Invalid element. You must select next word -> (Locked, Limited, Free)") );
	}
}

void i3TDKSpec_Joint::OnEnChangeEditMotionX()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtMotionXCtrl.GetWindowText( conv, _countof(conv) );

	CheckMotion( &pInfo->xMotion, conv );
}

void i3TDKSpec_Joint::OnEnChangeEditMotionY()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtMotionYCtrl.GetWindowText( conv, _countof(conv) );

	CheckMotion( &pInfo->yMotion, conv );
}

void i3TDKSpec_Joint::OnEnChangeEditMotionZ()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtMotionZCtrl.GetWindowText( conv, _countof(conv) );

	CheckMotion( &pInfo->zMotion, conv );
}

void i3TDKSpec_Joint::OnEnChangeEditMotionSwing1()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing1MotionCtrl.GetWindowText( conv, _countof(conv) );

	CheckMotion( &pInfo->swing1Motion, conv );
}

void i3TDKSpec_Joint::OnEnChangeEditMotionSwing2()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing2MotionCtrl.GetWindowText( conv, _countof(conv) );

	CheckMotion( &pInfo->swing2Motion, conv );
}

void i3TDKSpec_Joint::OnEnChangeEditMotionTwist()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtTwistMotionCtrl.GetWindowText( conv, _countof(conv) );

	CheckMotion( &pInfo->twistMotion, conv );
}

void i3TDKSpec_Joint::OnEnChangeEditLinearlimitValue()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtLinearLimitValueCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->linearLimit.value = value;
}

void i3TDKSpec_Joint::OnEnChangeEditLinearlimitRestitution()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtLinearLimitRestitutionCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->linearLimit.restitution = value;
}

void i3TDKSpec_Joint::OnEnChangeEditLinearlimitSpring()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtLinearLimitSpringCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->linearLimit.spring = value;
}

void i3TDKSpec_Joint::OnEnChangeEditLinearlimitDamping()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtLinearLimitDampingCtrl.GetWindowText( conv, 256 );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->linearLimit.damping = value;
}

void i3TDKSpec_Joint::OnEnChangeEditSwing1limitValue()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing1LimitValueCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->swing1Limit.value = value;
}

void i3TDKSpec_Joint::OnEnChangeEditSwing1limitRestitution()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing1LimitRestitutionCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->swing1Limit.restitution = value;
}

void i3TDKSpec_Joint::OnEnChangeEditSwing1limitSpring()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing1LimitSpringCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->swing1Limit.spring = value;
}

void i3TDKSpec_Joint::OnEnChangeEditSwing1limitDamping()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing1LimitDampingCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->swing1Limit.damping = value;
}

void i3TDKSpec_Joint::OnEnChangeEditSwing2limitValue()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing2LimitValueCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->swing2Limit.value = value;
}

void i3TDKSpec_Joint::OnEnChangeEditSwing2limitRestitution()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing2LimitRestitutionCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->swing2Limit.restitution = value;
}

void i3TDKSpec_Joint::OnEnChangeEditSwing2limitSpring()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing2LimitSpringCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->swing2Limit.spring = value;
}

void i3TDKSpec_Joint::OnEnChangeEditSwing2limitDamping()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtSwing2LimitDampingCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->swing2Limit.damping = value;
}

void i3TDKSpec_Joint::OnEnChangeEditTwistlimitHighValue()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtTwistLimitHighValueCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->twistLimit.high.value = value;
}

void i3TDKSpec_Joint::OnEnChangeEditTwistlimitHighRestitution()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtTwistLimitHighRestitutionCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->twistLimit.high.restitution = value;
}

void i3TDKSpec_Joint::OnEnChangeEditTwistlimitHighSpring()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtTwistLimitHighSpringCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->twistLimit.high.spring = value;
}

void i3TDKSpec_Joint::OnEnChangeEditTwistlimitHighDamping()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtTwistLimitHighDampingCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->twistLimit.high.damping = value;
}

void i3TDKSpec_Joint::OnEnChangeEditTwistlimitLowValue()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtTwistLimitLowValueCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->twistLimit.low.value = value;
}

void i3TDKSpec_Joint::OnEnChangeEditTwistlimitLowRestitution()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtTwistLimitLowRestitutionCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->twistLimit.low.restitution = value;
}

void i3TDKSpec_Joint::OnEnChangeEditTwistlimitLowSpring()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtTwistLimitLowSpringCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->twistLimit.low.spring = value;
}

void i3TDKSpec_Joint::OnEnChangeEditTwistlimitLowDamping()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtTwistLimitLowDampingCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->twistLimit.low.damping = value;
}

void i3TDKSpec_Joint::OnEnChangeEditGearratio()
{
	if( m_pShapeSet == nullptr )
		return;

	i3PhysixJointInfoD6 * pInfo = (i3PhysixJointInfoD6*) m_pShapeSet->getJointInfo();

	if( pInfo == nullptr )
		return;

	TCHAR conv[256];

	m_EdtGearRatioCtrl.GetWindowText( conv, _countof(conv) );

	REAL32 value = (REAL32) i3::atof( conv);
	pInfo->gearRatio = value;
}
