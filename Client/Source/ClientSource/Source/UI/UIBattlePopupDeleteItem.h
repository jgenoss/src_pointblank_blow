#pragma once

#include "UIBattlePopupBase.h"

class UIBattlePopupDeleteItem : public UIBattlePopupBase
{
	I3_CLASS_DEFINE( UIBattlePopupDeleteItem, UIBattlePopupBase);

public:
	UIBattlePopupDeleteItem();
	virtual ~UIBattlePopupDeleteItem();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape(void) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

public:
	void clicked(int idx);

private:
	void setting_ui();
	void remove_item();

private:
	class UIImgSetCtrl* m_pImageCtrl;
	T_ItemDBIdx m_item_db_idx;
	T_ItemID m_item_id;
};
