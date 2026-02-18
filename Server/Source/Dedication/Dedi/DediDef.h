// 
//
// YouJong Ha
//	Last update : 2010-10-06 (yyyy:mm:dd)
//	
// Description:
//	
//
//
// Dependency: 
//		
//	
#ifndef _DediDef_h
#define _DediDef_h

#include "DSObjectManager.h"
#include "GrenadeState.h"
#include "SpeedState.h"


inline
double distance_squared( N_POS* v1, N_POS* v2)
{
	VEC3D v1_, v2_;
	bool ret;

	ret = UDP_UTIL::ConvertNet3DtoVec3D( &v1_, v1 );
	I3ASSERT( ret == true );
	ret = UDP_UTIL::ConvertNet3DtoVec3D( &v2_, v2 );
	I3ASSERT( ret == true );
	
	double distance =	( v2_.x - v1_.x ) * ( v2_.x - v1_.x ) +
						( v2_.y - v1_.y ) * ( v2_.y - v1_.y ) +
						( v2_.z - v1_.z ) * ( v2_.z - v1_.z );
	
	I3ASSERT( _finite( distance ) != 0 ); // if zero that means overflow or nan
	
	return distance;
}

inline
double distance_squared2D( N_POS* v1, N_POS* v2)
{
	VEC3D v1_, v2_;
	bool ret;
	
	ret = UDP_UTIL::ConvertNet3DtoVec3D( &v1_, v1 );
	I3ASSERT( ret == true );
	ret = UDP_UTIL::ConvertNet3DtoVec3D( &v2_, v2 );
	I3ASSERT( ret == true );
	
	double distance =   ( v2_.x - v1_.x ) * ( v2_.x - v1_.x ) +
						( v2_.z - v1_.z ) * ( v2_.z - v1_.z );
	
	I3ASSERT( _finite( distance ) != 0 ); // if zero that means overflow or nan

	return distance;

}

inline
double distance_squared( N_POS_R* v1, N_POS_R* v2)
{
	double distance =   ( v2->x - v1->x ) * ( v2->x - v1->x ) +
						( v2->y - v1->y ) * ( v2->y - v1->y ) +
						( v2->z - v1->z ) * ( v2->z - v1->z );

	I3ASSERT( _finite( distance ) != 0 ); // if zero that means overflow or nan

	return distance;
}
inline
	double distance_squared( VEC3D * v1, VEC3D * v2)
{
	double distance =   ( v2->x - v1->x ) * ( v2->x - v1->x ) +
		( v2->y - v1->y ) * ( v2->y - v1->y ) +
		( v2->z - v1->z ) * ( v2->z - v1->z );

	I3ASSERT( _finite( distance ) != 0 ); // if zero that means overflow or nan

	return distance;
}

inline
BOOL IsFiniteNumber( REAL16 _h )
{
	UINT16 e = (*(UINT16*)&_h >> 10) & 0x001f; 
	return e < 31;
}

inline 
BOOL IsOverflow( N_POS* pPos )
{
	if(	! IsFiniteNumber( pPos->x ) )	return TRUE;
	if(	! IsFiniteNumber( pPos->y ) )	return TRUE;
	if(	! IsFiniteNumber( pPos->z ) )	return TRUE;
	return FALSE;
}

inline 
BOOL IsOverflow( N_ROT* pRot )
{
	if(	! IsFiniteNumber( pRot->Theta ) )	return TRUE;
	if(	! IsFiniteNumber( pRot->Rho ) )		return TRUE;
	if(	! IsFiniteNumber( pRot->Fan ) )		return TRUE;
	if(	! IsFiniteNumber( pRot->Weight ) )	return TRUE;
	return FALSE;
}

inline 
BOOL IsOverflow( const QUATERNION* pRot )
{
	if(	! IsFiniteNumber( pRot->x ) )	return TRUE;
	if(	! IsFiniteNumber( pRot->y ) )	return TRUE;
	if(	! IsFiniteNumber( pRot->z ) )	return TRUE;
	if(	! IsFiniteNumber( pRot->w ) )	return TRUE;
	return FALSE;
}

inline
	BOOL IsOverflow( const VEC3D* pPos)
{
	// http://www.johndcook.com/IEEE_exceptions_in_cpp.html
	if( ! IsFiniteNumber( pPos->x ) ) return TRUE;
	if( ! IsFiniteNumber( pPos->y ) ) return TRUE;
	if( ! IsFiniteNumber( pPos->z ) ) return TRUE;

	return FALSE;
}

