#if !defined( __GAMEP2PDEF_H__ )
#define __GAMEP2PDEF_H__

#include "../../CommonSource/NetworkP2PDef.h"

#include "GameCharaDef.h"

#define MAX_FIREHIT_COUNT			16				// 패킷으로 최대 히트 카운트 (이값이 넘으면 패킷 처리X)

#define MAX_THROWWEAPON_SLOP		8				// SLOT_MAX_COUNT의 배수로 카운트와 동일해야합니다.
#define MAX_THROWWEAPON_COUNT		(MAX_THROWWEAPON_SLOP * SLOT_MAX_COUNT)				//  처리가능한 수류탄의 갯수( include GrenadeShell, Rocket )
#define MAX_DROPEDWEAPON_COUNT		32				// Droped weapon count
#define OBJ_STATIC_COUNT			220				//  처리가능한 Static Obj
#define OBJ_MOVE_COUNT				4				//  moving object
#define OBJ_DYNAMIC_COUNT			8				//  Dynamic Object	사용하는 Object가 없습니다. 
#define OBJ_ANIM_COUNT				6				// animation object
#define OBJ_CONTROLED_COUNT			16				// controled Object
#define OBJ_RESERVED_COUNT			2
#define OBJ_TOTAL_COUNT				(OBJ_STATIC_COUNT+OBJ_DYNAMIC_COUNT+OBJ_ANIM_COUNT+OBJ_RESERVED_COUNT)	//


enum GRENADE_THROW_STATE
{
	GRENADE_THROW_NONE = 0,
	GRENADE_THROW_SEND,
	GRENADE_THROW_RECV,
	GRENADE_THROW_DONE,
};


/////////////////////////////////////////////////////////////////////////////////////
// 실제 패킷은 아닙니다. Game과 통신용입니다.
//
// 최종적으로 계산할 게임 데이터입니다. 이를 바탕으로 네트워크 캐릭터를 세팅
typedef struct _tagNetwork_GameInfo_Chara_Weapon
{
	bool				_bWeaponExtension;		// 무기 확장기능 상태 (1 이면 활성화 상태)
	WEAPON_USAGE_TYPE	_nWeaponSlot;			// 무기 슬롯 Index
	WEAPON_CLASS_TYPE	_nWeaponClassType;		// 무기 클래스 타입
	INT32				_nWeaponNumber;			// 무기 번호
	WEAPON_FIRE_ORDER	_nFireOrder;	

	void	Reset()
	{
		_bWeaponExtension	= false;
		_nWeaponSlot		= WEAPON_USAGE_PRIMARY;
		_nWeaponClassType	= WEAPON_CLASS_UNKNOWN;
		_nWeaponNumber		= 0;

		_nFireOrder			= WEAPON_PRIMARY_FIRE;
	}

	void	CopyTo( _tagNetwork_GameInfo_Chara_Weapon * pDest)
	{
		pDest->_bWeaponExtension	= _bWeaponExtension;
		pDest->_nWeaponSlot			= _nWeaponSlot;
		pDest->_nWeaponClassType	= _nWeaponClassType;
		pDest->_nWeaponNumber		= _nWeaponNumber;
		pDest->_nFireOrder			= _nFireOrder;
	}

} GAMEINFO_CHARA_WEAPON;

typedef struct _tagNetwork_GameInfo_Chara_PosRot
{
	REAL32	_fTheta;				//
	REAL32	_fRho;					//
	VEC3D   _vPos;					// Chara 위치

	void	Reset()
	{
		_fTheta			= 0.f;
		_fRho			= 0.f;
		i3Vector::Set( &_vPos, 0.f, 5000.f, 0.f );
	}

	void	CopyTo( _tagNetwork_GameInfo_Chara_PosRot * pDest)
	{
		pDest->_fTheta				= _fTheta;
		pDest->_fRho				= _fRho;
		i3Vector::Copy( &pDest->_vPos, &_vPos );
	}

} GAMEINFO_CHARA_POSROT;

