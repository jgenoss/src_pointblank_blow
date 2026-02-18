#pragma once

#include "../../../Common/CommonSource/GameP2PDef.h"

struct GAMEINFO_CHARACTER
{
	bool				_bRecvFlag = false;					// 받았다면 리시브 체크
	bool				_bSendFlag = false;					// 센드 체크 (true이면 보냅니다)
	bool				_bPosSendCheck = false;				// 이동량이 변했다면 체크합니다. 변하지 않았다면 최소 싱크타임만큼만 보냅니다.
	bool				_bReqStageInfo = false;				// only host (난입하면 보내주는 스테이지 정보 체크용)
	bool				_bPosStopSendCheck = false;			// 정지시에 사용합니다.(only local)

	UINT8				_iPing = 0;						// 1이면 핑 패킷으로 처리
	REAL32				_rPingSendTime = 0.0f;

	REAL32				_rCharaStateSendTime = 0.0f;
	REAL32				_rWeaponSyncSendTime = 0.0f;
	REAL32				_rCharaPosSendTime = 0.0f;
	REAL32				_rMissionSyncSendTime = 0.0f;
	REAL32				_rLifePacketSendTime = 0.0f;		// for host
	REAL32				_rObjectSyncSendTime = 0.0f;		// for host

	REAL32				_rDomiObjectSendTime = 0.0f;
	UINT8				_iDomiObjectSendCnt = 0;
	UINT32				_iDomiObjectActivateFlag = 0;

	REAL32				_rSendTime = 0.0f;

	// 네트워크 처리 버퍼
	GAMEINFO_CHARA_NETDATA	_tNetData;
	INT32				_MaxThrowingIndex = 0;

	void	Reset()
	{
		_bRecvFlag				= false;
		_bSendFlag				= false;
		_bPosSendCheck			= false;
		_bReqStageInfo			= false;
		_bPosStopSendCheck		= false;
		_iPing					= 0;
		_tNetData.Reset();
		_rCharaStateSendTime	= 0.f;
		_rWeaponSyncSendTime	= 0.f;
		_rCharaPosSendTime		= 0.f;
		_rMissionSyncSendTime	= 0.f;
		_rLifePacketSendTime	= 0.f;
		_rObjectSyncSendTime	= 0.f;
		_rSendTime				= 0.f;
		_MaxThrowingIndex		= -1;
		_rDomiObjectSendTime = 0.0f;
		_iDomiObjectSendCnt = 0;
	}

	void	ProcessHostLost()
	{
		_bRecvFlag				= false;
		_bSendFlag				= false;
		_bReqStageInfo			= false;
		_tNetData._iInfoFlag	= 0;
	}

	void	ResetSendCount()
	{	
		_bSendFlag				= false;
		_tNetData._iInfoFlag	= 0;
		_tNetData._tCharaState.setFireAction(0);
		_tNetData._tActionKey.Reset();
		_tNetData._tRadioChat.setRadio(0);
		_tNetData._tFireData._tHitData.setHitCount( 0);
		_iPing											= 0;
		_tNetData._tSuicideDamageData.setCount( 0);
		_tNetData._tDropWeaponData.setSuccess(0);
		_tNetData._tGetWeaponData.setSuccess(0);

		_rSendTime										= 0.f;
	}

	void	ResetSendCountForHost()
	{
		_tNetData._tSufferData.setDeath( false);
		_tNetData._tSufferData.setDeathType( CHARA_DEATH_UN);
		_tNetData._tGetWeaponData.setSuccess(0);
		_tNetData._tDropWeaponData.setSuccess(0);
	}
} ;

class GAMEINFO_AI_DINOs_NETDATA : public GameNetworkInfoBase
{
public:
	INT32 i32Mask;
	GAMEINFO_AI_DINO		m_AI_Dino_Infos[ MAX_COUNT_DOMI_DINO];
	i3::vector<GAMEINFO_AI_DINO*> list;

	GAMEINFO_AI_DINOs_NETDATA() { Reset(); }

	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest)  override { I3ASSERT_0;};

	// 서버에서 구현이 필요하다면 내용 구성해야 합니다.
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;
};

struct GAMEINFO_DOMI_EVENT : i3::class_common_pool<GAMEINFO_DOMI_EVENT>
{
	INT8 evtType;
};
struct GAMEINFO_SENTRY_DUMMY_REPAIR : GAMEINFO_DOMI_EVENT
{
	INT8 idx;
};
struct GAMEINFO_SENTRYGUN_FIRE : GAMEINFO_DOMI_EVENT
{
	VEC3D	target;
	INT8	idx;
};
struct GAMEINFO_SUPPLY_USE : GAMEINFO_DOMI_EVENT
{
	INT8 customerIdx;
	INT8 idx;
};