// 불필요할 시 삭제 합니다. 김현우
inline 
BOOL CheckWeaponID( T_ItemID TItemID )
{
	// Check Slot
	WEAPON_SLOT_TYPE eSlot = WEAPON::ItemID2Slot( TItemID );
	if( WEAPON_SLOT_UNKNOWN >= eSlot )						return FALSE;
	if( WEAPON_SLOT_COUNT <= eSlot )						return FALSE;

	// Check Class
	WEAPON_CLASS_TYPE eClass = (WEAPON_CLASS_TYPE)GET_ITEM_SUBTYPE( TItemID );
	if( WEAPON_CLASS_UNKNOWN >= eClass )					return FALSE;
	if( WEAPON_CLASS_COUNT <= eClass )						return FALSE;

	return TRUE;
}

inline
BOOL IsFiniteNumber( REAL32 x )
{
	return (x <= FLT_MAX && x >= -FLT_MAX); 
}

inline
BOOL IsOverflow( N_POS_R* pPos)
{
	// http://www.johndcook.com/IEEE_exceptions_in_cpp.html
	if( ! IsFiniteNumber( pPos->x ) ) return TRUE;
	if( ! IsFiniteNumber( pPos->y ) ) return TRUE;
	if( ! IsFiniteNumber( pPos->z ) ) return TRUE;
	
	return FALSE;
}

#define C4_BOMB_TIME				42.5f	// 폭파 미션 C4 설치 후 폭파 시간 ( 원래 42초지만 0.5초 유예시간을 둠 )

#define INVALID_RELAY_GROUP_ID		(0xFFFFFFFF)
#define	INVALID_ROUND_NUMBER		(0xFF)
#define INVALID_SLOT_IDX			(-1)
#define	INVALID_OBJ_IDX				(-1)
#define INVALID_BULLET_ID			(-1)

#define	MAX_UDP_SOCKET_COUNT		10

#define DS_MAX_CODENAME				32
#define DS_MAX_SHARE_COLLISION		4

#define DS_MAX_OBJECT				256
#define DS_MAX_RESPAWN_OBJECT		32 
#define DS_MAX_RESPAWN_PER_TEAM		(DS_MAX_RESPAWN_OBJECT>>1) // 각각의 red, blue, npc red, npc blue 에 대해서 리스폰 가능한 위치

#define UDP_SEND_PACKET_SIZE		NET_MTU_SIZE
#define GROUP_DESTORY_TIME			120		// 2분 : 120

#define MAX_PACKET_UPDATE_TIME		255
// 방생성시, 난입시 stageinfo 정보를 보내는데 이 패킷이 손실되는 경우가 있다.
// 이 문제 때문에 패킷 뒷부분에 오브젝트 몇 개씩 append해서 전송한다. 
// 패킷 송신 바이트를 줄이기 위해서 정의된 시간 동안만 동작하게 하기 위함(2011-09-23)
// 참고: 게임시작시, 난입시 원래의 stage info는 그대로 보낸다.
#define DS_SEND_STAGEINFO_REMAIN_TIME			5.0f	

#define UPDATE_CHECK_TIME			1		// 1
#define UPDATE_CHECK_JUMP_COUNT		5		

#define BUFFERS_PER_TRACKBUFFER		1024  // allocated buffers per socket.
										  // each socket uses 2 track bufers; one for send, one for recv
										  // and each track buffer manages this number of buffers.

#define DS_STAGE_INFO_SEND_COUNT	1
#define	DS_SEND_LIFE_COUNT			5		// suicide 처리시 life를 보내는 회수

// ------------------------------------------------------------------
// Framework
// ------------------------------------------------------------------
#define DS_MAX_TASK_THREAD_COUNT		63	// max task processor threads	
#define DS_MAX_IOCP_THREAD_COUNT		32	// max iocp worker threads
#define DS_MAX_HACK_CHECKER_COUNT		63	// max cheating checker threads
#define DS_MAX_MONITOR_THREAD_COUNT		1	// max monitor threads