typedef struct _tagNetwork_GameInfo_Normal
{
private:
	UINT16	_nCharaHP;				// HP

public:
	UINT16	_Body;					// 캐릭터 액션 상태 (앉기 여부)
	UINT16	_Body2;					// 캐릭터 액션 상태 (줌 상태 여부)
	UINT16	_Upper;					// 캐릭터 액션 상태 (상체)
	UINT16	_Lower;					// 캐릭터 액션 상태 (하체)
	UINT16	_nFireAction;			// 발사 동작 여부( 칼, 수류탄에서 사용됩니다)
	UINT16	_nHelmet;				// 헬멧 유무 ( 0:None, 1:Drop, 2:Equip)

	UINT16	_nWeaponInfo[WEAPON_USAGE_COUNT];			// Weapon Class (1byte), Weapon Number(1byte)	// only Intruder
	
	UINT8	_nMoveKey;				// 입력된 이동 키
	UINT8	_nActionKey;			// 입력된 행동 키
	UINT8	_nUseActionObjectIdx;	// 사용할 오브젝트 인덱스
	UINT8	_nRadio;				// Radio chat
	UINT8	_nSectorIdx;			// World Sector Index

	UINT8	_nSyncObjectState;		// 오브젝트 상태에 따른 캐릭터 싱크 처리
	UINT8	_iSyncObjectIdx;		// 오브젝트 상태에 따른 캐릭터 싱크 처리용 오브젝트 인덱스

	bool	_bOnLoadObject;			// Object Onload check
	UINT8	_iOnLoadObjectIndex;		//  & index

	void	Reset()
	{
		_nFireAction = _Body = _Body2 = _Upper = _Lower = 0;
		_nHelmet = 0;
		for( INT32 i = 0; i < WEAPON_USAGE_COUNT; i++ )
			_nWeaponInfo[i]	= 0;
		
		_nMoveKey		= 0;
		_nActionKey		= 0;
		_nUseActionObjectIdx = 0xFF;
		_nCharaHP		= 100;
		_nRadio			= 0;
		_nSectorIdx		= 0;

		_nSyncObjectState		= 0;		// 오브젝트 상태에 따른 캐릭터 싱크 처리
		_iSyncObjectIdx			= 0;		// 오브젝트 상태에 따른 캐릭터 싱크 처리용 오브젝트 인덱스

		_bOnLoadObject			= false;
		_iOnLoadObjectIndex		= 0;
		
	}

	void	CopyTo( _tagNetwork_GameInfo_Normal * pDest )
	{	
		pDest->_Body				= _Body;
		pDest->_Body2				= _Body2;
		pDest->_Upper				= _Upper;
		pDest->_Lower				= _Lower;
		pDest->_nFireAction			= _nFireAction;
		pDest->_nHelmet				= _nHelmet;
		for( INT32 i = 0; i < WEAPON_USAGE_COUNT; i++ )
			pDest->_nWeaponInfo[i]		= _nWeaponInfo[i];
		
		pDest->_nMoveKey			= _nMoveKey;
		pDest->_nActionKey			= _nActionKey;
		pDest->_nUseActionObjectIdx = _nUseActionObjectIdx;
		pDest->_nCharaHP			= _nCharaHP;
		pDest->_nRadio				= _nRadio;
		pDest->_nSectorIdx			= _nSectorIdx;
		pDest->_nSyncObjectState	= _nSyncObjectState;		// 오브젝트 상태에 따른 캐릭터 싱크 처리
		pDest->_iSyncObjectIdx		= _iSyncObjectIdx;		// 오브젝트 상태에 따른 캐릭터 싱크 처리용 오브젝트 인덱스
		pDest->_bOnLoadObject		= _bOnLoadObject;
		pDest->_iOnLoadObjectIndex	= _iOnLoadObjectIndex;
	}

	// Host, Client 에 상관없이 게임 업데이트에 적용하는 변수들만 복사
	void	CopyToRecv( _tagNetwork_GameInfo_Normal * pDest )
	{	
		pDest->_Body			= _Body;
		pDest->_Body2			= _Body2;
		pDest->_Upper			= _Upper;
		pDest->_Lower			= _Lower;
		pDest->_nFireAction		= _nFireAction;
		
		pDest->_nMoveKey		= _nMoveKey;
		pDest->_nRadio			= _nRadio;
		pDest->_nSectorIdx		= _nSectorIdx;
		pDest->_nActionKey		= _nActionKey;
		pDest->_nUseActionObjectIdx = _nUseActionObjectIdx;

		pDest->_nSyncObjectState	= _nSyncObjectState;
		pDest->_iSyncObjectIdx	= _iSyncObjectIdx;

		pDest->_bOnLoadObject		= _bOnLoadObject;
		pDest->_iOnLoadObjectIndex	= _iOnLoadObjectIndex;
	}

	UINT16	getHP( void);
	void	setHP( UINT16 hp);

} GAMEINFO_NORMAL;

