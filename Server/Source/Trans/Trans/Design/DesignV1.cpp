#include "pch.h"
#include "DesignV1.h"
//#include "MultiSlot.h"

CDesignV1::CDesignV1()
{
 
}

CDesignV1::~CDesignV1()
{
	OnDestroy();
}


BOOL CDesignV1::OnCreate()
{
	return TRUE;
}


void CDesignV1::OnDestroy()
{
	return;
}
 
void CDesignV1::SetTeamSlot(CMultiSlot& MultiSlot, UINT8 ui8TeamSlot, T_ItemID TCharItemID )
{
	INT32 i32Team = (TCharItemID % 100000) / 1000 ;

	switch( i32Team - 1 )
	{
	case TEAM_RED:
		MultiSlot.SetTeamSlot( ui8TeamSlot, TEAM_RED );
		break;
	case TEAM_BLUE:
		MultiSlot.SetTeamSlot( ui8TeamSlot, TEAM_BLUE );
		break;

	}
	 
	return;
}
 

INT32 CDesignV1::GetExpValue(INT32 i32Rank)
{
	if( i32Rank < 0 ) return 0xFFFFFFFF;
	if( i32Rank >= DEF_RANK_COUNT ) return 0xFFFFFFFF;

	return g_ExpValueV1[i32Rank];
}

void CDesignV1::GetSaveWeaponDBInfo(CMultiSlot* pMultiSlot, INT32 &i32SaveWaeponCount, INT32 &i32ActiveWeaponDBIdx)
{
	i32SaveWaeponCount = 1;

	for (INT32 i = 0; i < S2MO_MULTI_SLOT_COUNT; ++i)
	{
		if (!pMultiSlot->IsActiveChar(i)) continue;

		S2MO_CHAR_BASE_INFO CharaInfo;
		pMultiSlot->GetCharBaseInfo(i, &CharaInfo);

		if (CharaInfo.m_i32WeaponDBIdx <= 0) continue;

		i32ActiveWeaponDBIdx = CharaInfo.m_i32WeaponDBIdx;
		break;
	}
	return;
}

INT32 CDesignV1::CheckMainSlot(CMultiSlot& MultiSlot)
{
	INT32 i32MainSLotIdx = -1;
	for (INT32 i = 0; i < TEAM_COUNT; ++i)
	{
		i32MainSLotIdx = MultiSlot.CheckTeamSlot((TEAM_TYPE)i);
	}

	return i32MainSLotIdx;
}

BOOL CDesignV1::DeleteChara(CMultiSlot& MultiSlot, INT32 i32DeleteSlotIdx, INT32& i32MainSlotIdx)
{
	return MultiSlot.DeleteCharInfoTeam(i32DeleteSlotIdx, i32MainSlotIdx);
}

void CDesignV1::CheckTeamSlot(CMultiSlot* pMultiSlot, UINT8 &ui8RedTeamSlot, UINT8 &ui8BlueTeamSlot)
{
	//if (ui8RedTeamSlot != ui8BlueTeamSlot) return;

	//ITEM_INFO CharaInfo;
	//for (INT32 i = 0; i < S2MO_MULTI_SLOT_COUNT; ++i)
	//{
	//	if (!pMultiSlot->IsActiveChar(i, MULTI_SLOT_CHECK_USING)) continue;

	//	pMultiSlot->GetCharEquipment(i, CHAR_EQUIPMENT_CHARA, &CharaInfo);

	//	// 기본 지급 캐릭터가 아니면 return;
	//	if (!(CharaInfo.m_TItemDBIdx & DEFAULT_ITEM_DBIDX_MIN)) continue;

	//	T_ItemID TItemID = GET_DEFAULT_ITEM_ID(CharaInfo.m_TItemDBIdx);

	//	switch (GET_ITEM_SUBTYPE(TItemID))
	//	{
	//	default:
	//	case CHARACTER_TEAM_RED:	ui8RedTeamSlot = i;		break;
	//	case CHARACTER_TEAM_BLUE:	ui8BlueTeamSlot = i;	break;
	//	}
	//}

	return;
}