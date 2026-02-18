#ifndef __MODULE_CONTROL_H__
#define __MODULE_CONTROL_H__

//User PDH
#include <pdh.h>
#include <pdhmsg.h>
#include "UserSession.h"

struct PDH_STATUS_INFO
{
	HQUERY					m_hQuery;
	HCOUNTER				m_hCounter;
};

class CModuleControlSocket;
struct TYPE_ROOM_INFO;

class CModuleControl : public i3Thread
{
	I3_CLASS_DEFINE( CModuleControl );

private:
	BOOL					m_bRunning;
	UINT32					m_ui32ThreadCount;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Control
	CModuleControlSocket*	m_pControlSocket;
	UINT32					m_ui32ControlConnectTime; 
	UINT32					m_ui32ControlSystemInfoSendTime;

	// Control 은 Main, UserSession 에서만 접근이 가능합니다.
	SERVER_NET_BUFFER		m_sControlMainMessage;
	SERVER_NET_BUFFER*		m_pControlUserMessage;

	PDH_STATUS_INFO			m_stCPU;
	PDH_STATUS_INFO			m_stHandle;

protected:

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Control
	INT32					_ControlConnect();
	INT32					_ControlUpdate(void); 

public: 
	CModuleControl(void); 
	virtual ~CModuleControl(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common
	INT32					OnCreate( UINT32 ui32ThreadCount );
	void					OnDestroy(void);
	virtual UINT32			OnRunning( void * pUserData);

	BOOL					InitSystemLog();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Battle Server 
	BOOL	BattleCreate(		INT32 i32WorkIdx, TYPE_ROOM_INFO * pRoomInfo, UINT32 ui32StartUser );
	BOOL	BattleDestroy(		INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx ); 
	BOOL	BattleUserEnter(	INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT32 ui32Item, CUserSession* pSession); 
	BOOL	BattleUserLeave(	INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, CUserSession* pSession); 
	BOOL	BattleRoundStart(	INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, TYPE_ROOM_INFO * pTypeRoomInfo, bool bTeamChange );
	BOOL	BattleChangeDinoInfo(INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT16 ui16Slot, TYPE_ROOM_INFO* pInfo );
	BOOL	BattleWeaponInfo(	INT32 i32WorkIdx,	BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, GC_PROTOCOL_SLOT_EQUIPMENT* pStruct );
	BOOL	BattleRespawn(		INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, UINT8 ui8SlotIdx, UINT32 * pui32WeaponList, INT32 i32RespawnIdx, INT32 i32RespawnCount );
	BOOL	BattleGMPause(		INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32GroupIdx, bool bPause );

	void	SendPacketMessage( i3NetworkPacket *SendPacket );

};

extern CModuleControl * g_pModuleControl; 

#endif