// 히트 데이터
typedef struct _tagNetwork_GameInfo_Hit
{
private:
	INT32	_FireHitCount;
	UINT8	_FireHitWho[ MAX_FIREHIT_COUNT ];		// Hit된 사람 ( max 16개입니다. 샷건은 복수개의 값이 들어옵니다. )
	UINT8	_FireHitPart[ MAX_FIREHIT_COUNT ];		// Hit된 사람의 Hitpart
	UINT8	_FireHitDamage[ MAX_FIREHIT_COUNT ];	// Damage

public:

	void	Reset( void)
	{
		_FireHitCount	= 0;
		for( INT32 i = 0; i < MAX_FIREHIT_COUNT; i++ )
		{
			_FireHitWho[ i ]	= 0;
			_FireHitPart[ i ]	= 0;
			_FireHitDamage[ i ]	= 0;
		}
	}

	void	CopyTo( _tagNetwork_GameInfo_Hit * pDest)
	{
		pDest->_FireHitCount	= _FireHitCount;
		for( INT32 i = 0; i < _FireHitCount; i++ )
		{
			pDest->_FireHitWho[ i ]		= _FireHitWho[ i ];
			pDest->_FireHitPart[ i ]	= _FireHitPart[ i ];
			pDest->_FireHitDamage[ i ]	= _FireHitDamage[ i ];
		}
	}

	INT32	getHitCount( void)			{ return _FireHitCount; }
	void	setHitCount( INT32 count)	{ _FireHitCount = count; }

	UINT8	getHitWho( INT32 idx)				{ I3_BOUNDCHK( idx, MAX_FIREHIT_COUNT); return _FireHitWho[ idx]; }
	void	setHitWho( INT32 idx, UINT8 who)	{ I3_BOUNDCHK( idx, MAX_FIREHIT_COUNT); _FireHitWho[ idx] = who; }

	UINT8	getHitPart( INT32 idx)				{ I3_BOUNDCHK( idx, MAX_FIREHIT_COUNT); return _FireHitPart[ idx]; }
	void	setHitPart( INT32 idx, UINT8 part)	{ I3_BOUNDCHK( idx, MAX_FIREHIT_COUNT); _FireHitPart[ idx] = part; }

	UINT8	getHitDamage( INT32 idx)			{ I3_BOUNDCHK( idx, MAX_FIREHIT_COUNT); return _FireHitDamage[ idx]; }
	void	setHitDamage( INT32 idx, UINT8 dm)	{ I3_BOUNDCHK( idx, MAX_FIREHIT_COUNT); _FireHitDamage[ idx] = dm; }

} GAMEINFO_HIT;

// 발사 패킷에 대한 게임 데이터입니다.
typedef struct _tagNetwork_GameInfo_Fire
{
	UINT16	_nId;					// 발사 패킷 ID
	INT32	_nRand;					// Random code값입니다.
	VEC3D	_vStartPos;				// 발사시의 포지션
	VEC3D	_vTargetPos;			// 발사시의 목표점

	UINT16	_nNetSlotIdx;			// 수류탄 계열의 슬롯 번호

#if defined( CHECK_LOADEDBULLET)
	UINT8	_LoadedBullet;			// 탄약 수
	UINT8	_LoadedDualBullet;		// 듀얼 탄창용
#endif

	// 해당 Chara가 쏜 총알에 대한 Hit
	GAMEINFO_HIT _tHitData;

	void	Reset()
	{
		_nId			= MAX_THROWWEAPON_COUNT;
		_nRand			= 0;
		i3Vector::Zero( &_vStartPos );
		i3Vector::Zero( &_vTargetPos );
		_nNetSlotIdx	= 0;
		_tHitData.Reset();

#if defined( CHECK_LOADEDBULLET)
		_LoadedBullet	= 0;
		_LoadedDualBullet = 0;
#endif
	}

	void	CopyToFire( _tagNetwork_GameInfo_Fire * pDest)
	{
		pDest->_nId			= _nId;
		pDest->_nRand		= _nRand;
		i3Vector::Copy( &pDest->_vStartPos, &_vStartPos );
		i3Vector::Copy( &pDest->_vTargetPos, &_vTargetPos );
		pDest->_nNetSlotIdx	= _nNetSlotIdx;

#if defined( CHECK_LOADEDBULLET)
		pDest->_LoadedBullet	= _LoadedBullet;
		pDest->_LoadedDualBullet = _LoadedDualBullet;
#endif
	}

	void	CopyTo( _tagNetwork_GameInfo_Fire * pDest )
	{
		CopyToFire( pDest);
		_tHitData.CopyTo( &pDest->_tHitData);
	}

	bool	isValidThrow( INT32 userIdx)
	{
		if( _nNetSlotIdx >= MAX_THROWWEAPON_COUNT )	return false;
		if( MAX_THROWWEAPON_SLOP * userIdx > _nNetSlotIdx )	return false;
		if( MAX_THROWWEAPON_SLOP * (userIdx + 1) <= _nNetSlotIdx ) return false;

		return true;
	}
} GAMEINFO_FIRE;

//
// 맞은 부위에 대한 데이터입니다. Recv에만 사용
typedef struct _tagNetwork_GameInfo_Suffer
{
	UINT8	_nShooter;				// 쏜 사람
	UINT8	_nHitpart;				// 맞은 부위
	UINT8	_nDeathType;			// 맞은 무기에 대해서 죽는 타입
	VEC3D	_vHitDir;				// 맞는 방향

	BOOL	_bDeath;

	void	Reset()
	{
		_nShooter	= 0;
		_nHitpart	= 0;
		_nDeathType	= 0;
		i3Vector::Zero( &_vHitDir );
		_bDeath		= FALSE;
	}

	void	CopyTo( _tagNetwork_GameInfo_Suffer * pDest )
	{
		pDest->_nShooter	= _nShooter;
		pDest->_nHitpart	= _nHitpart;
		pDest->_nDeathType	= _nDeathType;
		i3Vector::Copy( &pDest->_vHitDir, &_vHitDir );
		pDest->_bDeath		= _bDeath;
	}
} GAMEINFO_SUFFER;

