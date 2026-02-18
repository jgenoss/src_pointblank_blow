#ifndef __MODULE_CAST_H__
#define __MODULE_CAST_H__

#include "UserSession.h"

class CModuleCastSocket;

class CModuleCast : public i3Thread
{
	I3_CLASS_DEFINE( CModuleCast );

private:
	BOOL					m_bRunning;
	UINT32					m_ui32ThreadCount;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	CModuleCastSocket*		m_pSocket;
	UINT32					m_ui32ConnectTime; 

	// Main, UserSession 에서만 접근이 가능합니다.
	CONNECT_SERVER_NET_BUFFER		m_sMainMessage;
	CONNECT_SERVER_NET_BUFFER*		m_pUserMessage;

protected:   

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Cast
	INT32					_Connect();
	INT32					_Update(void); 

public: 
	CModuleCast(void); 
	virtual ~CModuleCast(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common
	INT32					OnCreate( UINT32 ui32IP, UINT16 ui16Port, UINT32 ui32ThreadCount );
	void					OnDestroy(void);
	virtual UINT32			OnRunning( void * pUserData);

	void					SendPacketMessage( i3NetworkPacket *SendPacket );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Battle Server 
	BOOL					BattleCreate(		INT32 WorkIdx, TYPE_ROOM_INFO * pRoomInfo, UINT32 ui32StartUser );
	BOOL					BattleDestroy(		INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx ); 
	BOOL					BattleUserEnter(	INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT32 ui32Item, CUserSession* pSession); 
	BOOL					BattleUserLeave(	INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, CUserSession* pSession); 
	BOOL					BattleRoundStart(	INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, TYPE_ROOM_INFO * pTypeRoomInfo, UINT8 ui8AtkDefChange );
	BOOL					BattleChangeDinoInfo(INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT16 ui16Slot, TYPE_ROOM_INFO* pInfo );
	BOOL					BattleWeaponInfo(	INT32 WorkIdx,	BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, GC_PROTOCOL_SLOT_EQUIPMENT* pStruct );
	BOOL					BattleRespawn(		INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT32 * pui32WeaponList, INT32 i32RespawnIdx, INT32 i32RespawnCount );

	BOOL					BattleGMPause(		INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, bool bPause );
	BOOL					BattleQAAuto(		INT32 WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT8 ui8CommandID, INT32* pi32Arg );
};

extern CModuleCast * g_pModuleCast; 

#endif
