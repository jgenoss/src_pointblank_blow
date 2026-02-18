#include "pch.h"
#include "PBCmd_PenetrateWall.h"

#include "../../../BattleSlotContext.h"
#include "../../../GameCharaMoveContext.h"

/*virtual*/ CMD_RESULT_FLAG PBCmd_PenetrateWall::RunCommand(REAL32 DeltaSec)
{
	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	if( pMyChara != nullptr)
	{
		if( GetBoolean() == true)
			pMyChara->getMoveContext()->addMoveFlag( CHARA_MOVE_FLAG_PENETRATEWALL);
		else
			pMyChara->getMoveContext()->removeMoveFlag( CHARA_MOVE_FLAG_PENETRATEWALL);
	}
	return CMD_RESULT_OK;
}