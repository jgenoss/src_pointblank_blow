#ifndef __COMMON_BATTLE_DEF_H__
#define __COMMON_BATTLE_DEF_H__

#pragma pack( push, 1)
struct BATTLE_ROOM_INFO
{
	UINT8	_cBattle;
	UINT8	_cGame;
	UINT8	_cChannel;
	UINT16	_sRoom;
	void	Reset()
	{
		_cBattle	= 0;
		_cGame		= 0;
		_cChannel	= 0;
		_sRoom		= 0;
	}
	void	Copy( BATTLE_ROOM_INFO * pDes )
	{
		_cBattle	= pDes->_cBattle;
		_cGame		= pDes->_cGame;
		_cChannel	= pDes->_cChannel;
		_sRoom		= pDes->_sRoom;
	}

};

/*template <PROTOCOL P=0>
struct P_BATTLE_ROOM_BASE
{
private:
	char*					m_pBuffer;
	char*					m_pBufferPos;
	UINT16					m_ui16Size;

	UINT16*					m_pui16Size;
	PROTOCOL*				m_pProtocolID;

public:
	BATTLE_ROOM_INFO		_RoomInfo;

protected:
	void					_Init( char* pBuffer )
	{
		m_pBuffer		= pBuffer;
		m_pui16Size		= (UINT16*)m_pBuffer;
		m_pProtocolID	= (PROTOCOL*)m_pBuffer[ sizeof(UINT16) ];
		m_pBufferPos	= &m_pBuffer[ SOCKET_HEAD_SIZE ];
	}
	void					_ReadData( void* pData, UINT16 ui16Size )
	{
		i3mem::Copy( pData,			m_pBufferPos,	ui16Size );
		m_pBufferPos	+= ui16Size;
	}
	void					_WriteData( void* pData, UINT16 ui16Size )
	{
		i3mem::Copy( m_pBufferPos,	pData,			ui16Size );
		m_pBufferPos	+= ui16Size;
		*m_pui16Size	+= ui16Size;
	}

	virtual BOOL			_Packeting_H( char* pBuffer )
	{
		_Init( pBuffer );

		*m_pui16Size	= 0;
		*m_pProtocolID	= P;

		return TRUE;
	}
	virtual BOOL			_UnPacketing_H( char* pBuffer )
	{
		_Init( pBuffer );

		return TRUE;
	}

public:
	UINT16					GetSize()
	{
		return m_ui16Size;
	}

	virtual BOOL			Packeting( char* pBuffer )
	{
		_Packeting_H( pBuffer );

		_WriteData( &_RoomInfo,		sizeof(BATTLE_ROOM_INFO) );

		return TRUE;
	}
	virtual BOOL			UnPacketing( char* pBuffer )
	{
		_UnPacketing_H( pBuffer );

		_ReadData( &_RoomInfo,		sizeof(BATTLE_ROOM_INFO) );

		return TRUE;
	}
};
*/

struct P_BATTLE_USER_INFO
{
	SLOT_INFO				m_aSlotInfo;
	UINT32					m_TCharItemID;
	UINT32					m_TEqiuipmentItemID[ CHAR_EQUIPMENT_PARTS_COUNT ];
	S2MO_CHAR_SKILL_INFO	m_CharSkillInfo;
	T_UID					m_TUID;
	UINT8					m_ui8Title[ MAX_EQUIP_USER_TITLE ];
};

struct P_BATTLE_ROOM_CREATE_REQ
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32					_iStage; 
	UINT32					_iStartUser;			// 시작 유저들 (비트 번호 = slot Index)
	UINT32					_ui32BitHackOffUser;	// 핵탐지 제외 유저들	
	UINT8					_ui8AvailableWeapon;	// 해당룸에서 사용가능한 무기
	UINT8					_SubType;				// Time, Kill ( 라운드 종료 조건 )
	UINT8					_ui8InfoFlag;			// 방 설정 정보(자동공수교대 사용여부 정보 들어있음)
	bool					_bCanSoloPlay;			// 솔로 플레이 가능여부 (QA 자동화)
	UINT8					m_ui8AICount;
	UINT8					m_ui8MaxPerson;
	//P_BATTLE_USER_INFO		m_UserInfo[ SLOT_MAX_COUNT ];
}; 

struct P_BATTLE_USER_ENTER_REQ
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32					_wGroupIdx;
	UINT8					_cSlotIdx;
	UINT32					_iItem;
	bool					_bHackOff;
	T_ItemID				m_TPartsID[ CHAR_EQUIPMENT_PARTS_COUNT ];
	CHARA_SLOT_SKILL		m_CharSkills;
	UINT8					_aui8Designation[MAX_EQUIP_USER_TITLE];		// 호칭
	T_UID					_i64UID;
	UINT8					m_ui8PBVer;		// PB_VER
};