//
// 낙하 데미지 데이터
typedef struct _tagNetwork_GameInfo_Suicide
{
	UINT8	_nDamage;
	UINT8	_nObjectIdx;					// 0x00 ~ 0xFE		0xFF는 오브젝트가 아닌경우

	void	Reset()
	{
		_nDamage		= 0;
		_nObjectIdx		= 0xFF;
	}

	void	CopyTo( _tagNetwork_GameInfo_Suicide * pDest )
	{
		pDest->_nDamage		= _nDamage;
		pDest->_nObjectIdx	= _nObjectIdx;
	}
} GAMEINFO_SUICIDEDAMAGE;

//
// Mission Data
typedef struct _tagNetwork_GameInfo_Mission
{
	UINT8		_nState;
	BOMB_AREA	_nArea;

	void	Reset()
	{
		_nState			= 0;
		_nArea			= BOMB_AREA_A;
	}

	void	CopyTo( _tagNetwork_GameInfo_Mission * pDest )
	{
		pDest->_nState			= _nState;
		pDest->_nArea			= _nArea;
	}

} GAMEINFO_MISSION;

// 무기 아이디 : DropList의 아이디입니다( WeaponManager )
typedef struct _tagNetwork_GameInfo_DropWeapon
{
	UINT8	_bIsSuccess;
	UINT8	_nIdx;					// 드랍무기 인덱스 0 ~ 31
	UINT8	_nSlot;
	UINT8	_nClassType;
	UINT8	_nNumber;
	UINT8	_nLoadedBullet;
	UINT8	_nDualLoadedBullet;
	UINT16	_nMaxBullet;

	void	Reset()
	{
		_bIsSuccess = _nIdx = 0;
		_nLoadedBullet = _nDualLoadedBullet = 0;
		_nMaxBullet = 0;
		_nSlot		= 0;
		_nClassType = 0;
		_nNumber	= 0;
	}

	void	CopyTo( _tagNetwork_GameInfo_DropWeapon * pDest )
	{
		pDest->_bIsSuccess	= _bIsSuccess;
		pDest->_nIdx		= _nIdx;
		pDest->_nSlot		= _nSlot;
		pDest->_nClassType	= _nClassType;
		pDest->_nNumber		= _nNumber;
		pDest->_nLoadedBullet	= _nLoadedBullet;
		pDest->_nDualLoadedBullet = _nDualLoadedBullet;
		pDest->_nMaxBullet	= _nMaxBullet;
	}

	void	SetValue( UINT8 isSuccess, UINT8 idx, UINT8 nSlot, UINT8 nClassType, UINT8 nNumber, UINT8 loadBullet, UINT8 dualBullet, UINT16 maxBullet )
	{
		I3ASSERT( isSuccess < 2);
		I3ASSERT( nSlot < 3);
		I3ASSERT( idx < 32);
		
		_bIsSuccess = isSuccess;
		_nIdx = idx;
		_nSlot = nSlot;
		_nClassType = nClassType;
		_nNumber	= nNumber;
		_nLoadedBullet = loadBullet;
		_nDualLoadedBullet = dualBullet;
		_nMaxBullet = maxBullet;
	}
} GAMEINFO_DROPWEAPON;

typedef struct _tagNetwork_GameInfo_DropWeapon	GAMEINFO_GETWEAPON;


//
// 수류탄의 관리용입니다.

typedef struct _tagNetwork_GameInfo_Grenade_NetData
{
	UINT8	_nState;		// 수류탄의 진행 상태 ( 1bit : Settled check)
	UINT8	_nBoundType;	// 바운드 타입
	VEC3D	_vPos;			// 위치( CN은 CS로부터 수류탄의 위치를 받아옵니다 )
	VEC3D	_vDir;			// KEYWORD : WEAPON_C5	부착시의 방향

	void	Reset( void)
	{
		_nState			= 0;
		_nBoundType		= 0;
		i3Vector::Zero( &_vPos);
		i3Vector::Zero( &_vDir);
	}

	void	CopyTo( _tagNetwork_GameInfo_Grenade_NetData * pDest)
	{
		pDest->_nState		= _nState;
		pDest->_nBoundType	= _nBoundType;
		i3Vector::Copy( &pDest->_vPos, &_vPos);
		i3Vector::Copy( &pDest->_vDir, &_vDir);
	}
} GAMEINFO_THROW_WEAPON_NETDATA;

typedef struct _tagNetwork_GameInfo_Grenade_Assist
{
	REAL32	_fInterpolateTime;
	VEC3D	_vPostPos;
	VEC3D	_vDir;

	void	Reset( void)
	{
		_fInterpolateTime = 0.f;
		i3Vector::Zero( &_vPostPos);
		i3Vector::Zero( &_vDir);
	}
} GAMEINFO_THROW_WEAPON_ASSIST;

