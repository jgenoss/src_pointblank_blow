#include "pch.h"

#include "GameCharaDinoRaptorMercury.h"
#include "GameCharaContexts.h"
#include "StageBattle.h"
#include "Crosshair_Dino_Raptor.h"
#include "WeaponDino.h"
#include "CrosshairMgr.h"
#include "GameCharaContexts.h"
#include "UI/UIHudManager.h"
#include "UI/UIHudDinoState.h"


I3_CLASS_INSTANCE( CGameCharaDinoRaptorMercury);//, CGameCharaDinoCCRaptor);


void CGameCharaDinoRaptorMercury::_CreateDino( void)
{
	CGameCharaDino::_CreateDino();
	m_idxRaptorBuffEffect = g_pEffectManager->FindEffect( "TANK_ATTACK_B");
}