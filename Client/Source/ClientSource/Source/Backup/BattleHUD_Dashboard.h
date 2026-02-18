#if !defined( __BATTLE_HUD_DASHBOARD_H)
#define __BATTLE_HUD_DASHBOARD_H

#if 지워질_것들

#include "BattleHUD_Define.h"
#include "BattleGUI_Define.h"
#include "MovingText.h"

enum HUD_TEXT
{
	HUD_TEXT_TIME = 0,
	HUD_TEXT_COLON,
	HUD_TEXT_INFO_RED,
	HUD_TEXT_INFO_BLUE,	
	HUD_TEXT_INFO_GOAL,

	HUD_TEXT_COUNT,
};

enum HUD_BATTLE_INFO
{
	HUD_BATTLE_INFO_SCORE = 0,

	HUD_BATTLE_INFO_TEAM_RED,					// 자신이 속한팀의 표시 : RED
	HUD_BATTLE_INFO_TEAM_BLUE,				// 자신이 속한팀의 표시	: BLUE
	HUD_BATTLE_INFO_CLAN,
	HUD_BATTLE_INFO_TIME,
	HUD_BATTLE_INFO_LIMIT,
	HUD_BATTLE_INFO_ESCAPEARROW,				//탈출미션 공격자 방향 표시
	
	HUD_BATTLE_INFO_COUNT,
};

enum HUD_BATTLE_INFO_DINO
{
	HUD_BATTLE_INFO_ESCAPESHILD_0,				//탈출미션 디펜스 빈방패
	HUD_BATTLE_INFO_ESCAPESHILD_1,				//탈출미션 디펜스 반방패
	HUD_BATTLE_INFO_ESCAPESHILD_2,				//탈출미션 디펜스 풀방패
	HUD_BATTLE_INFO_ESCAPESHILD_3,				//탈출미션 디펜스 풀방패
	HUD_BATTLE_INFO_ESCAPESHILD_4,				//탈출미션 디펜스 풀방패

	HUD_BATTLE_INFO_DINO_COUNT,


};


class CHud;

class CHUD_Dashboard : public i3GameObjBase
{
	I3_CLASS_DEFINE( CHUD_Dashboard, i3GameObjBase);
protected:
	CHud *			m_pHUD;
	

	CMovingText*	m_pMovingText;
	// TopScore 관련
	INT32			m_RedWinScore;				// RED Team의 승리 점수. MOD에 따라 KillCount일 수도, Round 수 일수도 있다.
	INT32			m_BlueWinScore;				// BLUE Team의 승리 점수. MOD에 따라 KillCount일 수도, Round 수 일수도 있다.

	i3Texture *		m_pTex;
	i3Texture *		m_pTexDino;
	i3Sprite2D *	m_pSpriteBattleInfo;
	i3Sprite2D *	m_pSpriteBattleInfoDino;
	i3TextNodeDX2 *	m_pText[HUD_TEXT_COUNT];

	


protected:
	void			UpdateTime();
	void			UpdateRound();

	REAL32			m_updateTimer;
	INT32			m_iRedScore;
	INT32			m_iBlueScore;

	INT32			m_iTempRedScore;
	INT32			m_iTempBlueScore;

public:
	CHUD_Dashboard(void);
	virtual ~CHUD_Dashboard(void);

	virtual bool	Create( CHud * pHUD);
	virtual void	Init(void);

	virtual void	Update( REAL32 tm);

	void			SetScore( INT32 red, INT32 blue);
	void			SetEscapeScore(INT32 RedScore, INT32 BlueScore);
	virtual void	SetEnableWithoutNotice( BOOL bFlag, GAME_GUI_STATUS nState, BOOL bDeath);
};

#endif

#endif
