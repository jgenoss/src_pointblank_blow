#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaLocal.h"

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Anim Callback
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGameCharaNetwork::OnAttackRun( i3AIContext * pCtx, REAL32 tm)
{	
	CGameCharaPlayer::OnAttackRun( pCtx, tm );

	//	기본 상태로만 돌아가게 해줍니다.
	if( getActionContext()->isLower( CHARA_LOWER_DEATH)) return;

	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	if( pCurWeapon == NULL )
		return;

	if( pCurWeapon->getState() != WEAPON_STATE_FIRE)
	{	
		if( m_rAttackIdleTime <= 0.f )
		{
			if( getActionContext()->isLower( CHARA_LOWER_MOVE) )
				ANIM_playUpper( CHARA_UPPER_MOVE, ID_UPPER_MOVE, GetSyncTimeLower() );				//	이전 Move 동작으로
			else
				//ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_IDLE );		//	이전 Idle 동작으로
				CALL_ACTION_COMMAND( ACTION_CMD_STEADYUPPER, NULL);		//	이전 Idle 동작으로
		}
		else if( pCurWeapon->getWeaponInfo()->m_TypeFire != WEAPON_FIRE_RANGE)
		{
			if( getActionContext()->isLower( CHARA_LOWER_MOVE) )
				ANIM_playUpper( CHARA_UPPER_MOVE, ID_UPPER_MOVE, GetSyncTimeLower() );				//	이전 Move 동작으로
			else
				//ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_IDLE );		//	이전 Idle 동작으로
				CALL_ACTION_COMMAND( ACTION_CMD_STEADYUPPER, NULL);		//	이전 Idle 동작으로
		}
	}
}

void CGameCharaNetwork::OnAttackFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameCharaPlayer::OnAttackFinish( pCtx, tm );

	//	Network 캐릭만 호출해야 함. Network만 적용됩니다
	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	if( pCurWeapon == NULL )
		return;
	
	removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE);
	
	pCurWeapon->OnEndFire( pCtx, tm);
}

void CGameCharaNetwork::OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm)
{
	//	산탄총 때문에 Network에서도 처리해야 합니다.
	CGameCharaPlayer::OnLoadMagazineRun( pCtx, tm);

	WeaponBase * pWeapon = getWeaponContext()->getCurrent();
	if( pWeapon == NULL )
		return;
	
	if( pWeapon->getState() == WEAPON_STATE_NEEDTO_LOADMAG )
	{
		ACTION_LoadMagazine();
	}
}

void CGameCharaNetwork::OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm)
{	
	CGameCharaPlayer::OnLoadBulletRun( pCtx, tm );
}
*/
