#ifndef __PROTOCOL_SERVER_MEDAL_STRUCT_H__
#define __PROTOCOL_SERVER_MEDAL_STRUCT_H__

#pragma pack( push, 1)

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_NEW_SS_GET_USER_INFO_MEDAL_ACK,	PROTOCOL_NEW_SS_USER_ENTER_MEDAL_ACK // 메달정보
//////////////////////////////////////////////////////////////////////////
struct	PACKET_NEW_SS_GET_USER_INFO_MEDAL_ACK
{
	INT32				m_i32SessionIdx;
	T_UID				m_i64UID;
	INT32				m_i32Rv;	


};

struct PACKET_D_GET_USER_INFO_MEDAL
{
	NOTIFY_MEDAL		m_aNotifyMedal[MAX_NOTIFY_MADAL_COUNT];	// 알리미 등록 메달.	
	UINT16				m_ui16MedalCount;						// 일반 메달 갯수.
	UINT16				m_ui16CurMedalSetCount;					// 진행중인 메달 셋트 갯수.
	UINT16				m_ui16ComMedalSetCount;					// 완료된 메달 셋트 갯수.
};

struct DB_SHOP_ITEM
{
	INT32				m_i32Rv;			// 성공 여부.  
	T_UID				m_TUID;				// UserDBIdx
	DB_ITEM				m_stDBItem;
	DB_GACHA			m_stDBGacha;
	MEDAL_REWARD_INFO	m_stMedalReward;
	TTCHAR				m_strSenderNick[NET_NICK_NAME_SIZE];
	TTCHAR				m_strMessage[MAX_GIFT_MESSAGE_SIZE];
	UINT32				m_ui32CapsuleItemID;
};

#pragma pack(pop)

#endif