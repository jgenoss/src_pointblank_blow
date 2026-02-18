#if !defined( __UI_BATTLE_POPUP_EXIT_H__)
#define __UI_BATTLE_POPUP_EXIT_H__

#include "UIBattlePopupBase.h"

class UIBattlePopupExit : public UIBattlePopupBase
{
	I3_CLASS_DEFINE( UIBattlePopupExit, UIBattlePopupBase);

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void OnLoadAllScenes() override;
	virtual bool OnKey_Enter( void) override;

	void	ClickOption( void);
	void	ClickReport( void);
	void	ClickLeaveBattle( void);
	void	AttachToTopScreen_ForceRemove();
};

#endif
