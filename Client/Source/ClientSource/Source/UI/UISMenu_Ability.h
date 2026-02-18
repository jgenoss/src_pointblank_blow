#if !defined( __UI_SMenu_Ability_H__)
#define __UI_SMenu_Ability_H__

#include "UISMenuBase.h"
#include "UILBScrollSlot.h"

namespace item_def
{
	enum Category;
	enum WeaponComboType;
}

enum ABILITY_MAININFO_UPDATE_TYPE
{
	AMUT_PREVIEW_WEAPON,
	AMUT_PREVIEW_PARTS,
	AMUT_REVERT,
	AMUT_PURCHASED,
	AMUT_SLOTCHARA_CHANGED,
	AMUT_ITEM,
};

class SMenuAbility : public iSMenu
{
public:
	SMenuAbility(UISideMenu*);
	virtual ~SMenuAbility();
	

public:
	void clicked(int idx);

	void applyWeapon(CWeaponInfo* info)	{ m_previewWeaponInfo = info;	} // weapon
	void applyParts(const CHARA_PARTS_INFO* info)	{ m_partsInfo = info;	} // parts, chara
	void EnableExInfo(bool enable);
	void EnableMainInfo(bool enable);
	void EnableMainDefaultInfoOnly();

	void SetMainInfoUpdateType(ABILITY_MAININFO_UPDATE_TYPE type) { m_maininfoUpdateType = type; }
	void EnableCtrlsByShopCategory(item_def::Category shopCategory, bool nowFlag = false);

	void smenu_ability_update(void* =0, void* =0);

	virtual bool IsStatusEnable(void) const override;
	
public:
	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	
	virtual void	OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

private:
	virtual void Entrance(bool arg1 = false) override;	//frame Open, data update
	virtual void Exit(void* arg1 = nullptr, void* arg2 = nullptr) override;


private:
	void hit_test_enter( UINT32 inx, i3UIControl* pWnd );
	void hit_test_leave( UINT32 inx, i3UIControl* pWnd );

	UI::tmiHover_Callback< SMenuAbility, &SMenuAbility::hit_test_enter, &SMenuAbility::hit_test_leave	> m_HitTestCallback;
	UI::tmHover m_tmHitTest;

	CWeaponInfo* m_previewWeaponInfo;
	CWeaponInfo* m_equippedWeaponInfo;
	const CHARA_PARTS_INFO* m_partsInfo;
	ABILITY_MAININFO_UPDATE_TYPE m_maininfoUpdateType;
	item_def::Category m_curShopCategory;
	item_def::Category m_nowShopCategory;

private:
	typedef void	(SMenuAbility::*func)(INT32, const i3::user_data &);
	i3::unordered_map< GAME_EVENT, func>	m_RecvGameEventMap;
	
	void	RegisterGameEventFunc( GAME_EVENT evt, func f);
	void	ReceivedGameEvent_Update_Ability( INT32, const i3::user_data&);
};

#endif