// ------------------------------------------------------------------
// Timeout
// ------------------------------------------------------------------
#define DS_TASK_TIMEOUT					64		// ms
#define DS_PHYSICS_TIMEOUT				16		// ms
#define DS_CHARACTER_RESPAWN_TIME		1.0f	// sec
#define DS_DEFENCE_TARGET_LOCKTIME		3.0f	// sec : 방어미션에서 공격자를 KILL했을 경우 보너스를 주기 위함. DEATH PACKET 만들 때 사용
#define DS_DESTROY_TARGET_LOCKTIME		3.0f	// sec : 파괴미션에서 ...
#define DS_HOWL_LOCKTIME				5.0f	// sec : 하울링 타임. 하울링공격을 받는 유저를 kill했을 때 assist kill을 적용하기 위함
#define DS_TOUCHDOWN_TIME				3.0f	// sec : 터치다운 성공후부터 3초동안 데미지 적용 안되도록 
#define DS_FLYING_TIME					5.0f	// sec : fly 없는 수류탄 검사용

#define DS_DEFAULT_ROOM_COUNT				640
#define DS_DEFAULT_PORT_COUNT				16		// control server에서 포트 정보를 얻기 위한 여유 공간
#define DS_DEFAULT_PORT_BASE				40000
#define DS_DEFAULT_IO_THREAD_COUNT			10
#define DS_DEFAULT_TASK_THREAD_COUNT		10
#define DS_DEFAULT_CHEATING_CHECKER_COUNT	10		// task thead count와 같아야 한다. // 단, on/off가 가능해야 하기 때문에 별도의 변수로 유지.
#define DS_MAX_TASK_THREAD_COUNT			64
													
#define DS_DEFAULT_MONITOR_THREAD_COUNT		1
#define DS_DEFAULT_LOG_LEVEL				0	// 0: all 1: warning 2: error

#define DS_MAX_PACKETS_PER_ROOM		16		// task processor에서 한번에 처리하는 패킷 수
#define DS_MAX_WEAPON_LIST			1024	// weapon list의 max size

// thread type
enum DS_THREAD_TYPE
{
	IOCP_WORKER = 0,
	TASK_PROCESSOR,
	TASK_SECURITY		
};

// 클라에서(GameCharaDef.h에 정의)
enum CHARA_STATE_FOR_CAM
{
	CSFC_UNKNOWN	= -1,
	CSFC_STAND,
	CSFC_SITMOVE,
	CSFC_SIT,
	CSFC_JUMP,
};

// ------------------------------------------------------------------
// 데디케이션 서버에서 게임서버에 보내는 정보를 위한 플래그
// ------------------------------------------------------------------
#define DS_SEND_USER_KILL				0x01UL
#define DS_SEND_MISSION_TRIGGER			0x02UL
#define DS_SEND_MISSION_DESTRUCTION		0x04UL
#define DS_SEND_MISSION_DEFENCE			0x08UL
#define DS_SEND_CHEATING_REPORT			0x10UL

enum ENUM_GROUP_STATE
{
	RGS_INVALID = -1,	// 초기화 되지 않았다
	RGS_READY,			// 초기화 완료
	RGS_USE,			// 멤버참여 대기
};

#pragma pack( push, 1 )

/////////////////////////////////////////////////////////////////////////////////////////
// game object
// : net index, pos, hp 정보를 유지한다.
#define OBJECT_S_TYPE_TARGET				0x0800
#define OBJECT_S_TYPE_W_BOX					0x0002 // NET_ACTIONKEY_USEOBJECT
#define OBJECT_S_TYPE_HELICOPTER			0x0100 // NET_ACTIONKEY_USEOBJECT, NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT, NET_ACTIONKEY_LEAVE_WEAPON_ON_OBJECT, ANIMATION SYNC 예외처리 용도	
#define OBJECT_S_TYPE_RESPAWN_HELICOPTER	0x0101 
#define OBJECT_S_TYPE_RIDING_HELICOPTER		0x0102
#define OBJECT_S_TYPE_REPAIR_TRAIN			0x0201	// animation 예외처리
#define OBJECT_S_TYPE_BARRICADE				0x0400	// baricate 예외처리 (상태값이 2개 이상인 경우: 정상, 반파, 완파)
#define OBJECT_S_TYPE_EXPLOSIVE_OBJ			0x1000	// 드럼통, 자동차, 소화기 인식 (TEST)
#define OBJECT_S_TYPE_ESCAPE				0x2000	// 드럼통, 자동차, 소화기인식 (TEST)
#define OBJECT_S_TYPE_CONVOY				0x4000	// 탈취 미션물
#define OBJECT_S_TYPE_JUMPHOLD				0x8000	// 점프대
#define OBJECT_S_TYPE_USEITEMBOX			0x0004	// Battle Item Box
//  ObjectRespawnHelicopter.h : helispot에서만 사용
//  ObjectRidingHelicopter.h  : outpost에서만  사용

