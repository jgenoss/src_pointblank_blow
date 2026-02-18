#ifndef __DESIGN_BASE_H__
#define __DESIGN_BASE_H__

class CDesignBase 
{
	
private:

protected:

public:
	 
	CDesignBase();
	virtual ~CDesignBase();

	virtual BOOL				OnCreate(  );
	void						OnDestroy();

	virtual INT32				GetDefaultCreateCharaCount() 				{ return 0; }
	//virtual UINT8				GetBlueTeamSlot(UINT8 ui8BlueTeamSlot)		{ return 0; }
	virtual void				SetTeamSlot(CMultiSlot& MultiSlot, UINT8 ui8TeamSlot, T_ItemID TCharItemID) = 0;
	virtual INT32				GetExpValue(INT32 i32Rank) = 0;
	virtual void				GetSaveWeaponDBInfo(CMultiSlot* pMultiSlot, INT32 &i32SaveWaeponCount , INT32 &i32ActiveWeaponDBIdx) = 0;

	virtual INT32				CheckMainSlot(CMultiSlot& MultiSlot) = 0;
	virtual BOOL				DeleteChara(CMultiSlot& MultiSlot, INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx) = 0;

	virtual void				CheckTeamSlot(CMultiSlot* pMultiSlot, UINT8 &ui8RedTeamSlot, UINT8 &ui8BlueTeamSlot) = 0;
};

#endif