typedef struct _tagNetwork_GameInfo_Grenade
{
	WeaponBase * _pWeaponBase;
	BOOL	_bRecv;
	// Client : 리시브용
	// Host : 센드용
	GAMEINFO_THROW_WEAPON_NETDATA _tNetData;

	// 수류탄 정보의 싱크 보정용
	GAMEINFO_THROW_WEAPON_ASSIST _tAssistData;

	// for Send
	BOOL	_bBroadcast;			// CS가 Broadcast할지 체크합니다 ( 1이면 샌드 )
	REAL32	_fBroadcastTime;		// sync 타임 계산용

	// 이 시간이 되면 보내도록 세팅합니다.(던진 수류탄이 바인드될때 세팅됨)
	REAL32	_fBroadcastCheckSleepTime;	// Sleep 된 수류탄의 싱크 타임
	REAL32	_fBroadcastCheckSyncTime;	// 날라가는 수류탄의 싱크 타임

	void	Reset()
	{
		_pWeaponBase	= NULL;
		_bRecv			= FALSE;
		_bBroadcast		= FALSE;
		_fBroadcastTime = 0.f;
		_tNetData.Reset();
		_tAssistData.Reset();
	}
} GAMEINFO_THROW_WEAPON;


// 오브젝트
// I3_NETWORK_GAMEOBJECT_TYPE_NONE = 0,
// I3_NETWORK_GAMEOBJECT_TYPE_STATIC,			// 고정되어 있다
// I3_NETWORK_GAMEOBJECT_TYPE_MOVE,			// 이동값만
// I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC,			// 이동 + 회전
// I3_NETWORK_GAMEOBJECT_TYPE_ANIM,			// 애니메이션 키프레임

#define NET_ANIMOBJECT_TYPE_NONE					0
#define NET_ANIMOBJECT_TYPE_OUTPOST					1

typedef struct _tagNetwork_GameInfo_Object_NetData
{
	UINT8	_nState;		// Object State (0x80 using anim2, 0x0F : pathindex)
	UINT16	_nHP;			// HP
	REAL32	_fKey;			// Animation Object Key
	INT32	_nType;			// object net type
	VEC3D	_vPos;			// Position
	QUATERNION	_qRot;		// Rotation
	UINT8	_EnterCharaIdx;
	UINT8	_nPathKey;
	INT32	_nShellCount;	// WeaponBox용 ShellCount

	void	Reset( void)
	{
		_nState		= 0;
		_nHP		= 0;
		_fKey		= 0.f;
		_nType		= 0;
		i3Vector::Zero( &_vPos);
		i3Quat::Identity( &_qRot);
		_EnterCharaIdx	= 0xFF;
		_nPathKey	= 0;
		_nShellCount = 0;
	}

} GAMEINFO_OBJECT_NETDATA;

typedef struct _tagNetwork_GameInfo_Object_Assist
{
	VEC3D	_vPostPos;
	QUATERNION	_qPostRot;
	REAL32	_fAssistTime;

	void	Reset( void)
	{
		i3Vector::Zero( &_vPostPos );
		i3Quat::Identity( &_qPostRot );
		_fAssistTime = 0.f;
	}
} GAMEINFO_OBJECT_ASSIST;

typedef struct _tagNetwork_GameInfo_Object
{
	i3Object *	_pObjectRef;
	I3_NETWORK_GAMEOBJECT_TYPE _nType;
	REAL32		_rRadius;
	UINT8		_nInfoSlotIdx;		// 해당 오브젝트 종류 리스트에서의 인덱스(순번이 됩니다)
	BOOL		_bIsSleep;
	BOOL		_bRecv;
	BOOL		_bIntrudeCheck;

	// Client : 리시브로 받은 데이터 버퍼
	// Host : 센드용으로 사용합니다.
	GAMEINFO_OBJECT_NETDATA _tNetData;

	// 보정용입니다. 리시브시에 세팅됩니다.
	GAMEINFO_OBJECT_ASSIST	_tAssistData;
	
	VEC3D * _vHiterPos;

	// 방장만 해당됩니다.
	BOOL	_bBroadcast;	// (only CS)
	REAL32	_fBroadcastTime;// (only CS)

	void	Reset(void)
	{
		_pObjectRef			= NULL;
		_vHiterPos			= NULL;
		_rRadius			= 0.f;
		_nType				= I3_NETWORK_GAMEOBJECT_TYPE_NONE;
		_nInfoSlotIdx		= 0;
		_bBroadcast			= FALSE;
		_fBroadcastTime		= 1.f;
		_bRecv = _bIsSleep	= FALSE;
		_bIntrudeCheck		= FALSE;
		_tNetData.Reset();
		_tAssistData.Reset();
	}

} GAMEINFO_OBJECT;

class WeaponBase;

