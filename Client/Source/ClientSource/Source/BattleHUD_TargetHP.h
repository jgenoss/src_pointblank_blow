#pragma once

#include "BattleHUD_Define.h"
#include "BattleGUI.h"

enum HUD_TARGETHP_SPR
{
	HUD_TARGETHP_SPR_HPBAR_BG_RED = 0,
	HUD_TARGETHP_SPR_HPBAR_BG_BLUE,

	HUD_TARGETHP_SPR_HPBAR_RED,
	HUD_TARGETHP_SPR_HPBAR_BLUE,

	HUD_TARGETHP_SPR_HPBAR_WARING_RED,
	HUD_TARGETHP_SPR_HPBAR_WARING_BLUE,

	HUD_TARGETHP_SPR_OBJECTICON_RED,
	HUD_TARGETHP_SPR_OBJECTICON_BLUE,

	HUD_TARGETHP_SPR_OBJECT_IMG_BLUE,
	HUD_TARGETHP_SPR_OBJECT_IMG_RED,

	HUD_TARGETHP_SPR_HPSTATE_COUNT,
};

struct DETECTED_ENEMY_UPDATE_INFO
{
	bool	bEnableUpdate;
	REAL32	LocalElapsedTime;
	REAL32	TotalUpdateTime;

	void	Init(void)
	{
		bEnableUpdate		= false;
		LocalElapsedTime	= 0.0f;
		TotalUpdateTime		= 0.0f;
	}
};

class CHud;
class CHUD_TargetHP : public i3GameObjBase
{
	I3_CLASS_DEFINE( CHUD_TargetHP, i3GameObjBase);

public:
	CHUD_TargetHP(void);
	~CHUD_TargetHP(void);

public:
	static CHUD_TargetHP* instance();

public:
	bool			Create( CHud * pHUD);
	// Create용 Init
	void			Init(void);
	// Round용 Reset
	void			Reset( void);
	void			SetEnableTargetHP( bool bEnable);
	void			SetDetectedEnemyInfo( INT32 idx);
	void			SetEnableWithoutNotice( bool bFlag, GAME_GUI_STATUS nState, bool bDeath);

	void			Update( REAL32 tm);

private:
	void			_UpdateUnderAttack( REAL32 tm);
	void			_SetTargetHPWarning( INT32 idx);
	void			_UpdateTargetPos(void);				//파괴할 목표물의 위치를 표시해준다아~
	bool			_CalcProjectionPos( REAL32 * fX, REAL32 * fY, VEC3D * vIn);
	void			_UpdateTargetHP_DestroyMission( REAL32 tm);
	void			_UpdateTargetHP_DefenceMission( REAL32 tm);

	void			_UpdateDetectedEnemySprite( REAL32 tm);
	void			_UpdateWeakedEnemySprite( REAL32 tm);
	void			_UpdateTargetHP_BossMission( REAL32 tm);

	void			CreateDetectedEnemySprite(void);
	void			CreateWeakedEnemySprite(void);

	INT32			_GetCurrentSlotIndex( void);
	CGameCharaBase *	_GetCurrentChara( void);

private:
	CHud *			m_pHUD;

	i3Texture *		m_pTex;
	i3Sprite2D *	m_pSprite;

	VEC3D			m_vDestroyObjectCenterPos[2];

	// Breakdown Mission Object UnderAttack Warning
	bool			m_bUnderAttackWarn[ 2];
	REAL32			m_rUnderAttackWarnElapsed[ 2];
	REAL32			m_rUnderAttackWarnElapsedTotal[ 2];

	i3Sprite2D *	m_pSpriteDetectedEnemy;
	i3Sprite2D *	m_pSpriteWeakedEnemy;

	DETECTED_ENEMY_UPDATE_INFO	m_DEUI[MAX_TEAM_USER];	//Detected Emeny Update Info
	DETECTED_ENEMY_UPDATE_INFO	m_DEUI_Weak[MAX_ALL_USER];	//Detected Emeny Update Info

	REAL32			m_OldHP_RED;
	REAL32			m_OldHP_BLUE;
	bool m_bInitObjectHP;
	bool m_bInitObjectSynchronize;

	bool			m_bPlayBGMforDefence;

	i3Texture *		m_pTextureHUD3;
	i3Sprite2D *	m_pSpriteDefence;
};
