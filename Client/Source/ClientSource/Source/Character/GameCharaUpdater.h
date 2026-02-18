/**
 * \file	Character\GameCharaUpdater.h
 *
 * Declares the game chara updater class.
 */


#if !defined( __GAME_CHARA_UPDATER_H__)
#define __GAME_CHARA_UPDATER_H__

#include "GameCharaControlBase.h"
#include "../Weapon/WeaponBase.h"

class CGameCharaUpdater : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaUpdater, CGameCharaControlBase );
protected:
	bool			m_bObserverStart = false;		// 방장이 GM옵저버일 경우 피직스와 싱크가 맞지 않아 한프레임 늦쳐줘야 한다. 그때 필요한 변수

public:	
	void			DoFirstUpdate( REAL32 rDeltatime);
	virtual void	OnFirstUpdate( REAL32 rDeltatime);
	virtual void	OnFirstUpdateFullbody(REAL32 tm);
	void			DoUpdate( REAL32 rDeltatime);
	virtual void	OnUpdate( REAL32 rDeltatime);
	void			DoLastUpdate( REAL32 rDeltatime);
	virtual void	OnLastUpdate( REAL32 rDeltatime);

	virtual void	OnProcessInvincible( REAL32 rDeltatime);
	virtual void	OnProcessJump( REAL32 rDeltatime);

	/** \brief Packet을 받으면 처리하는 함수
		\note for Only Local */
	void	ProcessLocalFromRecvPacket( void);

	/** \brief Respawn을 계산합니다. */
	void	ProcessRespawn( REAL32 rDeltatime);

	/** \brief 물리 효과 끄기에 대한 계산을 합니다. */
	void	ProcessPhysixSleep( REAL32 rDeltatime);

	/** \brief 물리 효과 끄기에 대한 계산을 합니다.
		\note Object위에서 죽은 경우 처리 */
	void	ProcessPhysixSleepForOnObject( REAL32 rDeltatime);

	/** \brief 물리 효과중 보정 처리를 합니다. */
	void	ProcessRecalcPhysixPos( REAL32 rDeltatime);

	/** \brief 죽은 경우 처리합니다. */
	void	ProcessDeathState( REAL32 rDeltatime);

	/** \brief 캐릭터의 이동 설정을 하는 Debug code */
	void	DebugProcess( REAL32 rDeltatime);

	/** \brief Graze sound를 검사합니다. */
	void	CheckGrazeSoundPlay(void);

	/** \brief Graze sound를 처리합니다. */
	void	GrazeSoundPlay(const i3::vector<WeaponRocketLauncherShell*>& List);

	virtual void	Reset(void)		{ m_bObserverStart	= false; }
};

#endif