enum DS_EXPLOSION_STATE
{
	DS_EXPLOSION_STATE_NORMAL, 
	DS_EXPLOSION_STATE_HALF_EXPLOSION,
	DS_EXPLOSION_STATE_EXPLOSION,
	DS_EXPLOSION_STATE_DAMAGE_APPLIED=99
};

///////////////////////////////////////////////////////////////////////////////////////////
// *.i3sobj 파일 구조
// ------------------
//	[ DS_I3SOBJ_HEADER ] 
//	[ DS_GAME_OBJECT   ] 
//	[ DS_GAME_OBJECT   ]
//	...
//	[ DS_GAME_OBJECT   ] : header의 count 만큼 반복 (배열)
//
#define I3SOBJ_VERSION		MAKELONG(1, 8)

struct DS_I3SOBJ_HEADER
{
	DWORD				size;			// this structure's size
	DWORD				version;		// file version
	DWORD				count;			// count of the game obj
	char				pad[4];			// reserved
};

///////////////////////////////////////////////////////////////////////////////////////////
// *.i3sRpn 파일 구조
// ------------------
//	[ DS_I3SOBJ_HEADER ] 
//	[ DS_RESPAWN_OBJECT   ] 
//	[ DS_RESPAWN_OBJECT   ]
//	...
//	[ DS_RESPAWN_OBJECT   ] : header의 count 만큼 반복 (배열)
//
#define I3SRPN_VERSION		MAKELONG(1, 0)

struct DS_RESPAWN_OBJECT 
{
	UINT8	m_ui8RespawnStyle;	// 0: CHARACTER_TEAM_RED, 1: CHARACTER_TEAM_BLUE, 2: NPC_TEAM_RED, 3:NPC_TEAM:BLUE
	char	m_pad[3];			// reserved
	REAL32	m_vPos[3];			// respawn position (x,y,z)

	void	Reset()
	{
		m_ui8RespawnStyle = 0;
		m_pad[0] = m_pad[1]= m_pad[2] = '\0';
		m_vPos[0] = m_vPos[1] = m_vPos[2] = 0.0f;
	}
	DS_RESPAWN_OBJECT()
	{
		Reset();
	}
};
///////////////////////////////////////////////////////////////////////////////////////////
// *.i3scol 파일 구조
// ------------------
//	[ DS_I3SCOL_HEADER	]
//	[ DS_I3SCOL_DESC	] 
//  [ DS_I3SCOL_DESC	]
//	....
//  [ DS_I3SCOL_DESC	]  : header의 count 만큼 반복.
//  [ vertex list ][face list][nxData] : 가변 길이
//  [ vertex list ][face list][nxData]
//  ....
//  [ vertex list ][face list][nxData]
//  

#define I3SCOL_VERSION1		MAKELONG(1, 0)
#define I3SCOL_VERSION2		MAKELONG(1, 1)

// DS_I3SCOL_HEADER
struct DS_I3SCOL_HEADER
{
	DWORD				size;		// this structure's size
	DWORD				version;	// file version
	DWORD				count;		// count of the collision obj. 
	char				pad[4];		// reserved
};

struct DS_I3SCOL_HEADER2 : public DS_I3SCOL_HEADER
{
	UINT32				m_ChecksumCode[4];
};

// DS_I3SCOL_DESC를 이용해서 NxShapeSet을 구성한다
struct DS_I3SCOL_DESC
{
	NxMat34				transform;			// matrix for pos, orientation, scale
	DWORD				shapeType;			// shape type (triangle mesh? convex?)
	DWORD				group;				// collision test group
	NxF32				restitution;		// for creating material
	NxF32				staticFriction;		// ..
	NxF32				dynamicFriction;	// ..
	// mesh data
	DWORD				toffset;	// terrain list position
	DWORD				tsize;		// terrain list size (bytes)
	DWORD				tstep;		// size of the list item
	DWORD				voffset;	// vertex list position 
	DWORD				vsize;		// vertex list size (bytes)
	DWORD				vstep;		// size of the list item  
	DWORD				foffset;	// face list position
	DWORD				fsize;		// face list size (bytes)
	DWORD				fstep;		// size of the list item
	// physX data
	DWORD				nxoffset;	// nxData offset
	DWORD				nxsize;		// nxData size (bytes)
};

