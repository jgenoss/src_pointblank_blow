#ifndef __PROTOCOL__STRUCT_H__
#define __PROTOCOL__STRUCT_H__

#pragma pack( push, 1)

// 서버에서 일방적으로 던지는 패킷

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_SERVER_MESSAGE_CONNECTIONSUCCESS
{
	INT32		m_ui32SessionIdx;
	UINT32		m_ui32ConIp;
	UINT16		m_ui16ClientPort;
	INT16		m_i16PacketRandSeed;
#ifdef USE_GPK
	INT32		m_i32GPKCodeCount;
#endif

	//////////////////////////////////////////////////////////////////////////
	// Function
	void		Function(	INT32		ui32SessionIdx,
		UINT32		ui32ConIp,
		UINT16		ui16ClientPort,
		INT16		i16PacketRandSeed							
		)
	{
		m_ui32SessionIdx		= ui32SessionIdx;
		m_ui32ConIp				= ui32ConIp;
		m_ui16ClientPort		= ui16ClientPort;
		m_i16PacketRandSeed		= i16PacketRandSeed;		
	}
}; 


//////////////////////////////////////////////////////////////////////////
// PROTOCOL_SERVER_MESSAGE_NEW_CHANGE_INVENTORY
//////////////////////////////////////////////////////////////////////////
struct PACKET_H_SERVER_MESSAGE_NEW_CHANGE_INVENTORY
{
	UINT16		m_ui16CharaPartsInfo;			// 파츠류 장착할 캐릭슬롯 bit Flag(eSD_CHARA_SLOT_INFO)
	UINT16		m_ui16CharaWeaponInfo;			// 무기류 장착할 캐릭슬롯 bit Flag(eSD_CHARA_SLOT_INFO)
	UINT8		m_ui8CharaPartsCount;			// 파츠류 변경된 캐릭개수
	UINT8		m_ui8CharaWeaponCount;			// 무기류 변경된 캐릭개수
	UINT8		m_ui8CharaCommonPartsCount;		// 공통 파츠

};


struct PACKET_D_SERVER_MESSAGE_NEW_CHANGE_INVENTORY_CHARA_PARTS_INFO
{
	CHARA_PARTS_INFO	CharPartsInfo;
};

struct PACKET_D_SERVER_MESSAGE_NEW_CHANGE_INVENTORY_CHARA_WEAPON_INFO
{
	CHARA_WEAPON_INFO	CharWeaponInfo;
};

struct PACKET_D_SERVER_MESSAGE_NEW_CHANGE_INVENTORY_COMMON_PARTS_INFO
{
	COMMON_PARTS_INFO	CommonPartsInfo;
};
//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif