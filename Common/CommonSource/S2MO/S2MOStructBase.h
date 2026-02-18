#ifndef __S2_MO_STRUCT_BASE_H__
#define __S2_MO_STRUCT_BASE_H__

//#include "S2MOStruct.h"

#pragma pack( push, 1)

struct PACKET_BASE_CONNECT_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_CONNECT_ACK>
{
	S2MOValue<INT32>									m_i32SessionIdx;	
	S2MOValue<INT16>									m_i16PacketRandSeed;
	S2MOValue<SC_VERSION>								m_stServerVersion;
	S2MOValue<INT8, RSA_KET_SIZE>						m_pRSAKey;

	// For 1.0
	S2MOValue<UINT8, GAME_CHANNEL_COUNT>				m_ui8ChannelType;

	PACKET_BASE_CONNECT_ACK()
	{
		_SetValue( m_i32SessionIdx );
		_SetValue( m_i16PacketRandSeed );
		_SetValue( m_stServerVersion );
		_SetValue( m_pRSAKey );
		_SetValue( m_ui8ChannelType );
	}
};

struct PACKET_BASE_GET_USER_INFO_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_GET_USER_INFO_REQ>
{
};

struct PACKET_BASE_GET_USER_INFO_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_GET_USER_INFO_ACK>
{
	S2MOValue<UINT8>												m_ui8Age;											// 나이
	S2MOValue<USER_INFO_BASIC>										m_UserInfoBasic;									// User 기본 정보
	S2MOValue<USER_INFO_RECORD>										m_UserInfoRecord;									// User 전적 정보
	S2MOValue<USER_INFO_INVITEM_DATA>								m_UserInfoInvItemData;								// 상용화 아이템 정보

	S2MOValue<REAL32>												m_r32PCCafeExp;										// Total PC Cafe Bonus
	S2MOValue<REAL32>												m_r32PCCafePoint;									// Total PC Cafe Bonus

	S2MOValue<bool>													m_bGetEventItem;									// 이벤트 아이템 지급 여부
	S2MOValue<UINT32>												m_ui32TotalConnectCount;							// 총 누적 접속 횟수. 가이드 시스템 때문에 추가.	USER_INFO_DAILY			m_UserInfoDaily;};

	S2MOValue<S2_MULTI_SLOT_INFO>									m_MultiSlotInfo;									// MultiSlot Data

	S2MOValue<UINT32>												m_ui32ReportServerTime;
	S2MOValue<UINT32>												m_ui32ClientIP;

	S2MOValue<ATTENDANCE_USER, ATTENDANCE_TYPE_MAX>					m_stAttendance;
	S2MOValue<ATTENDANCE_INFO, ATTENDANCE_TYPE_MAX>					m_stAttendanceInfo;

	// For 1.0
	S2MOValue<UINT32>												m_ui32Miniature;									// 약장
	S2MOValue<UINT32>												m_ui32Insignia;										// 휘장
	S2MOValue<UINT32>												m_ui32Order;										// 훈장
	S2MOValue<UINT32>												m_ui32Master;										// 마스터훈장. v1.5버전에서도 사용합니다. 지우면 안되요!

	S2MOValue<QUESTING_INFO>										m_QuestIngInfo;										// 퀘스트 진행 상태
	S2MOValue<UINT8, MAX_QUEST_ALL_USE_NEW>							m_aQuestCtxActive;									// 퀘스트 완료 상태

	S2MOValue<UINT64>												m_ui64GainUserTitle;								// 획득한 호칭
	S2MOValue<UINT8, MAX_EQUIP_USER_TITLE>							m_aui8EquipUserTitle;								// 장착한 호칭
	S2MOValue<INT32>												m_i32LimitUserTitleCount;							// 사용가능 호칭 장비 슬롯 갯수

	// For 1.5
	S2MOValue<NOTIFY_MEDAL, S2MO_MAX_NOTIFY_MADAL_COUNT>			m_NotifyMedal;										// 알리미 등록 메달
	S2MOValue<USER_INFO_DAILY>										m_UserInfoDaily;									// 당일전적
	S2MOValue<QUICKJOIN_INFO, S2MO_MAX_QUICK_JOIN_INFO_COUNT>		m_aQuickJoinInfo;									// 빠른입장 설정정보.

	S2MOValue<bool>													m_bGiftAutoAuth;									// 선물함에 선물이 인벤토리로 자동 지급 됬는지.
	S2MOValue<UINT8>												m_ui8Country;										// 국가코드

