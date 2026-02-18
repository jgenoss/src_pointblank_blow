#if !defined( __BATTLE_HUD_DASHBOARD_BOSS_H)
#define __BATTLE_HUD_DASHBOARD_BOSS_H

#if 지워질_것들

#include "BattleHUD_Define.h"
#include "BattleGUI_Define.h"
#include "BattleHUD_Dashboard.h"

enum HUD_BSD_TEXT
{
	HUD_BSD_TEXT_TIME_MIN = 0,
	HUD_BSD_TEXT_TIME_SEC,
	HUD_BSD_TEXT_CUR_ROUND,
	HUD_BSD_TEXT_MAX_ROUND,	

	HUD_BSD_TEXT_COUNT,
};

enum HUD_BSD_SPR
{
	HUD_BSD_SPR_ROUND_BACK_LEFT = 0,
	HUD_BSD_SPR_ROUND_BACK_CENTER,
	HUD_BSD_SPR_ROUND_BACK_RIGHT,
	HUD_BSD_SPR_ROUND_SLASH,

	HUD_BSD_SPR_TIME_BACK_LEFT,
	HUD_BSD_SPR_TIME_BACK_CENTER,
	HUD_BSD_SPR_TIME_BACK_RIGHT,
	HUD_BSD_SPR_TIME_CLOCK,
	HUD_BSD_SPR_TIME_COLON,

	HUD_BSD_SPR_COUNT,
};


class CHud;

class CHUD_Dashboard_Boss : public CHUD_Dashboard
{
	I3_CLASS_DEFINE( CHUD_Dashboard_Boss, CHUD_Dashboard);
protected:
	i3Sprite2D *	m_pSprite;
	i3TextNodeDX2 *	m_pTxt[HUD_BSD_TEXT_COUNT];

protected:
	void			UpdateTime();

public:
	CHUD_Dashboard_Boss(void);
	virtual ~CHUD_Dashboard_Boss(void);

	virtual bool	Create( CHud * pHUD);
	virtual void	Init(void);

	virtual void	Update( REAL32 tm);

	virtual void	SetEnableWithoutNotice( BOOL bFlag, GAME_GUI_STATUS nState, BOOL bDeath);
};

#endif

#endif
