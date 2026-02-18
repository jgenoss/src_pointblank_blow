#if !defined( __UI_BATTLE_POPUPBASE_H__)
#define __UI_BATTLE_POPUPBASE_H__

#include "UIPopupBase.h"

class UIBattleFrame;

class UIBattlePopupBase : public UIPopupBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIBattlePopupBase, UIPopupBase);

protected:
	void		_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void OnLoadAllScenes() override;
	virtual void OnUnloadAllScenes() override;

	UIBattleFrame *	GetUIBattleFrame( void);

	virtual void	_OnClose( void) override;

public:
	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f) override;

public:
	UIBattlePopupBase();
	virtual ~UIBattlePopupBase();
};

#endif