	PACKET_BASE_GET_USER_INFO_ACK()
	{
		// Last Update : 2015-10-21 10:33:54
		_SetValue( m_ui8Age );
		_SetValue( m_UserInfoBasic );
		_SetValue( m_UserInfoRecord );
		_SetValue( m_UserInfoInvItemData );
		_SetValue( m_r32PCCafeExp );
		_SetValue( m_r32PCCafePoint );
		_SetValue( m_bGetEventItem );
		_SetValue( m_ui32TotalConnectCount );
		_SetValue( m_MultiSlotInfo );
		_SetValue( m_ui32ReportServerTime );
		_SetValue( m_ui32ClientIP );
		_SetValue( m_stAttendance );
		_SetValue( m_stAttendanceInfo );
		_SetValue( m_ui32Miniature );
		_SetValue( m_ui32Insignia );
		_SetValue( m_ui32Order );
		_SetValue( m_ui32Master );
		_SetValue( m_QuestIngInfo );
		_SetValue( m_aQuestCtxActive );
		_SetValue( m_ui64GainUserTitle );
		_SetValue( m_aui8EquipUserTitle );
		_SetValue( m_i32LimitUserTitleCount );
		_SetValue( m_NotifyMedal );
		_SetValue( m_UserInfoDaily );
		_SetValue( m_aQuickJoinInfo );
		_SetValue( m_bGiftAutoAuth );
		_SetValue( m_ui8Country );
	}
};

struct PACKET_BASE_GET_INVEN_INFO_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_GET_INVEN_INFO_REQ>
{
};

struct PACKET_BASE_GET_INVEN_INFO_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_GET_INVEN_INFO_ACK>
{
	S2MOValue<INVEN_BUFFER, MAX_INVEN_COUNT>		m_aInventory;

	PACKET_BASE_GET_INVEN_INFO_ACK()
	{
		_SetValue( m_aInventory );
	}
};

struct PACKET_BASE_GET_SYSTEM_INFO_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_GET_SYSTEM_INFO_REQ>
{
	S2MOValue<UINT8>									m_ui8LinkMethod;

	PACKET_BASE_GET_SYSTEM_INFO_REQ()
	{
		_SetValue(m_ui8LinkMethod);
	}
};

struct PACKET_BASE_GET_SYSTEM_INFO_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_GET_SYSTEM_INFO_ACK>
{
	S2MOValue<bool>													m_bNewRepair;			// 신규 내구도 수리 공식인지 체크
	S2MOValue<RANKUP_ITEM, MAX_RANK_COUNT>							m_aRankUpItem;			// 랭크업 아이템
	S2MOValue<UINT8>												m_ui8RSEnable;

	S2MOStringA<MAX_REPORT_FTP_ADDRESS>								m_strReportFtpAddress;
	S2MOValue<UINT16>												m_ui16ReportFtpPort;

	S2MOValue<bool>													m_bGiftEnable;										//선물함 on/off 여부
	S2MOValue<bool>													m_bCashRecharge;		// 캐쉬 충전 페이지 On/Off
	S2MOStringA<MAX_BILLING_URL>									m_strBillingURL;

	S2MOValue<GAME_SERVER_INFO, MAX_GAME_SERVER_COUNT>				m_aGameServerInfo;

	S2MOValue<UINT16>												m_ui16UDPPort;

	// For 1.0
	S2MOValue<UINT32, QUEST_CARDSET_TYPE_COUNT + 31 / 32>			m_pui32ActiveQuest;
	S2MOValue<UINT32, QUEST_CARDSET_TYPE_COUNT>						m_ui32QuestCardGoodsID;

	// For 1.5
	S2MOValue<UINT8>												m_ui8RecvNote;			// Note List  받았는지 체크 ?? 뭐하는거지?
	S2MOValue<UINT32, DAILY_GIFT_TYPE_MAX>							m_ui32DailyGiftGauge;	// 판수보상 단계별 시간값(sec)

	S2MOStringA<MAX_RANKING_URL_SIZE>								m_strRankingURL;		// 제압모드 랭킹 URL

	S2MOValue<INT32>												m_i32DomiScore;			// 제압모드 스코어
	S2MOValue<INT32>												m_i32DomiScoreBest;		// 제압모드 최고

	S2MOValue<ContentsEnableFlag>									m_ContentsFlag;

	S2MOValue<CLAN_MATCH_SEASON_EXT>								m_ClanMatchSeason;

	S2MOValue<GIFT_BUY_RANKING>										m_aGiftBuyRanking;
	S2MOValue<bool>													m_bGiftShopEnable;

