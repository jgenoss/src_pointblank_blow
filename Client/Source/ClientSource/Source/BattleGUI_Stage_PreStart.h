#if !defined( __BATTLE_GUI_STAGE_PRESTART_H)
#define __BATTLE_GUI_STAGE_PRESTART_H

#if legacy_gui_scene

#include "BattleGUI_BOSS_PreStart.h"

class CBattleGui;
class CHud;

class CBattleGUI_Stage_PreStart : public CBattleGUI_BOSS_PreStart
{
	I3_CLASS_DEFINE( CBattleGUI_Stage_PreStart, CBattleGUI_BOSS_PreStart);
public:

protected:
	i3GuiEditBox *	_InitEditBox( const char * pszPath, INT32 maxLengh, INT32 pt, INT32 alignX, INT32 alignY, const char * pszText = nullptr);

	i3GuiStatic * m_pTitleViewGuiStatic;
	i3Texture * m_pTextureHUD3;

public:
	CBattleGUI_Stage_PreStart(void);
	virtual ~CBattleGUI_Stage_PreStart(void);

	virtual bool	Create( CBattleGui * pGUI) override;
	virtual void	Init(void) override;
	virtual void	SetEnableGUI( bool bFlag, GAME_GUI_STATUS nState, bool bDeath) override;
};

#endif

#endif
