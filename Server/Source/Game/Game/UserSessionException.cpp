#include "pch.h"
#include "UserSession.h"
#include "ModuleTrans.h"
#include "ModuleContextNC.h"

void CUserSession::_ExceptionEquip( void )
{
	// Default Item Insert
	// Normal 지급 아이템은 묻지도 따지지도 않고 지급 합니다.
	DEFAULT_ITEM_SET* pDefaultSet;
	pDefaultSet = g_pContextMain->GetDefaultItemSet( DEFAULT_ITEM_TYPE_NORMAL );
	if( pDefaultSet )
	{
		for( INT32 i = 0 ; i < pDefaultSet->m_ui8Count ; i++ )
		{
			_ExceptionInsert(pDefaultSet->m_TItemID[i]);
		}
	}

	// PC방 유저일 경우 지급 아이템
	pDefaultSet = g_pContextMain->GetDefaultItemSet( DEFAULT_ITEM_TYPE_PCCAFE, m_UserInfoBasic.m_ui8PCCafe );
	if( pDefaultSet )
	{
		for( INT32 i = 0 ; i < pDefaultSet->m_ui8Count ; i++ )
		{
			_ExceptionInsert( pDefaultSet->m_TItemID[ i ], ITEM_ATTR_TYPE_PC_1 );
		}
	}
}

void CUserSession::_ExceptionChara(T_ItemID TCharaID, UINT8 ui8ItemType)
{
	INT32 i32SlotIdx = m_MultiSlot.FindSlotIdxPCCafeChara();
	if (i32SlotIdx < 0) return;

	T_GoodsID				TGoodsID = TCharaID * 100;
	

	PACKET_CHAR_CREATE_CHARA_ACK	Packet; 

	Packet.m_TResult = EVENT_ERROR_SUCCESS;
	Packet.m_ui8SlotIdx = (UINT8)i32SlotIdx;
	Packet.m_ui8CreateType = GOODS_BUY_NONE;
	Packet.m_bAddCharaSlot = true;
	Packet.m_ui8SlotState = MULTI_SLOT_STATE_NORMAL;
	Packet.m_ui32Point = m_UserInfoBasic.m_ui32Point;
		
	S2MO_CHAR_BASE_INFO BaseInfo;
	BaseInfo.Reset(); 
	BaseInfo.m_ui32CreateDate = i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI();
	i3String::Copy(BaseInfo.m_strCharName, _TT("CharacterGame"), NET_NICK_NAME_SIZE); 
	Packet.m_BaseInfo = &BaseInfo;

	S2MO_CHAR_SKILL_INFO stSkill;
	stSkill.Reset();
	Packet.m_SkillInfo = stSkill;

	INVEN_BUFFER			CharaInven;
	CharaInven._ui32ItemID	= TCharaID;
	CharaInven._ui8ItemType	= ui8ItemType;
	CharaInven._ui32ItemArg = 1;
	g_MakeBasicItemDBIdx(CharaInven._TItemWareDBIdx, TCharaID);
		
	CreateChar_A(&Packet, TGoodsID, &CharaInven);
}


void CUserSession::_ExceptionSpecial( void )
{
	if (g_pContextMain->IsContentsEnabled(CONTENTS_MISSION_CARD))
	{
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//베레모 예외처리 입니다. 
		if (GET_USERTITLE(m_ui64GainUserTitle, DESIGNATION_TYPE_13))
		{
			_ExceptionInsert(BERET_ASSAULT_ITEMID);
		}

		if (GET_USERTITLE(m_ui64GainUserTitle, DESIGNATION_TYPE_25))
		{
			_ExceptionInsert(BERET_SMG_ITEMID);
		}

		if (GET_USERTITLE(m_ui64GainUserTitle, DESIGNATION_TYPE_19))
		{
			_ExceptionInsert(BERET_RIFLE_ITEMID);
		}

		if (GET_USERTITLE(m_ui64GainUserTitle, DESIGNATION_TYPE_39))
		{
			_ExceptionInsert(BERET_SHOTGUN_ITEMID);
		}

		if (GET_USERTITLE(m_ui64GainUserTitle, DESIGNATION_TYPE_34))
		{
			_ExceptionInsert(BERET_HANDGUN_ITEMID);
		}

	}
	
	//소장이상인 경우 지정된 장성계급 아이템을 지급합니다.
	if( m_UserInfoBasic.m_ui32Rank > RANK_46 && m_UserInfoBasic.m_ui32Rank <= RANK_51 )
	{		
		const UINT8 count = g_pContextMain->m_ui8GeneralItemCount;
		for ( int  i = 0 ; i < count; ++i )
		{
			if( g_pContextMain->m_aaui32GeneralItem[i] != 0 )
				_ExceptionInsert( g_pContextMain->m_aaui32GeneralItem[i] );
		}
	}
}

