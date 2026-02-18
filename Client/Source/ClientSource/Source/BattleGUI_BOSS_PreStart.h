#if !defined( __BATTLE_GUI_BOSS_PRESTART_H)
#define __BATTLE_GUI_BOSS_PRESTART_H

#include "BattleHUD_Define.h"

class CBattleGui;
class CHud;

class CBattleGUI_BOSS_PreStart : public i3GameObjBase
{
	I3_CLASS_DEFINE( CBattleGUI_BOSS_PreStart, i3GameObjBase);
public:

protected:
	bool				m_bVisible;

	i3GuiStatic *		m_pPanel;

	i3GuiStatic *		m_pST_BossTeam;
	i3GuiStatic *		m_pST_UnionTeam;
	i3GuiProgressBar *	m_pST_Gauge;

	i3GuiStatic * m_pBossTopViewGuiStatic;
	i3GuiStatic * m_pBossExitGuiStatic;

protected:
	i3GuiEditBox *	_InitEditBox( const char * pszPath, INT32 maxLengh, INT32 pt, INT32 alignX, INT32 alignY, const i3::wliteral_range& wstrText);

public:
	CBattleGUI_BOSS_PreStart(void);
	virtual ~CBattleGUI_BOSS_PreStart(void);

	bool			isVisible(void)				{ return m_bVisible; }

	virtual bool	Create( CBattleGui * pGUI);
	virtual void	Init(void);
	virtual void	SetEnableGUI( bool bFlag, GAME_GUI_STATUS nState, bool bDeath);
};

#endif
