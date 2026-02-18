#ifndef __DESIGN_V2_H__
#define __DESIGN_V2_H__

#include "DesignBase.h"

class CDesignV2 : public CDesignBase
{
private:
	//UINT8		m_ui8BlueTeamSlot;
	
protected:

public:
	 CDesignV2();
	virtual ~CDesignV2();

	virtual BOOL				OnCreate(  );
	void						OnDestroy();

	virtual INT32				GetDefaultCreateCharaCount() 				{ return 0; }
	//virtual UINT8				GetBlueTeamSlot(UINT8 ui8BlueTeamSlot)		{ return m_ui8BlueTeamSlot;  }
	virtual void				SetTeamSlot(CMultiSlot& MultiSlot, UINT8 ui8TeamSlot, T_ItemID TCharItemID);
	virtual INT32				GetExpValue(INT32 i32Rank);
	virtual void				GetSaveWeaponDBInfo(CMultiSlot* pMultiSlot, INT32 &i32SaveWaeponCount, INT32 &i32ActiveWeaponDBIdx);

	virtual INT32				CheckMainSlot(CMultiSlot& MultiSlot);
	virtual BOOL				DeleteChara(CMultiSlot& MultiSlot, INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx);

	virtual void				CheckTeamSlot(CMultiSlot* pMultiSlot, UINT8 &ui8RedTeamSlot, UINT8 &ui8BlueTeamSlot) { return; };;
};

#endif