typedef struct _tagNetwork_GameInfo_DropedWeapon_NetData
{
	UINT8				_nState;				// 드랍된 상태 ( 0x80 : dropcheck)
	VEC3D				_vPos;					// 위치
	QUATERNION			_qRot;					// 회전

	void	Reset( void)
	{
		_nState = 0;
		i3Vector::Zero( &_vPos);
		i3Quat::Identity( &_qRot);
	}
} GAMEINFO_DROPEDWEAPON_NETDATA;

typedef struct _tagNetwork_GameInfo_DropedWeapon_Assist
{	
	REAL32			_fAssistTime;			// 인터폴레이션 타임
	VEC3D			_vPostPos;
	QUATERNION		_qPostRot;
	VEC3D			_vDir;					// 던진 방향

	void	Reset( void)
	{
		_fAssistTime	= 0.f;
		i3Vector::Zero( &_vPostPos);
		i3Quat::Identity( &_qPostRot);
		i3Vector::Zero( &_vDir);
	}
} GAMEINFO_DROPEDWEAPON_ASSIST;

typedef struct _tagNetwork_GameInfo_OBJ_Weapon
{
	WeaponBase *	_pWeaponRef;
	
	BOOL	_bRecv;					// 받은 여부
	REAL32	_fDropedWeaponLifeTime;	// 라이프 타임

	// 네트워크 데이터
	// Client : 리시브용으로 사용
	// Host : 센드용으로 사용
	GAMEINFO_DROPEDWEAPON_NETDATA	_tNetData;

	// 보정용
	GAMEINFO_DROPEDWEAPON_ASSIST	_tAssistData;

	// for CS
	BOOL	_bBroadcast;
	REAL32	_fBroadcastTime;

	void	Reset(void)
	{
		_pWeaponRef				= NULL;
		_bRecv					= FALSE;
		_fDropedWeaponLifeTime	= 0.f;
		_tNetData.Reset();
		_tAssistData.Reset();
		_bBroadcast				= FALSE;
		_fBroadcastTime			= 0.f;
	}

	void	setDroped( bool bDrop)
	{
		if( bDrop )
			_tNetData._nState |= NET_WEAPONDROPED_DROPED;
		else
			_tNetData._nState &= ~NET_WEAPONDROPED_DROPED;
	}
	bool	isDroped( void)
	{
		if( _tNetData._nState & NET_WEAPONDROPED_DROPED )
			return true;
		return false;
	}

	NET_WEAPONDROPED_STATE	getState( void)
	{
		return (NET_WEAPONDROPED_STATE) (_tNetData._nState & 0x0F);
	}
	void	setState( NET_WEAPONDROPED_STATE state )
	{
		_tNetData._nState = (_tNetData._nState & 0xF0) | (UINT8)state;
	}

} GAMEINFO_DROPED_WEAPON;

//////////////////////////////////////////////////////////////////////////////////////////////
//					NPC game info
typedef struct _tagNetwork_GameInfo_NPC
{	
	UINT8	_nState;
	UINT8	_nHP;

	VEC3D	_vTargetPos;
	INT32	_nHitUserPart;

	BOOL	_bBroadcast;

	void	Reset( void)
	{
		_nState			= 0;
		_nHP			= 0;
		i3Vector::Zero( &_vTargetPos);
		_nHitUserPart	= 0;
		_bBroadcast		= FALSE;
	}
} GAMEINFO_NPC;

///////////////////////////////////////////////////////////////////////////////////////////
// 싱크 보정용 데이터입니다. ( Recv에만 사용 )
typedef struct _tagNetwork_GameInfo_Assist
{
	// 인터폴레이션용
	REAL32	_fRecvPosTime;			// 이동에 대한 Recv 타임
									// ( Recv 시점으로부터 _vRecvPosition으로 인터폴레이션하기 위한 타이머, Recv하면 초기화 )
	VEC3D	_vPostPosition;			// Recv시점의 현재 위치( 이 위치를 기준으로 받은 패킷의 포지션으로 인터폴레이션한다. )
	VEC3D	_vRecvPosition;			// Recv한 실제 포지션( 이 위치로 이동하게 됩니다 )
	REAL32	_fLength;				// 현재 위치와 받은 시점의 위치에 대한 거리

	UINT32	_uThrowCount;	// 해킹 방지 count
	
	REAL32	_fRecvThetaRhoTime;		//
	VEC3D	_vPostDir;				// FirePacket에서 Recv시점의 현재 방향 ( Theta, Rho를 구하는데 사용됩니다. )
	VEC3D	_vRecvDir;				// FirePacket에서 Recv된 타겟 포지션 ( Theta, Rho를 구하는데 사용됩니다. )
	 
	REAL32	_fPostTheta, _fRecvTheta;// SyncPacket에서의 Theta
	REAL32	_fPostRho,	_fRecvRho;	//	Rho

	UINT32	_nPrvMoveKey;

	UINT8	_nMissionLoaded;
	VEC3D	_vMissionLoadPos;		// Mission Object install position // only intruder

	// 투척 데이터 only client
	GRENADE_THROW_STATE		_nThrowState;		// Send에만 사용됩니다. 던진 수류탄의 상태(CN)
	WEAPON_USAGE_TYPE		_nThrowSlotIdx;		// Send에만 사용됩니다. Slot Index(CN)
	VEC3D					_vThrowDir;

	INT32	_iSendGrenadeCount;

	void	Reset()
	{
		_fRecvPosTime		= 0.f;
		_fLength			= 0.f;
		_uThrowCount		= 0xFFFFF0FF & i3Math::Rand();
		_fRecvThetaRhoTime	= 0.f;
		_fPostTheta			= 0.f;
		_fRecvTheta			= 0.f;
		_fPostRho			= 0.f;
		_fRecvRho			= 0.f;
		_nPrvMoveKey		= 0;
		_nMissionLoaded		= 0;
		_nThrowState		= GRENADE_THROW_NONE;
		_nThrowSlotIdx		= WEAPON_USAGE_PRIMARY;
		i3Vector::Zero( &_vPostPosition );
		i3Vector::Zero( &_vRecvPosition );
		i3Vector::Zero( &_vPostDir );
		i3Vector::Zero( &_vRecvDir );
		i3Vector::Zero( &_vMissionLoadPos );
		_iSendGrenadeCount = 0;
	}

} GAMEINFO_ASSIST;