	S2MOValue<UINT8>												m_ui8TagCount;			//태그 개수
	S2MOValue<UINT8, SHOP_TAG_COUNT>								m_ui8TagPriority;		//우선순위
	S2MOValue<UINT8>												m_ui8MultiWeaponCount;	// 멀티 슬롯 개수

	PACKET_BASE_GET_SYSTEM_INFO_ACK()
	{
		// Last Update : 2015-10-21 10:34:50
		_SetValue( m_bNewRepair );
		_SetValue( m_aRankUpItem );
		_SetValue( m_ui8RSEnable );
		_SetValue( m_strReportFtpAddress );
		_SetValue( m_ui16ReportFtpPort );
		_SetValue( m_bGiftEnable );
		_SetValue( m_bCashRecharge );
		_SetValue( m_strBillingURL );		
		_SetValue( m_aGameServerInfo );
		_SetValue( m_ui16UDPPort );
		_SetValue( m_pui32ActiveQuest );
		_SetValue( m_ui32QuestCardGoodsID );
		_SetValue( m_ui8RecvNote );
		_SetValue( m_ui32DailyGiftGauge );
		_SetValue( m_strRankingURL );
		_SetValue( m_i32DomiScore );
		_SetValue( m_i32DomiScoreBest );
		_SetValue( m_ContentsFlag );
		_SetValue( m_ClanMatchSeason );
		_SetValue( m_aGiftBuyRanking );
		_SetValue( m_ui8TagCount );
		_SetValue( m_ui8TagPriority );
		_SetValue( m_bGiftShopEnable);
		_SetValue( m_ui8MultiWeaponCount );
	}
};


struct PACKET_BASE_GET_OPTION_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_GET_OPTION_REQ>
{
};

struct PACKET_BASE_GET_OPTION_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_GET_OPTION_ACK>
{
	S2MOValue<UINT8>												m_ui8DefaultKey;
	S2MOValue<NET_ENV_SET>											m_NetEnvSet;
	S2MOValue<GAME_KEY_STRUCT, GAME_KEY_MAP_COUNT>					m_KeyMapArray;
	S2MOStringW<MACRO_MAX_LENGHT>									m_strMacro[ MACRO_MAX_COUNT ];

	PACKET_BASE_GET_OPTION_ACK()
	{
		// Last Update : 2015-10-21 16:33:14
		_SetValue( m_ui8DefaultKey );
		_SetValue( m_NetEnvSet );
		_SetValue( m_KeyMapArray );
		_SetValue( m_strMacro[0] );
		_SetValue( m_strMacro[1] );
		_SetValue( m_strMacro[2] );
		_SetValue( m_strMacro[3] );
		_SetValue( m_strMacro[4] );
	}
};

struct PACKET_BASE_CREATE_NICK_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_CREATE_NICK_REQ>
{
	S2MOStringW<NET_NICK_NAME_SIZE>									m_strNickName;

	PACKET_BASE_CREATE_NICK_REQ()
	{
		_SetValue( m_strNickName );
	}
};

struct PACKET_BASE_CREATE_NICK_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_CREATE_NICK_ACK>
{
	S2MOStringW<NET_NICK_NAME_SIZE>									m_strNickName;
	S2MOValue<ITEM_INFO, CHAR_EQUIPMENT_COMMON_COUNT>				m_aCommonEquipment;

	PACKET_BASE_CREATE_NICK_ACK()
	{
		_SetValue( m_strNickName );
		_SetValue( m_aCommonEquipment );
	}
};

struct PACKET_SERVER_MESSAGE_CHANGE_INVENTORY : public S2MOPacketBaseT<PROTOCOL_SERVER_MESSAGE_CHANGE_INVENTORY>
{
	S2MOValue<ITEM_INFO, CHAR_EQUIPMENT_COMMON_COUNT>				m_aCommonEquips;
	S2MOValue<S2_CHAR_CHANGE_EQUIP_INFO, S2MO_MULTI_SLOT_PACKET_COUNT>	m_ChangeEquipInfo;

	PACKET_SERVER_MESSAGE_CHANGE_INVENTORY()
	{
		_SetValue( m_aCommonEquips );
		_SetValue( m_ChangeEquipInfo );
	}
};

struct PACKET_BASE_USER_LEAVE_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_USER_LEAVE_REQ>
{
};

struct PACKET_BASE_USER_LEAVE_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_USER_LEAVE_ACK>
{
};

struct PACKET_BASE_USER_ENTER_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_USER_ENTER_REQ>
{
	S2MOValue<T_UID>												m_TUID;
	S2MOStringA<NET_ID_SIZE>										m_strID;

