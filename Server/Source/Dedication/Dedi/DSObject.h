#ifndef __DS_OBJECT_H__
#define __DS_OBJECT_H__

#include "DSObjectBase.h"

enum SEND_PACKET_TYPE
{
	SEND_PACKET_TYPE_LIVE,
	SEND_PACKET_TYPE_SYNC,
};

enum OBJECT_USE_TYPE
{
	OBJECT_USE_TYPE_NONE,
	OBJECT_USE_TYPE_HAVE,
	OBJECT_USE_TYPE_RIDING,
	OBJECT_USE_TYPE_RESPAWN,
	OBJECT_USE_TYPE_SWITCH,

	OBJECT_USE_TYPE_MAX,
};

enum OBJECT_RESET_POSITION
{
	OBJECT_RESET_POSITION_NONE,
	OBJECT_RESET_POSITION_RESPAWN,
	OBJECT_RESET_POSITION_DIRECT,
};

enum SEND_HEAD_TYPE
{
	SEND_HEAD_TYPE_CHAR,
	SEND_HEAD_TYPE_OBJ_STATIC,
	SEND_HEAD_TYPE_OBJ_MOVE,
	SEND_HEAD_TYPE_OBJ_DYNAMIC,
	SEND_HEAD_TYPE_OBJ_ANIM,
	SEND_HEAD_TYPE_OBJ_CONTROLED,
	SEND_HEAD_TYPE_MISSION,
};

enum OBJECT_USE_ITEM_TYPE
{
	OBJECT_USE_ITEM_NONE = 0,
	OBJECT_USE_ITEM_WEAPON,
	OBJECT_USE_ITEM_HPUP,
	OBJECT_USE_ITEM_RECOVERY_GRAVITY,
	OBJECT_USE_ITEM_UNBEATABLE,

	OBJECT_USE_ITEM_MAX,
};

enum OBJECT_USE_ITEM_STATE
{
	OBJECT_USE_ITEM_STATE_OFF = 0,
	OBJECT_USE_ITEM_STATE_ON = 1,

	OBJECT_USE_ITEM_STATE_MAX,
};

class CDediRoom;
class CCharacter;

class CDSObject : public DS_GAME_OBJECT, public GAMEINFO_OBJECT_NETDATA
{
private:
	//REAL32					m_r32RespawnTimer;				// 리스폰 타이머 : 계산 값
		
	REAL32					m_r32StateChangeTime;			// Event 발생으로 인한 Time

	REAL32					m_r32FirstAnimationSyncTime;	
public:
		
	SEND_HEAD_TYPE			m_eSendHeadType;

	UINT8					m_ui8PosState;					// 위치에 따른 State

	INT16					m_i16ExplosionHP[ 3 ];			// 폭발 할 HP 최소값 설정
	
	N_POS					m_vec16OrgPos;
	//N_POS					m_vec16Pos;

	OBJECT_USE_TYPE			m_eUseType;
	//UINT8					m_ui8SlotIdx;					// 오브젝트를 활성화 시킨 유저, 사용자 : 0xff 이면 not used
	
	INT8					m_i8TeamIdx;					// 이 오브젝트의 소유 TeamIdx;
public:

	BOOL					m_bUDPSend;						// Event 발생으로 인한 UDP 전송Flag

	//UINT8					m_ui8ShellCount;	// stage info 보낼 때 state로 사용됨.
	BOOL					m_bItemStartTimerEnable;	
	BOOL					m_bTimerEnable;		// 누군가 주웠을 때 시작. 리스폰 후 리셋.
	UINT32					m_ui32StartTime;	// 주운 시간
	////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////
	// DS_ANIMATION_OBJECT
	//REAL32					m_r32PlayTime;		// current play time
	REAL32					m_r32StartTime;		// animation start time
	BOOL					m_bPathKeyChanged;	// pathkey changed

	REAL32					m_rKeyFrame;
	////////////////////////////////////////////////////////////////////////////////

	REAL32					m_r32LastSendTime;

	T_OBJ					m_TObjectType;
	T_OBJ					m_TObjectSyncType;

	UINT8					m_ui8PathKeyMax;			// riding 헬리콥터에서만 사용 ( random patchkey max )

	OBJECT_RESET_POSITION	m_eResetPosition;

