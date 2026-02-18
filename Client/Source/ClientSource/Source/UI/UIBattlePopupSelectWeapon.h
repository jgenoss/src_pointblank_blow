#pragma once

#include "UIBattlePopupBase.h"

class UIImgSetCtrl;

namespace ws //weapon select
{
	enum BPSW_BTN_ID;
}

class UIBattlePopupSelectWeapon : public UIBattlePopupBase
{
	I3_CLASS_DEFINE( UIBattlePopupSelectWeapon, UIBattlePopupBase);
	
public:
	UIBattlePopupSelectWeapon();
	virtual ~UIBattlePopupSelectWeapon();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void _InitializeAtLoad( i3UIScene * pScene) override;
	virtual void _ClearAtUnload( i3UIScene * pScene) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr ) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnExitStart() override;
	virtual void	OnUnloadAllScenes() override;

	virtual bool	OnKeyInput(i3InputDeviceManager * pInputMng) override;

	virtual bool	OnOKButton( void) override;
	virtual bool	OnCloseButton( void) override;

public:
	void SetInitList() { m_bInitList = true; m_Is_Clicked_Ok_Btn = false; } //사용가능한 무기 정보를 얻을 수 있도록 설정한다.
	void WeaponChange(); //선택한 무기를 변경한다.
	void	 OnBtnClick(ws::BPSW_BTN_ID btnID);

private:
	void Init_UI(); //weapon slot을 갱신한다.
	void Update_UI(); //weapon slot을 갱신한다.
	void Update_Slot(size_t idx, UIImgSetCtrl* p, INVEN_BUFFER* ib);
		
	void Move_Category(bool dir); //false: up, true: down
	void Move_Slot(bool dir); //false: left, true: right

private:
	void Init_Weapon_DB();

	size_t Get_Count_inDB(size_t idx) const; //GetWeaponCount
	size_t Get_Index_inDB(T_ItemID itemID, size_t idx) const;
	T_ItemDBIdx Get_WareIndex_inDB(T_ItemID itemID); //GetWeaponWareDBIdx
	bool Get_Type_inDB(INT32 idx, WEAPON_SLOT_TYPE nUsageType, UINT8* Type, UINT32* Arg); //GetWeaponType
	T_ItemID Get_ID_inDB(INT32 idx, WEAPON_SLOT_TYPE nUsageType); //GetWeaponID

private:
	i3::vector<INVEN_BUFFER*> m_WeaponDBList[ MAX_CATEGORY_WEAPON];

	UIImgSetCtrl* m_ImageSet_List[MAX_CATEGORY_WEAPON];
	UIImgSetCtrl* m_pImageSetPrevPrimary;
	UIImgSetCtrl* m_pImageSetNextPrimary;

	struct SelectInfo
	{
		INT32 curr, backup;
	} m_SelectInfo[MAX_CATEGORY_WEAPON];
	INT32 m_Focused_Category;

	bool m_bInitList;
	bool m_Is_Clicked_Ok_Btn;
	bool m_is_zero_durability_weapon_selected;
};

namespace ws
{
	enum BPSW_BTN_ID 
	{
		BPSW_BTN_NONE = -1,

		BPSW_BTN_PRIM_PREV,
		BPSW_BTN_PRIM_NEXT,
		BPSW_BTN_SUB_PREV,
		BPSW_BTN_SUB_NEXT,
		BPSW_BTN_MELEE_PREV,
		BPSW_BTN_MELEE_NEXT,
		BPSW_BTN_THROW_PREV,
		BPSW_BTN_THROW_NEXT,
		BPSW_BTN_ITEM_PREV,
		BPSW_BTN_ITEM_NEXT,

		BPSW_BTN_COUNT
	};

	size_t get_move_number(size_t curr, size_t max, bool dir); //false: left, true: right, max는 max - 1 이다.
}