	PACKET_BASE_USER_ENTER_REQ()
	{
		_SetValue( m_TUID );
		_SetValue( m_strID );
	}
};

struct PACKET_BASE_USER_ENTER_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_USER_ENTER_ACK>
{
};

struct PACKET_BASE_ENTER_PASS_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_ENTER_PASS_REQ>
{
	S2MOStringA<NET_SERVER_PW>										m_strPass;

	PACKET_BASE_ENTER_PASS_REQ()
	{
		_SetValue( m_strPass );
	}
};

struct PACKET_BASE_ENTER_PASS_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_ENTER_PASS_ACK>
{
};

struct PACKET_BASE_GET_CHANNELLIST_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_GET_CHANNELLIST_REQ>
{
	S2MOValue<INT32>												m_i32ServerIdx;

	PACKET_BASE_GET_CHANNELLIST_REQ()
	{
		_SetValue( m_i32ServerIdx );
	}
};

struct PACKET_BASE_GET_CHANNELLIST_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_GET_CHANNELLIST_ACK>
{
	S2MOValue<UINT8>												m_ui8ChannelCount;
	S2MOValue<UINT16>												m_ui16MaxUserCount;
	S2MOValue<UINT16, GAME_CHANNEL_COUNT>							m_ui16NowUserCount;
	S2MOValue<TYPE_CHANNEL_BONUS, CHANNEL_TYPE_MAX>					m_stChannelBonus;

	PACKET_BASE_GET_CHANNELLIST_ACK()
	{
		// Last Update : 2015-10-15 15:46:48
		_SetValue( m_ui8ChannelCount );
		_SetValue( m_ui16MaxUserCount );
		_SetValue( m_ui16NowUserCount );
		_SetValue( m_stChannelBonus );
	}
};

struct PACKET_SELECT_CHANNEL_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_SELECT_CHANNEL_REQ>
{
	S2MOValue<UINT16>									m_ui16ChannelIdx;		// 선택한 채널 번호

	PACKET_SELECT_CHANNEL_REQ()
	{
		_SetValue( m_ui16ChannelIdx );
	}
};

struct PACKET_SELECT_CHANNEL_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_SELECT_CHANNEL_ACK>
{
	S2MOValue<UINT16>									m_ui16ChannelNum;
	S2MOStringW<NOTICE_STRING_MAX_COUNT>				m_strNormalNotice;

	PACKET_SELECT_CHANNEL_ACK()
	{
		// Last Update : 2015-10-15 15:27:05
		_SetValue( m_ui16ChannelNum );
		_SetValue( m_strNormalNotice );
	}
};

struct PACKET_BASE_GUIDE_COMPLETE_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_GUIDE_COMPLETE_ACK>
{
	S2MOValue<UINT16>									m_ui16GuideEventIdx;
	S2MOValue<UINT64>									m_ui64GuideComplete;
	S2MOValue<UINT32>									m_ui32GetPoint;
	S2MOValue<UINT32>									m_ui32Point;

	PACKET_BASE_GUIDE_COMPLETE_ACK()
	{
		// Last Update : 2015-10-15 15:21:00
		_SetValue( m_ui16GuideEventIdx );
		_SetValue( m_ui64GuideComplete );
		_SetValue( m_ui32GetPoint );
		_SetValue( m_ui32Point );
	}
};

struct PACKET_BASE_GET_USER_DETAIL_INFO_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_GET_USER_DETAIL_INFO_ACK>
{
	S2MOValue<S2_USER_DETAIL_INFO>			m_UserDetailInfo;

	PACKET_BASE_GET_USER_DETAIL_INFO_ACK()
	{
		// Last Update : 2015-10-14 18:26:18
		_SetValue( m_UserDetailInfo );
	}
};

struct PACKET_BASE_GET_UID_LOBBY_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_GET_UID_LOBBY_ACK>
{
	S2MOValue<T_UID>						m_TUID;

	PACKET_BASE_GET_UID_LOBBY_ACK()
	{
		// Last Update : 2015-10-14 18:24:33
		_SetValue( m_TUID );
	}
};

struct PACKET_BASE_GET_UID_ROOM_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_GET_UID_ROOM_ACK>
{
	S2MOValue<T_UID>						m_TUID;

	PACKET_BASE_GET_UID_ROOM_ACK()
	{
		// Last Update : 2015-10-14 18:23:48
		_SetValue( m_TUID );
	}
};

