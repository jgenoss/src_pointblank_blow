#include "pch.h"
#include "GameCharaActionControl3PVLocal.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaActionControl3PVLocal, CGameCharaActionControl3PV);

CGameCharaActionControl3PVLocal::CGameCharaActionControl3PVLocal()
{
}

CGameCharaActionControl3PVLocal::~CGameCharaActionControl3PVLocal()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Action

//bool CGameCharaActionControl3PVLocal::OnActionStandup( void)
//{
//	return _OnCmdActionStandUpForLocal();
//}
//
//bool CGameCharaActionControl3PVLocal::OnActionJumpStart( void)
//{
//	return OnActionJumpStartLocal();
//}


//bool CGameCharaActionControl3PVLocal::OnActionExtensionWeapon( bool bActivate)
//{	//	ActionParameter가 필요한 함수입니다.
//	if( !CGameCharaActionControl3PV::OnActionExtensionWeapon( bActivate) )
//		return false;
//
//	m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);	//	발사 준비중에 발사하지 못하도록. 애니메이션이 끝나면 발사 가능
//
//#if defined( I3_DEBUG )//디버깅 용도로 만들어진것 같음
//	if( m_pOwner->is1PV())
//	{
//		if( bActivate)
//			g_pFramework->getChatBox()->PutSystemChatting( "Extension On");
//		else
//			g_pFramework->getChatBox()->PutSystemChatting( "Extension Off");
//	}
//#endif
//
//	return true;
//}