struct GAMEINFO_DROPITEM_DROPPED : GAMEINFO_DOMI_EVENT
{
	VEC3D pos;
	INT8 type;
	INT8 idx;
};
struct GAMEINFO_DROPITEM_CATCHED : GAMEINFO_DOMI_EVENT
{
	INT8 idx;
	INT8 userIdx;
};
struct GAMEINFO_SP_SCORE : GAMEINFO_DOMI_EVENT
{
	INT32 totalScore;
	UINT16 totalSP;
	INT16 totalKill;
	INT8  userIdx;
};
struct GAMEINFO_DINO_DEATH : GAMEINFO_DOMI_EVENT
{
	INT16 sp[4];
	INT16 score[4];
	INT8  killerIdx;
	INT8  combo;
	bool  bHeadShot;
};
struct GAMEINFO_MUTANTTREX_DASH : GAMEINFO_DOMI_EVENT
{
	INT8 dash_info;
};
struct GAMEINFO_MUTANTTREX_HOWL : GAMEINFO_DOMI_EVENT
{
	INT8 mutantIdx;
	INT16 threshold; // 하울링 취소 임계치(HP)
};

struct GAMEINFO_SPECIAL_EFFECT : GAMEINFO_DOMI_EVENT
{
	PACKET_DOMINATION_DIVULGE_SPECIAL_EFFECT_ACK data;
};

struct GAMEINFO_SKILL_OBJ
{
	INT16	hp;
	INT8	idx;
	INT8	installerIdx;
	INT8	lv;
};

struct GAMEINFO_DOMI_DROPITEM
{
	VEC3D	pos;
	INT8	type;
	bool	dropped;
	INT8	idx;
};

class GAMEINFO_DOMIOBJECT_CONT : public GameNetworkInfoBase
{
protected:
	GAMEINFO_SKILL_OBJ				skillObj;
	GAMEINFO_DOMI_DROPITEM			dropItem;

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest)  override {}
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize )  override { I3ASSERT_0; return 0;}
	virtual UINT16	Unpack( const char * pInPacket) override;

	const GAMEINFO_SKILL_OBJ * getSkillObjectInfo( void)			{ return &skillObj; }
	const GAMEINFO_DOMI_DROPITEM * getDropItemInfo( void)			{ return &dropItem; }
};

class GAMEINFO_DOMIEVENT_CONT : public GameNetworkInfoBase
{
public:

	i3::vector<GAMEINFO_DOMI_EVENT*> events;

	GAMEINFO_DOMIEVENT_CONT() {}
	~GAMEINFO_DOMIEVENT_CONT()
	{
		Clear();
	}
	void Clear()
	{
		for (size_t i=0; i<events.size(); i++)
		{
			I3_SAFE_DELETE( events[i]);
		}
		events.clear();
	}

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override {}
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override { I3ASSERT_0; return 0;}
	virtual UINT16	Unpack( const char * pInPacket) override;
};
//#endif

class GAMEINFO_SENTRYGUN_DEGREE : public GameNetworkInfoBase
{
public:
	INT8 idx;
	REAL32 theta;

	virtual void	Reset() override { idx = -1; theta = 0.f; }
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override {}
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override { I3ASSERT_0; return 0; }
	virtual UINT16	Unpack( const char * pInPacket) override;
};

//잠시 올려서 확인합니다. 성백
struct BROADCASTINFO
{
	REAL32						_rDeltatime;
	GAMEINFO_CHARACTER *		_pCharaInfoList;
	GAMEINFO_CHARACTER *		_pCharaInfoForAI_List;
	GAMEINFO_THROW_WEAPON *		_pGrenadeInfoList;
	GAMEINFO_OBJECT *			_pObjectInfoList;
	GAMEINFO_DROPED_WEAPON *	_pDropedWeaponInfoList;
    GAMEINFO_NPC *				_pNPCInfoList;
	INT32						_iTotalObjectCount;			// 월드의 전체 오브젝트 수
	UINT8						_iRoundNum;					// 현재 라운드 수 2009.06.09 추가 dfly79
};

///////////////////////////////////////////////////////////////////////////////////////////
// 싱크 보정용 데이터입니다. ( Recv에만 사용 )
struct NET_ASSIST_INFO
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
	WEAPON_SLOT_TYPE		_nThrowSlotIdx;		// Send에만 사용됩니다. Slot Index(CN)
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
		_nThrowSlotIdx		= WEAPON_SLOT_PRIMARY;
		i3Vector::Zero( &_vPostPosition );
		i3Vector::Zero( &_vRecvPosition );
		i3Vector::Zero( &_vPostDir );
		i3Vector::Zero( &_vRecvDir );
		i3Vector::Zero( &_vMissionLoadPos );
		_iSendGrenadeCount = 0;
	}

} ;

struct NET_ASSIST_SEND	// Send시 버퍼링이 필요한 경우 사용
{
	UINT8	_SendCountForHitByObj;		// 보낼 횟수
	i3::vector<CHARA_DAMAGEINFO*>		_CharaDamageInfoList;
};