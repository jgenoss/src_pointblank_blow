#include "pch.h"
#include "GameCharaActionControl1PVLocal.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaActionControl1PVLocal, CGameCharaActionControl1PV);

CGameCharaActionControl1PVLocal::CGameCharaActionControl1PVLocal()
{
	
}

CGameCharaActionControl1PVLocal::~CGameCharaActionControl1PVLocal()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Action

//bool CGameCharaActionControl1PVLocal::OnActionStandup( void)
//{
//	return _OnCmdActionStandUpForLocal();
//}


//bool CGameCharaActionControl1PVLocal::OnActionRunLower( VEC3D * pDir)
//{
//	EXT_ASSERT( pDir != NULL);
//
//	if( i3Vector::IsSame( pDir, &I3_ZEROVECTOR) )
//		return false;
//
//	////////////////////////////////////////////////////////////////////
//	REAL32 fScaleTime = 1.f;
//	ID_LOWER_AI nId = getAnimContext()->getMoveIdByDir( pDir);
//	CHARA_ACTION_LOWER state = CHARA_LOWER_RUN;
//	REAL32 fVel = getMoveContext()->getSpeed();
//	
//	if( nId != ID_LOWER_IDLE )
//	{
//		fVel = i3Vector::Length( getMoveContext()->getVelocity());
//		fScaleTime = _CalcRunLowerScaleTime( nId, fVel);
//	}
//
//	if( (getAnimContext()->getOldLowerID() != nId) )
//	{
//		setLower( state);
//		_Animation_Lower( nId, 0.f, fScaleTime);
//	}
//
//	return true;
//}


//bool CGameCharaActionControl1PVLocal::OnActionJumpStart( void)
//{
//	return OnActionJumpStartLocal();
//}


//bool CGameCharaActionControl1PVLocal::OnActionExtensionWeapon( bool bActivate)
//{
//	if( !CGameCharaActionControl1PV::OnActionExtensionWeapon( bActivate))
//		return false;
//
//	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	발사 준비중에 발사하지 못하도록. 애니메이션이 끝나면 발사 가능
//
//	return true;
//}

