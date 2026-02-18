#include "pch.h"
#include "GameDinoActionContext.h"

#include "Character/GameDinoContexts.h"

#include "Character/GameCharaActionControl1PV.h"
#include "Character/GameCharaActionControl3PV.h"

I3_CLASS_INSTANCE( CGameDinoActionContext);

void CGameDinoActionContext::_Play_Dino_Roar( void)
{
	if( !isLower( CHARA_LOWER_ROAR ))
		m_pCurrentActionControl->OnActionDinoRoar();
}

void CGameDinoActionContext::_Play_Dino_Idle( void)
{
	if( !isLower( CHARA_LOWER_WAIT ))
		m_pCurrentActionControl->OnActionDinoIdle();
}
void CGameDinoActionContext::_Play_Dino_Stun( void)
{
	if( !isLower( CHARA_LOWER_STUN ))
		m_pCurrentActionControl->OnActionDinoStun();
}