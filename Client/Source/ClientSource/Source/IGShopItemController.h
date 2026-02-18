#ifndef _IGSHOP_ITEM_CONTROLLER_
#define _IGSHOP_ITEM_CONTROLLER_

#include "Network/ClientP2PDef.h"

class IGShopItemController
{
private:
	static IGShopItemController* m_pInstance;

public:
	static IGShopItemController* GetInstance()
	{
		if (m_pInstance == 0)
		{
			m_pInstance = new IGShopItemController;
			MEMDUMP_NEW( m_pInstance, sizeof( IGShopItemController));
		}
		return m_pInstance;
	}
	static void Destroy() { I3_SAFE_DELETE(m_pInstance); }

public:
	virtual ~IGShopItemController() { m_pInstance = nullptr; }

protected:
	IGShopItemController();

public:
	void Apply();
	void ApplyWeapon();
	void ApplyWeaponEndurance();
	void ApplyEquipment();
	void ApplyCashItem();
	void ApplyTitle();
	void ApplyCharaHUD();

private:
	void				_ChangeWeapon( WEAPON_SLOT_TYPE slot, bool changeDirect = true);
	WEAPON_SLOT_TYPE	_FindSlotWillBeApplied(UINT32* old, UINT32* cur);
	bool				_CanApply() const;
	void				_SetEquipToNetChara(NET_CHARA_INFO* charaNetInfo) const;
	bool				_IsMyCharacter() const; 
	UINT32				_FindCashItemFlagPurchased(UINT32 oldItemFlag, UINT32 curItemFlag);
	void				_ChangeWeaponSlot( WEAPON_SLOT_TYPE slot);

private:
	INT32 get_chara_net_index() const { return m_Slot_ItemInfo_Buf.m_Slot_Equipment.m_i32SlotIdx; } //이 정도면 인라인으로 처리될거다.

	CGameCharaBase*				_GetChara() const			{ return g_pCharaManager->getCharaByNetIdx( get_chara_net_index() ); }
	const GLOBAL_CHARA_INFO*	_GetCharaInfo() const		{ return _GetChara()->getCharaInfo(); }
	GLOBAL_CHARA_INFO*			_SetCharaInfo()				{ return _GetChara()->setCharaInfo(); }

	NET_CHARA_INFO*				_GetCharaNetInfo() const;
	const GAMEINFO_CHARACTER*	_GetGameCharaInfo() const;
	GAMEINFO_CHARACTER*			_SetGameCharaInfo();
	const SLOT_INFO*			_GetSlotInfo() const;
	SLOT_INFO*					_SetSlotInfo();

public:
	PACKET_PROTOCOL_BASE_EQUIPMENT_ACK m_Base_ItemInfo_Buf;
	PACKET_H_PROTOCOL_SLOT_EQUIPMENT_ACK m_Slot_ItemInfo_Buf;
};

#endif