////////////////////////////////////////////////////////////////////////////
//
// 캐릭터 정보 버퍼입니다.
typedef struct _tagNetwork_GameInfo_Chara_NetData
{
	UINT16					_iInfoFlag;
	GAMEINFO_NORMAL			_tDefData;					// 기본 데이터
	GAMEINFO_CHARA_WEAPON	_tWeaponData;
	GAMEINFO_CHARA_POSROT	_tPosRotData;
	GAMEINFO_FIRE			_tFireData;					// 발사 데이터(발사시의 히트 데이터 포함)
	GAMEINFO_SUFFER			_tSufferData;				// 캐릭터 자신의 피격 데이터입니다.(수류탄)
	GAMEINFO_SUICIDEDAMAGE	_tSuicideDamageData;		//
	GAMEINFO_MISSION		_tMissionData;
	GAMEINFO_DROPWEAPON 	_tDropWeaponData;
	GAMEINFO_GETWEAPON		_tGetWeaponData;

	void	Reset( void)
	{
		_iInfoFlag			= 0x0000;
		_tDefData.Reset();
		_tWeaponData.Reset();
		_tPosRotData.Reset();
		_tFireData.Reset();	
		_tSufferData.Reset();
		_tSuicideDamageData.Reset();
		_tMissionData.Reset();
		_tDropWeaponData.Reset();
		_tGetWeaponData.Reset();
	}

	void	CopyTo( _tagNetwork_GameInfo_Chara_NetData * pDest)
	{
		pDest->_iInfoFlag	= _iInfoFlag;
		_tDefData.CopyTo( &pDest->_tDefData);
		_tWeaponData.CopyTo( &pDest->_tWeaponData);
		_tPosRotData.CopyTo( &pDest->_tPosRotData);
		_tFireData.CopyTo( &pDest->_tFireData);
		_tSufferData.CopyTo( &pDest->_tSufferData);
		_tSuicideDamageData.CopyTo( &pDest->_tSuicideDamageData);
		_tMissionData.CopyTo( &pDest->_tMissionData);
		_tDropWeaponData.CopyTo( &pDest->_tDropWeaponData);
		_tGetWeaponData.CopyTo( &pDest->_tGetWeaponData);
	}
} GAMEINFO_CHARA_NETDATA;