struct PACKET_BASE_CHANGE_PCCAFE_STATUS_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_CHANGE_PCCAFE_STATUS_ACK>
{
	S2MOValue<UINT8>						m_ui8PCCafe;

	PACKET_BASE_CHANGE_PCCAFE_STATUS_ACK()
	{
		// Last Update : 2015-10-14 18:24:15
		_SetValue( m_ui8PCCafe );
	}
};


struct PACKET_BASE_GET_USER_SUBTASK_REQ : public S2MOPacketBaseT<PROTOCOL_BASE_GET_USER_SUBTASK_REQ>
{
	S2MOValue<INT32>						m_i32SessionIdx;
	
	PACKET_BASE_GET_USER_SUBTASK_REQ()
	{
		_SetValue( m_i32SessionIdx );
	}
};

struct PACKET_BASE_GET_USER_SUBTASK_ACK : public S2MOPacketBaseResultT<PROTOCOL_BASE_GET_USER_SUBTASK_ACK>
{
	S2MOValue<INT32>						m_i32SessionIdx;
	S2MOValue<UINT8>						m_ui8SubTask;	// GAME_TASK_SUB

	PACKET_BASE_GET_USER_SUBTASK_ACK()
	{
		_SetValue( m_i32SessionIdx );
		_SetValue( m_ui8SubTask );
	}
};

struct PACKET_BASE_MEGAPHONE_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_MEGAPHONE_ACK>
{
	S2MOStringW<NET_NICK_NAME_SIZE>			m_strNickName;
	S2MOStringW<MAX_CHATTING_COUNT>			m_strChatMessage;	

	PACKET_BASE_MEGAPHONE_ACK()
	{
		_SetValue( m_strNickName );
		_SetValue( m_strChatMessage );
	}
};


struct PACKET_BASE_GET_CHARA_INFO_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_GET_CHARA_INFO_ACK>
{
	S2MOValue<UINT8>												m_ui8PacketIdx;		// 이번 패킷의 번호
	S2MOValue<S2MO_CHAR_INFO, S2MO_MULTI_SLOT_PACKET_COUNT>			m_aChara;

	PACKET_BASE_GET_CHARA_INFO_ACK()
	{
		_SetValue( m_ui8PacketIdx );
		_SetValue( m_aChara );		
	}
};

struct PACKET_NOTICE_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_NOTICE_ACK>
{
	S2MOStringW<NOTICE_STRING_COUNT>	m_strSlidingNotice;
	S2MOStringW<NOTICE_STRING_COUNT>	m_strChatNotice;
	S2MOValue<INT32>					m_i32SlidingRGB;
	S2MOValue<INT32>					m_i32ChatRGB;

	PACKET_NOTICE_ACK()
	{
		_SetValue(m_strSlidingNotice);
		_SetValue(m_strChatNotice);
		_SetValue(m_i32SlidingRGB);
		_SetValue(m_i32ChatRGB);		
	}
};

struct PACKET_BASE_MAP_RULELIST_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_MAP_RULELIST_ACK>
{
	S2MOValue<MAP_RULE_DATA, RULE_DATA_MAX>	m_aRuleData;


	PACKET_BASE_MAP_RULELIST_ACK()
	{
		// Last Update : 2016-04-20 14:29:39
		_SetValue( m_aRuleData );
	}
};

struct PACKET_BASE_MAP_MATCHINGLIST_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_MAP_MATCHINGLIST_ACK>
{
	S2MOValue<UINT16>										m_ui16StageMatchingCount;
	S2MOValue<UINT16>										m_ui16StartIdx;
	S2MOValue<BOOL>											m_bLastMatchingList;
	S2MOValue<MAP_STAGE_MATCHING, STAGE_MATCHING_SEND_MAX>	m_aStageMatching;
	
	PACKET_BASE_MAP_MATCHINGLIST_ACK()
	{
		// Last Update : 2016-04-20 14:35:29
		_SetValue( m_ui16StageMatchingCount );
		_SetValue( m_ui16StartIdx );
		_SetValue( m_bLastMatchingList );
		_SetValue( m_aStageMatching );
	}
};
struct PACKET_URL_LIST_ACK : public S2MOPacketBaseT<PROTOCOL_BASE_URL_LIST_ACK>
{
	S2MOValue<UINT8>								m_ui8URLVersion;
	S2MOValue<URL_LIST, URL_MAX_COUNT>				m_ulURLList;

	PACKET_URL_LIST_ACK()
	{
		_SetValue(m_ulURLList);
	}
};

#pragma pack( pop )

#endif