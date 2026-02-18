#include "pch.h"
#include "GameCharaUpdater1PV.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater1PV);

void CGameCharaUpdater1PV::Process1PVOnly( REAL32 rDeltatime)
{
	if( !m_pOwner->isNetwork())
	{
		const LadderStateInfo* ladderInfo = m_pOwner->getMoveContext()->getLadderStateInfo();

		if( getActionContext()->isLower( CHARA_LOWER_JUMP) || m_pOwner->isCharaStateMask( CHARA_STATE_JUMP) ||
		 ( ladderInfo == nullptr && !m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) ) )
		{
			getAnimContext()->CallbackAnim_JumpStartRun( nullptr, rDeltatime);
		}
		else if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) || ladderInfo != nullptr ) 
		{
			if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
			{
				getAnimContext()->CallbackAnim_MoveRun( nullptr, rDeltatime);
			}
		}
	}
	else
	{
		if( getActionContext()->isLower( CHARA_LOWER_JUMP) )
		{
			getAnimContext()->CallbackAnim_JumpStartRun( nullptr, rDeltatime);
		}
		else if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
		{
			getAnimContext()->CallbackAnim_MoveRun( nullptr, rDeltatime);
		}
	}
	
}

void CGameCharaUpdater1PV::OnUpdate( REAL32 rDeltatime)
{
	//	Add code here
	//	1인칭일 경우에 하체 aistate의경우는 호출되지 않으므로 필요한경우 강제 호출해주도록 하는 함수
	Process1PVOnly( rDeltatime);

	CGameCharaUpdater::OnUpdate( rDeltatime);

	//	사용자의 HUD 화면 이펙트 
	getHUDContext()->OnUpdate( rDeltatime);

	// RPG7 접근 사운드 플레이
	CheckGrazeSoundPlay();
}