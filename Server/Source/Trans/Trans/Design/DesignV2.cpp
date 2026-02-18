#include "pch.h"
#include "DesignV2.h"

CDesignV2::CDesignV2()
{
 
}

CDesignV2::~CDesignV2()
{
	OnDestroy();
}


BOOL CDesignV2::OnCreate()
{
	//m_ui8BlueTeamSlot	= 0xFF;
	return TRUE;
}


void CDesignV2::OnDestroy()
{
	return;
} 

void CDesignV2::SetTeamSlot( CMultiSlot& MultiSlot, UINT8 ui8TeamSlot, T_ItemID TCharItemID )
{
	MultiSlot.SetTeamSlot( ui8TeamSlot, TEAM_RED );

	return;
}

INT32 CDesignV2::GetExpValue(INT32 i32Rank)
{
	if( i32Rank < 0 ) return 0xFFFFFFFF;
	if( i32Rank >= DEF_RANK_COUNT ) return 0xFFFFFFFF;

	return g_ExpValueV2[i32Rank];
}

void CDesignV2::GetSaveWeaponDBInfo(CMultiSlot* pMultiSlot, INT32 &i32SaveWaeponCount, INT32 &i32ActiveWeaponDBIdx)
{
	i32SaveWaeponCount = S2MO_MULTI_SLOT_COUNT;
	i32ActiveWeaponDBIdx = -1;
	return;
}

INT32 CDesignV2::CheckMainSlot(CMultiSlot& MultiSlot)
{
	return MultiSlot.CheckMainSlot(TEAM_RED);
}

BOOL CDesignV2::DeleteChara(CMultiSlot& MultiSlot, INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx)
{
	return MultiSlot.DeleteCharInfo(i32DeleteSlotIdx, i32MainSlotIdx);
}