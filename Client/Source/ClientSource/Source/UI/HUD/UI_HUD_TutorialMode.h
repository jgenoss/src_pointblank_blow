#if !defined( __UI_HUD_TUTORIALMODE_H__)
#define __UI_HUD_TUTORIALMODE_H__

#include "../UIBase.h"

enum TUTORIAL_HUD_TYPE
{
	THT_KEY_MOVE,
	THT_KEY_JUMP,
	THT_KEY_SHOOT,
	THT_KEY_EXT,
	THT_KEY_GRENADE_LAUNCHER,
	THT_KEY_GRENADE_SMOKE,
	THT_KEY_C4,
	THT_KEY_MELEE,
	THT_KEY_HELICOPTER,
	THT_MISSION_START_CLEAR,
	THT_GAME_RESULT,
	THT_COUNT
};

enum TUTORIAL_HUD
{
	TH_KEY_MOVE,
	TH_KEY_JUMP,
	TH_KEY_SHOOT,
	TH_KEY_EXT,
	TH_KEY_GRENADE_LAUNCHER,
	TH_KEY_GRENADE_SMOKE,
	TH_KEY_C4,
	TH_KEY_MELLE,
	TH_KEY_HELICOPTER,
	TH_MISSION_START,
	TH_MISSION_CLEAR,
	TH_GAME_OVER,
	TH_RESULT,
	TH_COUNT
	
};

class UI_HUD_Tutorial : public UIBase
{
	I3_CLASS_DEFINE( UI_HUD_Tutorial, UIBase);

private:
	i3UIScene* m_pSceneHUD_Tutorial[THT_COUNT];
protected:
	void			_InitFont();
public:
	UI_HUD_Tutorial();
	virtual ~UI_HUD_Tutorial();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual void	OnUpdate( REAL32 rDeltaSeconds);
	void			OffAllTutorialHUD();
	void			OnOffTutorialHUD(TUTORIAL_HUD eHud, bool bFlag);
	void			TurnOnResultHUD(INT32 iExp, INT32 iPoint, UINT32 uiTime);
};

#endif // __UI_HUD_TUTORIALMODE_H__