typedef struct _tagNetwork_GameInfo_Character
{
	BOOL				_bRecvFlag;					// 받았다면 리시브 체크
	BOOL				_bSendFlag;					// 센드 체크 (TRUE이면 보냅니다)
	BOOL				_bPosSendCheck;				// 이동량이 변했다면 체크합니다. 변하지 않았다면 최소 싱크타임만큼만 보냅니다.
	BOOL				_bReqStageInfo;				// only host (난입하면 보내주는 스테이지 정보 체크용)
	BOOL				_bPosStopSendCheck;			// 정지시에 사용합니다.(only local)

	UINT8				_iPing;						// 1이면 핑 패킷으로 처리
	REAL32				_rPingSendTime;

	REAL32				_rCharaStateSendTime;
	REAL32				_rWeaponSyncSendTime;
	REAL32				_rCharaPosSendTime;
	REAL32				_rMissionSyncSendTime;
	REAL32				_rLifePacketSendTime;		// for host
	REAL32				_rObjectSyncSendTime;		// for host

	REAL32				_rSendTime;

	// 네트워크 처리 버퍼
	GAMEINFO_CHARA_NETDATA	_tNetData;

	void	Reset()
	{
		_bRecvFlag				= FALSE;
		_bSendFlag				= FALSE;
		_bPosSendCheck			= FALSE;
		_bReqStageInfo			= FALSE;
		_bPosStopSendCheck		= FALSE;
		_iPing					= 0;
		_tNetData.Reset();
		_rCharaStateSendTime	= 0.f;
		_rWeaponSyncSendTime	= 0.f;
		_rCharaPosSendTime		= 0.f;
		_rMissionSyncSendTime	= 0.f;
		_rLifePacketSendTime	= 0.f;
		_rObjectSyncSendTime	= 0.f;
		_rSendTime				= 0.f;
	}

	void	ProcessHostLost()
	{
		_bRecvFlag				= FALSE;
		_bSendFlag				= FALSE;
		_bReqStageInfo			= FALSE;
		_tNetData._iInfoFlag	= 0x00;
	}

	void	ResetSendCount()
	{
		_bSendFlag										= FALSE;
		_tNetData._iInfoFlag							= 0x00;
		_tNetData._tDefData._nFireAction				= 0;
		_tNetData._tDefData._nActionKey					= 0;
		_tNetData._tDefData._nRadio						= 0;
		_tNetData._tFireData._tHitData.setHitCount( 0);
		_tNetData._tDefData._nUseActionObjectIdx		= 0xFF;		// 보냈다면 초기화합니다. 이코드 상태이면 다른 오브젝트에 대한 액션을 보낼 수 있습니다.
		_iPing											= 0;
		_tNetData._tSuicideDamageData._nDamage			= 0;
		_tNetData._tDropWeaponData._bIsSuccess			= FALSE;
		_tNetData._tGetWeaponData._bIsSuccess			= FALSE;

		_rSendTime										= 0.f;
	}

	void	ResetSendCountForHost()
	{
		_tNetData._tSufferData._bDeath					= FALSE;
		_tNetData._tSufferData._nDeathType				= CHARA_DEATH_UN;
		_tNetData._tGetWeaponData._bIsSuccess			= FALSE;
		_tNetData._tDropWeaponData._bIsSuccess			= FALSE;
	}
} GAMEINFO_CHARACTER;

// for P2P data recv buffer reference
typedef struct _RecvData
{
	INT32		Type;
	INT32		Idx;
	UINT8		iPing;			// bool
	UINT8		iRoundNum;
	REAL32		rPingSendTime;
	void	*	pArg;

	void	Reset() 
	{
		Type		= 0;
		Idx			= -1;
		iPing		= 0;
		iRoundNum	= 0;
		rPingSendTime = 0.f;
		pArg		= NULL;
	}
} RECV_P2P_DATA;

typedef struct __tagGameToContextInfoCharaDeath
{
	INT32				_iKillidx;
	INT32				_iDeathidx;
	WEAPON_USAGE_TYPE	_nWeaponSlot;
	WEAPON_CLASS_TYPE	_nWeaponClassType;
	INT32				_iWeaponNum;
	CHARA_DEATH_TYPE	_nDeathType;
	CHARA_HIT_PART		_nHitPart;
	VEC3D *				_pHitDir;
	INT32				_iByObjectIdx;

	void	Reset( void)
	{
		_iKillidx			= -1;
		_iDeathidx			= -1;
		_nDeathType			= CHARA_DEATH_UN;
		_nHitPart			= CHARA_HIT_UNKNOWN;
		_pHitDir			= NULL;
		_iByObjectIdx		= -1;
	}

} GAMETOCONTEXT_INFO_CHARA_DEATH;


typedef struct __tagGameToContextInfoWeapon_Throw
{
	INT32				_nThrowNum;			// 보내는 폭탄 종류입니다. 네트워크로는 패킷 타입을 결정하게됩니다.

	VEC3D *				_pPos;
	VEC3D *				_pNormal;
	UINT8				_nAddStateFlag;
	INT32				_nBoundType;

	void	Reset( void)
	{
		_nThrowNum		= 0;

		_pPos			= NULL;
		_pNormal		= NULL;
		_nAddStateFlag	= 0;
		_nBoundType		= 0;
	}

} GAMETOCONTEXT_INFO_WEAPON_THROW;

//잠시 올려서 확인합니다. 성백
typedef struct __tagBroadcastInfo
{
	REAL32						_rDeltatime;
	GAMEINFO_CHARACTER *		_pCharaInfoList;
	GAMEINFO_THROW_WEAPON *		_pGrenadeInfoList;
	GAMEINFO_OBJECT *			_pObjectInfoList;
	GAMEINFO_DROPED_WEAPON *	_pDropedWeaponInfoList;
    GAMEINFO_NPC *				_pNPCInfoList;
	INT32						_iTotalObjectCount;			// 월드의 전체 오브젝트 수
	UINT8						_iRoundNum;					// 현재 라운드 수 2009.06.09 추가 dfly79
}BROADCASTINFO;

#endif