struct DS_MISSION_DATA_DESTRUCTION
{
	CDSObject *				m_pObjectBlue;								// 룸 생성시에만 초기화 (예외)
	CDSObject *				m_pObjectRed;								// 룸 생성시에만 초기화 (예외)
	UINT16					m_ui16intervalRateBlue;						// 오브젝트에 대한 1%의 데미지 량
	UINT16					m_ui16intervalRateRed;						// 
	DESTRUCTION_INFO		m_ObjectRecvDamageInfo;						// Recv용
	DESTRUCTION_INFO		m_ObjectSendDamageInfo;						// only Host (Send용)
	INT32					m_ai32ObjectIntervalIdx[ SLOT_MAX_COUNT];
	INT32					m_i32RedObjectIntervalIdx;
	INT32					m_i32BlueObjectIntervalIdx;

public:
	void		reset( void)
	{
		INT32 i;

		m_ObjectRecvDamageInfo.Reset();
		m_ObjectSendDamageInfo.Reset();

		m_i32RedObjectIntervalIdx		= 1;
		m_i32BlueObjectIntervalIdx		= 1;

		if( m_pObjectRed != NULL )
		{	
			m_ObjectSendDamageInfo._nRedObjectHP = m_ObjectRecvDamageInfo._nRedObjectHP = (UINT16) m_pObjectRed->GetMaxHP();
		}

		if( m_pObjectBlue != NULL )
		{
			m_ObjectSendDamageInfo._nBlueObjectHP = m_ObjectRecvDamageInfo._nBlueObjectHP = (UINT16) m_pObjectBlue->GetMaxHP();
		}

		for( i = 0; i < SLOT_MAX_COUNT; i++)
		{
			m_ai32ObjectIntervalIdx[ i]			= 1;
		}
	}
};

struct  DS_MISSION_DATA_DEFENCE
{
public:
	void		reset( void)
	{
		INT32 i;

		
		m_DefenceObj_SendDamageInfo.Reset();

		
		for( i = 0; i < MAX_OBJECT_COUNT; i++)
		{
			if( m_pDefenceObj[ i] != NULL)
			{
				UINT16 hp = (UINT16) m_pDefenceObj[ i]->GetMaxHP();
				m_DefenceObj_SendDamageInfo.setObjectHP( i, hp);
			}
		}
	}

	CDSObject*			getDefenceObj( UINT32 ui32Idx ) { return ( ui32Idx < MAX_OBJECT_COUNT) ? m_pDefenceObj[ ui32Idx] : NULL;  }
	
	void				setDefenceObj( UINT32 ui32Idx, CDSObject* pObj)
	{
		if( ui32Idx < MAX_OBJECT_COUNT )
		{
			m_pDefenceObj[ ui32Idx] = pObj;
		}
	}
	
	DEFENCE_INFO *		getSendDamageInfo( void)		{ return &m_DefenceObj_SendDamageInfo; }

private :
	CDSObject*				m_pDefenceObj[ MAX_OBJECT_COUNT];
	DEFENCE_INFO			m_DefenceObj_SendDamageInfo;				// send ( only Host)
};

struct DS_CHECK_WALLSHOT_RESULT
{
	UINT32	m_ui32Result;
	UINT8	m_ui8ByteUID;
	UINT8	m_ui8SlotIdx;
	UINT8	m_ui8RoundNum;
	UINT8	m_ui8RespawnCount;

	void Reset()
	{
		m_ui32Result		= 0;
		m_ui8ByteUID		= 0;
		m_ui8SlotIdx		= (UINT8)-1;
		m_ui8RoundNum		= 0xff;
		m_ui8RespawnCount	= 0;
	}

	DS_CHECK_WALLSHOT_RESULT()
	{
		Reset();
	}
}; 

// 패킷 파싱 도중 서브 패킷 오류에 의해 일부만 상태 값이 변경되는 것을 방지하기 위한 구조체
class CDediRoom;

// action key rollback을 위한 sub flag
#define DS_OBJECT_MODIFIED	0x01
#define DS_M197_MODIFIED	0x02
#define DS_WEAPON_MODIFIED	0x04
#define DS_PRIMARY_MODIFIED	0x08

struct DS_PACKET_ROLLBACK
{
	UINT32					m_ui32InfoFlag;
	CDediRoom*				m_pRoom;
	UINT32					m_ui32SlotIdx;

	// user
	N_PCINFO_CHARA_STATE	m_state;
	BOOL					m_bIsOnLoad;

	UINT32					m_ui32FlagCollectPos;
	UINT32					m_ui32FlagChkRespawnPos;

	N_PCINFO_POSROTATION	m_pos;
	REAL32					m_r32PosRecvTime;
	
