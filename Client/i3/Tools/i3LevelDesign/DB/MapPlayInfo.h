#pragma once

#include "Ado.h"
#include "MapPlayInfoDef.h"

enum DB_STATE
{
	DB_STATE_NONE,
	DB_STATE_ROOMLIST,
	DB_STATE_KILLINFO,
};

class CMapPlayInfo
{
	DB_STATE				m_eDBState;
	CADODatabase*			m_pDB;	

public:
	BOOL					_ChangePosToText( D3DVECTOR* v3Pos, char* strText );
	void					_InsertPos( D3DVECTOR* v3Pos, char* strPos, int i32InputType );
	void					_ChangeDateToUINT32( UINT32 ui32Date, COleDateTime* pdtTime );
	void					_MakeLocation( UINT64 ui64BattleUID, LOCATION_INFO* pLocation );

public:
	CMapPlayInfo();
	~CMapPlayInfo();

	BOOL					Create( __in char* strDataSource, __in char* strID, __in char* strPW );
	BOOL					Create( __in char* strConnectFile );
	void					Destroy();

	// Room List 얻어오기
	BOOL					GetRoomList( __in COleDateTime* ptmStart, __in COleDateTime* ptmEnd, __out INT32* pi32Count, __out PLAY_ROOM_LIST** pRoomList );

	BOOL					QueryRoomList( __in COleDateTime* ptmStart, __in COleDateTime* ptmEnd, __in UINT32 ui32MapID = 0 );
	INT32					GetRoomListCount();
	BOOL					GetRoomListInfo( __out PLAY_ROOM_LIST* pRoomList, __in INT32 i32BuffSize );

	// Kill List 얻어오기
	BOOL					GetKillList( __in COleDateTime* ptmStart, __in COleDateTime* ptmEnd, __in UINT64& ui64BattleUID, __out INT32* pi32Count, __out PLAY_KILL_LIST** pKillInfo );

	BOOL					QueryKillList( __in COleDateTime* ptmStart, __in COleDateTime* ptmEnd, __in UINT64& ui64BattleUID );
	INT32					GetKillListCount();
	BOOL					GetKillListInfo( __out PLAY_KILL_LIST* pKillInfo, __in INT32 i32BuffSize );
};