#if !defined( __BATTLE_GUI_BOSS_SCORE_H)
#define __BATTLE_GUI_BOSS_SCORE_H

#include "BattleHUD_Define.h"

class CBattleGui;
class CHud;

#define		BOSS_TIP_CNT		7

class CBattleGUI_BOSS_Score : public i3GameObjBase
{
	I3_CLASS_DEFINE( CBattleGUI_BOSS_Score, i3GameObjBase);
public:
	enum RANK_EDBOX
	{
		RANK_EDBOX_RANK,
		RANK_EDBOX_NICK,
		RANK_EDBOX_KILLCOUNT,
		RANK_EDBOX_POINT,

		RANK_EDBOX_COUNT
	};

	enum QUEST_EDBOX
	{
		QUEST_EDBOX_MSG = 0,

		QUEST_EDBOX_COUNT
	};

protected:
	CBattleGui *	m_pGUI;
	bool			m_bVisible;

	i3GuiStatic *	m_pPanel;

	i3GuiEditBox *	m_pED_Boss[ DINO_TOTAL_CNT][RANK_EDBOX_COUNT];
	i3GuiEditBox *	m_pED_Union[ UNION_TOTAL_CNT][RANK_EDBOX_COUNT];
	
	i3Texture *		m_pTex;
	i3GuiStatic *	m_pST_RankMark[ MAX_ALL_USER];
	i3GuiStatic *	m_pST_Clan[ MAX_ALL_USER];
	
	i3GuiStatic *	m_pST_Ping[MAX_ALL_USER];

	const char *	m_pTip[BOSS_TIP_CNT];
	i3GuiEditBox *	m_pED_Tip;

	i3GuiEditBox *	m_pED_ServerInfo;

	bool			m_bFirstRound;

protected:
	void			_InitText( i3GuiControl * pCtrl);
	i3GuiEditBox *	_InitEditBox( const char * pszPath, INT32 maxLengh, INT32 pt, INT32 alignX, INT32 alignY, const char * pszText = NULL);
	void			_UpdateScore(void);

	void			_InitPing(i3Texture* pTexture);
	void			_ShowPing( INT32 nSlot, INT32 idxNet);
	void			_ShowHostMark(INT32 nSlot);
	void			_ShowClanMark(INT32 nSlot, SLOT_INFO * pSlotInfo);
	void			_HideClanMark(void);
	void			_HidePingAll(void);
	void			_ShowRankMark( INT32 nSlot,INT32 nRank);
	void			_HideRankMarkAll(void);

	void			_SetTipText(void);

public:
	CBattleGUI_BOSS_Score(void);
	virtual ~CBattleGUI_BOSS_Score(void);

	bool			isVisible(void)				{ return m_bVisible; }

	void			Create( CBattleGui * pGUI);
	void			ShowScore( bool bVisible, REAL32 tm);

	virtual void	SetEnableGUI( bool bFlag, GAME_GUI_STATUS nState, BOOL bDeath);
};

#endif