void CUserSession::_ExceptionTitleItem( INT32 i32ThreadIdx, UINT8 ui8TitleId )
{
	if( m_ui8LocaleCode == LOCALE_CODE_RUSSIA )	// 러시아는 v1에서 호칭 획득시 아이템 지급 안 하고 있음
		return;

	UINT8 ui8ItemCount = 0;
	ST_ITEM aItem[MAX_SHOP_DB_ITEM_COUNT];


	switch( ui8TitleId )
	{
	case 8:		// 일반 돌격 - SG 550 S.
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 103014;
			aItem[0].m_TItemArg = 10; 
		}
		break;
	case 10:	// 수석 돌격 - G36C Ext.
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 103013;
			aItem[0].m_TItemArg = 10;  
		}
		break;
	case 12:	// 돌격 지휘관 - AK SOPMAD, AUG A3
		{
			
				ui8ItemCount = 2;
				aItem[0].m_TItemID = 103015;
				aItem[0].m_TItemArg = 10;  
				aItem[1].m_TItemID = 103036;
				aItem[1].m_TItemArg = 10;  
			
		}
		break;
	case 26:	// 일반 침투 - Dual Knife
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 315001;
			aItem[0].m_TItemArg = 10;   
		}
		break;
	case 28:	// 특등 침투장교 - Amok Kukri
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 301004;
			aItem[0].m_TItemArg = 10;  
		}
		break;
	case 29:	// 침투 지휘관 - Mini Axe
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 301007;
			aItem[0].m_TItemArg = 10;  
		}
		break;
	case 14:	// 일반 저격 - Dragunov G.
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 105006;
			aItem[0].m_TItemArg = 10;  
		}
		break;
	case 16:	// 수석 저격 - PSG-1 S.
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 105007;
			aItem[0].m_TItemArg = 10;  
		}
		break;
	case 18:	// 저격 지휘관 - L115A1
		{
			
				ui8ItemCount = 1;
				aItem[0].m_TItemID = 105005;
				aItem[0].m_TItemArg = 10;  
			
		}
		break;
	case 30:	// 일반 사수 - P99&HAK
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 213001;
			aItem[0].m_TItemArg = 10;  
		}
		break;
	case 32:	// 특등 사수장교 - Dual Handgun
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 214001;
			aItem[0].m_TItemArg = 10;  
		}
		break;
	case 33:	// 사격 지휘관 - Dual D-Eagle
		{ 
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 214002;
			aItem[0].m_TItemArg = 10;  
		}
		break;
	case 20:	// 일반 습격 - MP5K G./ Spectre W.
		{
			ui8ItemCount = 2;
			aItem[0].m_TItemID = 104007;
			aItem[0].m_TItemArg = 10;  
			aItem[1].m_TItemID = 104009;
			aItem[1].m_TItemArg = 10;   
		}
		break;
	case 22:	// 수석 습격 - P90 Ext.
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 104011;
			aItem[0].m_TItemArg = 10;   
		}
		break;
	case 24:	// 습격 지휘관 - Kriss Super V
		{
			switch(m_ui8LocaleCode)
			{			
			case LOCALE_CODE_INDONESIA:
				{
					ui8ItemCount = 1;
					aItem[0].m_TItemID = 104052;
					aItem[0].m_TItemArg = 10;   
				}
				break;				
			default: 
				{
					ui8ItemCount = 1;
					aItem[0].m_TItemID = 104013;
					aItem[0].m_TItemArg = 10;   
				}
				break;
			}
		}
		break;
	case 40:	// 일반 폭파 - C-5
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 407002;
			aItem[0].m_TItemArg = 10;    
		}
		break;
	case 42:	// 수석 폭파 - WP Smoke
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 527001;
			aItem[0].m_TItemArg = 10;     
		}
		break;
	case 35:	// 일반 타격 - 870MCS W.
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 106004;
			aItem[0].m_TItemArg = 10;     
		}
		break;	break;	
	case 37:	// 특등 타격장교 - SPAS - 15
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 106003;
			aItem[0].m_TItemArg = 10; 
		}
		break;
	case 44:
	{
		if (m_ui8LocaleCode == LOCALE_CODE_LATIN_AMERICA)
		{
			ui8ItemCount = 1;
			aItem[0].m_TItemID = 407013;
			aItem[0].m_TItemArg = 10;
		}

	}
	break;
	case 1:		// 일반
	case 2:		// 선임
	case 3:		// 특등
	case 4:		// 특수
	case 5:		// 돌격
	case 6:		// 저격
	case 7:		// 습격
	case 9:		// 선임 돌격
	case 11:	// 특등 돌격
	case 13:	// 광폭한 맹수
	case 15:	// 선임 저격
	case 17:	// 특등 저격
	case 19:	// 은밀한 암살자
	case 21:	// 선임 습격
	case 23:	// 특등 습격
	case 25:	// 교활한 침입자
	case 27:	// 수석 침투
	case 31:	// 수석 사수
	case 34:	// 예리한 명사수	
	case 39:	// 난폭한 파괴자
	case 43:	// 특등 폭파
	case 41:
	case 38:	// 타격지휘관
		break;
	}

	g_pModuleTrans->AuthSendInsertItem(i32ThreadIdx, this, ITEM_INSERT_REQUEST_GET_TITLE, ui8ItemCount , aItem );
}