struct P_BATTLE_USER_LEAVE_REQ
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32					_wGroupIdx;
	UINT8					_cSlotIdx;
};

struct P_BATTLE_QA_AUTO_REQ
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT8					_ui8SlotIdx;
	UINT32					_ui32GroupIdx;
	UINT8					_ui8CommandId;
	INT32					_i32Arg[ MAX_COUNT_QA_AUTO_ARG ];
};

struct P_BATTLE_ROUND_REQ
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32				_GroupIdx; 
	UINT8				_iRound;
	UINT32				_aui32Item[SLOT_MAX_COUNT];
	T_ItemID			m_TPartsID[SLOT_MAX_COUNT][ CHAR_EQUIPMENT_PARTS_COUNT ];
	UINT8				_aui8Designation[SLOT_MAX_COUNT][MAX_EQUIP_USER_TITLE];
}; 

struct P_BATTLE_RESPAWN_REQ
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32				_GroupIdx; 
	UINT8				_cSlotIdx;
	UINT32				_pUseWeaponList[ CHAR_EQUIPMENT_WEAPON_COUNT ];
	INT32				_i32TotalRespawnIdx;
	INT32				_i32RespawnCount;
};

struct P_BATTLE_M_DINO_INFO_REQ
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32				_GroupIdx;
	UINT16				_iSlotIdx; 
	UINT32				_ui32Item;
	T_ItemID			m_TPartsID[ CHAR_EQUIPMENT_PARTS_COUNT ];
	UINT8				_aui8Designation[MAX_EQUIP_USER_TITLE];
};

struct P_BATTLE_ROOM_DESTROY_REQ
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32				_GroupIdx; 
}; 

struct P_BATTLE_ROOM_CREATE_ACK
{
	BATTLE_ROOM_INFO		_RoomInfo;
	INT32					m_i32Result;
	UINT8					_BattleType;
	UINT32					_GroupIdx; 
	UINT32					_Key; 
	UINT32					_Ip;
	UINT16					_Port; 
};

struct P_BATTLE_ROOM_KILL_ACK
{
	BATTLE_ROOM_INFO		_RoomInfo;
	DEATH_INFO_HEADER		_killHeader; 
};

struct P_BATTLE_M_BOMB_INSTALL_ACK
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32					_iSlotIdx; 
	UINT8					_BombPosition; 
	REAL32					_BombVecPos[3];
}; 

struct P_BATTLE_M_BOMB_UNINSTALL_ACK
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32					_iSlotIdx; 
};

struct P_BATTLE_M_DESTRUCTION_HP_ACK
{
	BATTLE_ROOM_INFO		_RoomInfo;
	DESTRUCTION_INFO		_DestructionInfo; 
};

struct P_BATTLE_M_DEFENCE_ACK
{
	BATTLE_ROOM_INFO		_RoomInfo;
	DEFENCE_INFO			_DefenceInfo;
};

struct STRUCT_BATTLE_SLOT_EQUIPMENT_ACK
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32					m_ui32GroupIdx;
	UINT8					m_ui8SlotIdx;
	
	// 하단의 두 멤버는 GC_PROTOCOL_SLOT_EQUIPMENT 구조체의 멤버입니다.
	// 데디케이션서버까지 전송된 STRUCT_BATTLE_SLOT_EQUIPMENT_REQ 정보는 다시 전부 되돌아올필요없이 게임서버에서
	// 다시 복구할수 없는 정보인 하단의 두 멤버만 돌려줍니다. 52 Bytes -> 6 Bytes 용량절약
	UINT16					m_ui16ChangeFlag;
	UINT32					m_ui32EquipItem;
};

struct P_BATTLE_HACK_USER_ACK
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32					_uCount; 
}; 

struct P_BATTLE_M_TOUCHDOWN_REQ
{
	BATTLE_ROOM_INFO		_RoomInfo;
	UINT32					_GroupIdx; 
	UINT8					_cSlotIdx;
	UINT16					_cObjectIdx;
};

struct STRUCT_BATTLE_SLOT_EQUIPMENT_REQ
{
	BATTLE_ROOM_INFO			_RoomInfo;
	UINT32						m_ui32GroupIdx;
	UINT8						m_ui8SlotIdx;
	GC_PROTOCOL_SLOT_EQUIPMENT	m_Struct;	// 52 Bytes
};

#pragma pack( pop)

#endif
