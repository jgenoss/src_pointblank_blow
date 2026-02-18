#pragma once

#include "UISMenuBase.h"
#include "UILBScrollSlot.h"
#include "UISMenu_SlotBase.h"
#include "i3Base/itl/signal/signal_common.h"

class iSMenuSlot;

class SMenuEquip : public iSMenu
{
	enum { SM_SLOT5 = 0, 
		SM_SLOT7_P, SM_SLOT7_W, 
		SM_SUB_P, SM_SUB_W, 
		SM_SLOT_MAX };

private:
	typedef i3::vector<iSMenuSlot* > SubList;
	SubList mSubList;

	iSMenuSlot* pMainSlot;
	iSMenuSlot* pSubSlot;
	iSMenuSlot* pCharaSlot;

	i3::scoped_connection_set		m_SigConnection;

	bool m_bBackupEquipment;

public:
	void clicked(int idx);

public:
	void cb_smenu_equip_close(bool bArg);
	void cb_smenu_equip_slot_clear();
	
public:
	SMenuEquip(UISideMenu*);
	virtual ~SMenuEquip();

private:
	virtual void OnCreate( i3GameNode * pParent) override;

	virtual void OnLoadAllScenes() override;
	virtual void OnUnloadAllScenes() override;

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	virtual void MoveBackScene() override;

private:
	virtual void Entrance(bool arg1 = false) override;	//frame Open, data update
	virtual void Exit(void* arg1 = nullptr, void* arg2 = nullptr) override;

private:
	void notify_open_slot_sub_impl(void * pArg);
	void notify_close_slot_sub_impl(void * pArg);
	void notify_item_change_impl(void * pArg);

public:
	virtual void Notify( iSMenuSlot::SLOT_MSG msg, void * pArg = nullptr);
	void GetFrmPos(INT32 &x, INT32 &y, UINT32 &w, UINT32 &h) const;
	void GetSubFrmPos( INT32 &x,  INT32 &y, UINT32 &w, UINT32 &h) const;
	void SetDefaultWeapon( WEAPON_SLOT_TYPE slot, T_ItemID ui32ItemID );

	i3UIScene * GetMainSlotScene(INT32 idx);
	i3UIScene * GetSubSlotScene(INT32 idx);
	bool		IsSelectedSlot(INT32 idx);

	void		UIEvent_ChangedCharaEquip( void);

private:
	typedef void	(SMenuEquip::*func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, func>	m_RecvEventMap;
	void		RegisterGameEventFunc( GAME_EVENT evt, func f);

	void		ReceivedGameEvent_ChangedCharaEquip( INT32, const i3::user_data&);
};