	REAL32					m_fActionDistance;

	BOOL					m_bAnimRepeat;

public:
	CDSObject();
	virtual ~CDSObject();

	void					Init();
	void					Reset();
	// inline utility
	void					CopyTo( CDSObject* pObject );

	void					OnUpdate();

	// get / set
	INT16	GetMaxHP()													{	return m_i16MaxHP;																}
	
	INT16	AddHP( INT16  i16Inc )										{	_nHP += i16Inc;							CheckHPState();		return _nHP;		}
	INT16   SubHP( INT16  i16Dec )										{	_nHP = _nHP>i16Dec?_nHP-i16Dec:0;		CheckHPState();		return _nHP;		}
	void	SetExplosionHP( INT32 i32Idx, INT16 i16HP )					{	m_i16ExplosionHP[ i32Idx ] = i16HP;												}
	void	CheckHPState()
	{
		if( m_i16ExplosionHP[ _nState ] >= _nHP )
		{
			_nState++;
		}
	}

	void					SetHeadType( SEND_HEAD_TYPE eHeadType )				{	m_eSendHeadType = eHeadType;											}

	OBJECT_USE_TYPE			GetObjectUseType()									{	return m_eUseType;														}
	void					SetObjectUseType( OBJECT_USE_TYPE eType )			{	m_eUseType = eType;														}
	UINT16					GetSType()											{	return m_iSType;														}

	T_OBJ					GetObjectType()										{	return m_TObjectType;													}
	void					SetObjectType( T_OBJ TObjectType )					{	m_TObjectType = TObjectType;											}
	void					AddObjectType( T_OBJ TObjectType )					{	m_TObjectType |= TObjectType;											}

	T_OBJ					GetSyncObjectType()									{	return m_TObjectSyncType;												}
	void					SetSyncObjectType( T_OBJ TObjectType )				{	m_TObjectSyncType = TObjectType;										}
	void					AddSyncObjectType( T_OBJ TObjectType )				{	m_TObjectSyncType |= TObjectType;										}
	////////////////////////////////////////////////////////////////////////////////
	// 오브젝트가 사용중임을 표시
	void					ResetUse( N_POS* pvec32Pos = NULL );
	BOOL					SetUse( UINT32 ui32SlotIdx );	
	BOOL					IsUsed()											{	return (0xFF != _EnterCharaIdx);										}
	BOOL					GetCanUse( UINT8 ui8SlotIdx )						{	return (0xFF == _EnterCharaIdx) || (ui8SlotIdx == _EnterCharaIdx);		}

	void					SubShellCount( INT32 i32Count )						{	_nShellCount -= i32Count;												}

	void					ResetPosion()										{	_vPos = m_vec16OrgPos;													}
	void					SetPosition( N_POS* pPos )							{	_vPos = *pPos;															}

	//REAL32					GetRespawnTimer()									{	return m_r32RespawnTimer;												}
	void					SetRespawnTimer( REAL32 r32Time );									
	
	void					SetEventUser( UINT8 ui8SlotIdx )					{	m_bUDPSend = TRUE;		setEventSender(	ui8SlotIdx );					}

	void					ResetUDPSend()										{	m_bUDPSend = FALSE;														}
	////////////////////////////////////////////////////////////////////////////////
	REAL32					GetDistance( N_POS* pPosition );

	BOOL					CheckActionDistance( N_POS* pPosition );

	void					SetAnimRepeat( BOOL bRepeat )						{	m_bAnimRepeat = bRepeat;												}
	BOOL					GetAnimRepeat()										{	return m_bAnimRepeat;													}

	BOOL					MakePacket( char* pBuffer, INT32 i32MaxSize, UINT32& ui32WriteSize, SEND_PACKET_TYPE ePacketType );

	BOOL					CheckActionDistanceSpace(N_POS* pPosition);
	REAL32					GetDistanceSpace(N_POS* pPosition);
	

	INT32					ProcessUseItem(INT32 UseItemProbability, CDediRoom* pRoom = NULL);
	BOOL					_UseItem(CDediRoom* pRoom, OBJECT_USE_ITEM_TYPE eType, INT32 UseItemidx);
	OBJECT_USE_ITEM_TYPE	_GetUseItemType(INT32 UseItemidx);
};

#endif