	N_PCINFO_WEAPON			m_syncWeapon;
	N_PCINFO_BULLET			m_bullet;
	UINT32					m_ui32PrimaryWeaponID;
	UINT32					m_ui32SecondaryWeaponID;

	UINT32					m_ui32ContinuousFireCount;
	UINT32					m_ui32FireCount;

	REAL32					m_r32FireCheckTime;
	REAL32					m_r32WeaponParamReceivedTime;
	REAL32					m_r32TriggerTime;	
	REAL32					m_r32LastFireTime;
	T_ItemID				m_TLastFireItemID;			// 마지막 ItemID
	WEAPON::ATTACK_TYPE		m_eLastFireAttackType;		// 마지막 AttackType

	//stage : use object, c4 status, get, drop
	//N_PCINFO_GETWEAPON		m_getWeapon;
	//N_PCINFO_GETWEAPON		m_dropWeapon;
	
	UINT32					m_ui32UseObjectIdx;
	UINT32					m_ui32UseObjectSlot;
	BOOL					m_bUseM197;

	//hit는 버려지면 적용되지 않아야 함.
	//수류탄은 고민 필요.
	//드랍웨폰도 고민 필요.

	void Reset()
	{
		m_ui32InfoFlag = 0;
		m_pRoom = NULL;
		m_ui32SlotIdx = 0;

		// user
		m_state.Reset();
		m_bIsOnLoad = FALSE;

		m_ui32FlagCollectPos	= 0;
		m_ui32FlagChkRespawnPos	= 0;

		m_pos.Reset();
		m_r32PosRecvTime		= 0.0f;
		
		m_syncWeapon.Reset();
		m_bullet.Reset();
		m_ui32PrimaryWeaponID	= 0;
		m_ui32SecondaryWeaponID	= 0;

		m_ui32ContinuousFireCount	= 0;
		m_ui32FireCount				= 0;

		m_r32FireCheckTime				= 0.0f;
		m_r32WeaponParamReceivedTime	= 0.0f;
		m_r32TriggerTime				= 0.0f;	
		m_r32LastFireTime				= 0.0f;

		//stage : use object, c4 status, get, drop
		//m_getWeapon.Reset();
		
		m_ui32UseObjectIdx	= (UINT32)-1;
		m_ui32UseObjectSlot	= (UINT32)-1;
		m_bUseM197			= FALSE;
	}
};

#pragma pack( pop )

//////////////////////////////////////////////////////////////////////////
// MAP MANAGER
#define DS_MAX_MAP			STAGE_UID_MAX		// 최대 맵 인덱스 
												// : 이 값 보다 작아야 한다. 현재 35개)

// path
#define MEDIA_PATH				"Media"					// base path
#define SOBJ_FILE_EXTENSION		"i3sobj"				// game object
#define SCOL_FILE_EXTENSION		"i3scol"				// collision object
#define SRPN_FILE_EXTENSION		"i3srpn"				// respawn object

BOOL ChangeAvailableWeapon( EWEAPON_TYPE eWeapon, T_ItemID& TItemID, WEAPON_SLOT_TYPE& eSlot );

//////////////////////////////////////////////////////////////////////////
// PHYSICS
#define DS_MAX_ACTORS		256		// 월드 하나에 포함할 수 있는 최대 액터 수

#define SAFE_DELETE(p)	{ if (p) { delete p; p = NULL; } }

inline BOOL	GetBit( const UINT16 ui16Value, UINT32 ui32SlotIdx )	{ return ui16Value & UINT16( 0x0001 << ui32SlotIdx );	}
inline void	SetBit( UINT16* pui16Value, UINT32 ui32SlotIdx )		{ *pui16Value |= UINT16(0x0001 << ui32SlotIdx );		}
inline void	ClearBit( UINT16* pui16Value, UINT32 ui32SlotIdx )		{ *pui16Value &= ~(0x0001 << ui32SlotIdx );				}

inline BOOL	GetBit( const UINT32 ui32Value, UINT32 ui32SlotIdx )	{ return ui32Value & UINT32( 1UL << ui32SlotIdx );		}
inline void	SetBit( UINT32* pui32Value, UINT32 ui32SlotIdx )		{ *pui32Value |= UINT32(1UL << ui32SlotIdx );			}
inline void	ClearBit( UINT32* pui32Value, UINT32 ui32SlotIdx )		{ *pui32Value &= ~(1UL << ui32SlotIdx );				}

#endif
