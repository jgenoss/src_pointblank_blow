#if !defined( __MULTICASTSYSTEM_H )
#define __MULTICASTSYSTEM_H

#include "Network/ClientP2PDef.h"

#define MULTICASTBUFFERSIZE		2048				//센드 버퍼 사이즈 

enum STAGEINFO_REQ_STATE
{
	STAGEINFO_REQ_STATE_NONE = 0,					//
	STAGEINFO_REQ_STATE_SEND,
	STAGEINFO_REQ_STATE_WAITSEND,
};

struct PACKET_INFO
{
	INT32 iPacketSize;		// Packet structure size
	INT32 iReserve;			// 허용되는 Size
	INT32 iMaxCount;		// network로 보낼수 있는 최대 count
	INT32 iDataMaxCount;	// original data count
}; 

class CCastPacket_Chara
{
public:
	CASTCHECK_USER		m_CastCheck_Chara				;
	N_PCINFO_DEF2		m_CastPacket_Chara_Def			;
	N_PCINFO_CHARA_STATE	m_CastPacket_Chara_State	;
	N_PCINFO_POSROTATION	m_CastPacket_Chara_PosRot	;
	N_PCINFO_ACTIONKEY	m_CastPacket_Chara_ActionKey	;
	N_PCINFO_ONLOADOBJECT	m_CastPacket_Chara_OnLoadObject;
	N_PCINFO_SYNCOBJECT	m_CastPacket_Chara_SyncObject	;
	N_PCINFO_RADIOCHAT	m_CastPacket_Chara_Radio		;
	N_PCINFO_WEAPON		m_CastPacket_Chara_Weapon		;
	N_PCINFO_LIFE		m_CastPacket_Chara_Life		;
	N_PCINFO_BULLET		m_CastPacket_Chara_Bullet		;
	INT32				m_CastPacket_Chara_HitCount	;		// 히트 카운트
	N_PCINFO_HIT		m_CastPacket_Chara_Hit			[MAX_FIREHIT_COUNT];
	N_PCINFO_SUFFER		m_CastPacket_Chara_Suffering	;		// 피격입니다.
	N_PCINFO_DEATH		m_CastPacket_Chara_Death		;
	INT32				m_CastPacket_Chara_DecentCount	;	// 자살 패킷에 대한 카운트
	N_PCINFO_SUICIDEDMG	m_CastPacket_Chara_Decent		[MAX_SUICIDE_COUNT];
	N_PCINFO_MISSION	m_CastPacket_Chara_Mission		;
	N_PCINFO_DROPWEAPON m_CastPacket_Chara_DropW		;
	N_PCINFO_GETWEAPON	m_CastPacket_Chara_GetW		;

	void ResetCastPacket();
};

class CMulticastSystem : public i3::shared_ginst<CMulticastSystem>
{
	
protected:
	REAL32				m_rCheckSendTime;						// 센드 타임 계산용(초당 55회를 맞추는 용도)
	REAL32				m_rCheckSendPingTime;					// Ping send check	
	INT32				m_nAnimObjReserveSize;					// 애니메이션 오브젝트 센드 카운트에서 여분의 사이즈
	INT32				m_nStaticObjReserveSize;				// 고정 오브젝트 센드 카운트에서 여분의 사이즈
	INT32				m_nMoveObjReserveSize;					// 이동 오브젝트 센드 카운트에서 여분의 사이즈
	INT32				m_nDynamicObjReserveSize;				// 물리 오브젝트 센드 카운트에서 여분의 사이즈
	INT32				m_nDropedWeaponReserveSize;				// 드랍된 무기 센드 카운트에서 여분의 사이즈	
	UINT16				m_nSendPos;								// 패킷의 센드 사이즈
	char 				m_SendBuffer[ MULTICASTBUFFERSIZE ];	// 패킷 버퍼

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 게임 캐릭터 정보 버퍼
	
	UINT32				m_nPreSend_CharaIdx;									// 이전에 보낸 인덱스

	/*CCastPacket_Chara m_CastPacket_Chara[SLOT_MAX_COUNT];
	CCastPacket_Chara m_CastPacket_CharaForAI[MAX_COUNT_DOMI_DINO];*/

	CASTCHECK_USER		m_pCastCheck_Chara				[SLOT_MAX_COUNT];
	N_PCINFO_DEF2		m_pCastPacket_Chara_Def			[SLOT_MAX_COUNT];
	N_PCINFO_CHARA_STATE	m_pCastPacket_Chara_State	[SLOT_MAX_COUNT];
	N_PCINFO_POSROTATION	m_pCastPacket_Chara_PosRot	[SLOT_MAX_COUNT];
	UINT8				m_pCastPacket_Chara_ActionKeyCount[SLOT_MAX_COUNT];
	N_PCINFO_ACTIONKEY	m_pCastPacket_Chara_ActionKey	[SLOT_MAX_COUNT][MAX_ACTION_COUNT];
	N_PCINFO_ONLOADOBJECT	m_pCastPacket_Chara_OnLoadObject[SLOT_MAX_COUNT];
	N_PCINFO_SYNCOBJECT	m_pCastPacket_Chara_SyncObject	[SLOT_MAX_COUNT];
	N_PCINFO_RADIOCHAT	m_pCastPacket_Chara_Radio		[SLOT_MAX_COUNT];
	N_PCINFO_WEAPON		m_pCastPacket_Chara_Weapon		[SLOT_MAX_COUNT];
	N_PCINFO_LIFE		m_pCastPacket_Chara_Life		[SLOT_MAX_COUNT];
	N_PCINFO_BULLET		m_pCastPacket_Chara_Bullet		[SLOT_MAX_COUNT];
	INT32				m_pCastPacket_Chara_HitCount	[SLOT_MAX_COUNT];		// 히트 카운트
	N_PCINFO_HIT		m_pCastPacket_Chara_Hit			[SLOT_MAX_COUNT][MAX_FIREHIT_COUNT];
	N_PCINFO_SUFFER		m_pCastPacket_Chara_Suffering	[SLOT_MAX_COUNT];		// 피격입니다.
	N_PCINFO_DEATH		m_pCastPacket_Chara_Death		[SLOT_MAX_COUNT];
	INT32				m_pCastPacket_Chara_DecentCount	[SLOT_MAX_COUNT];	// 자살 패킷에 대한 카운트
	N_PCINFO_SUICIDEDMG	m_pCastPacket_Chara_Decent		[SLOT_MAX_COUNT][MAX_SUICIDE_COUNT];
	N_PCINFO_MISSION	m_pCastPacket_Chara_Mission		[SLOT_MAX_COUNT];
	N_PCINFO_DROPWEAPON m_pCastPacket_Chara_DropW		[SLOT_MAX_COUNT];
	N_PCINFO_GETWEAPON	m_pCastPacket_Chara_GetW		[SLOT_MAX_COUNT];

	//패킷 사이즈 
	PACKET_INFO			m_iPacketInfo[P2P_SUB_HEAD_COUNT]; 	
	INT32				m_pPreSend_Idx[P2P_SUB_HEAD_COUNT];//이전에 보낸 인덱스

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// 게임 오브젝트 정보 버퍼
	// 수류탄	
	bool				m_pbCastCheck_Grenade			[MAX_THROWWEAPON_COUNT];
	N_WEAPONINFO_THROW	m_pCastPacket_Grenade			[MAX_THROWWEAPON_COUNT];

	// Static Object (HP만 변경되는 오브젝트 이동하지 않음)
	struct _OBJINFO_FOR_MULTICAST 
	{
		T_OBJ		_CastCheck;
		char		_buffer[ 64];	// 최대 33바이트
		UINT16		_writePos;

		void	Reset()
		{
			_CastCheck = 0;
			_writePos = 0;
			i3mem::FillZero( _buffer, sizeof(_buffer));
		}

		UINT16		CopyToPacket( char * pDest)
		{
			if( _writePos > 0)
				i3mem::Copy( pDest, _buffer, _writePos);
			return _writePos;
		}
	};
	_OBJINFO_FOR_MULTICAST	m_pCastPacket_Obj				[OBJ_TOTAL_COUNT];

	// Droped weapon	
	bool				m_pbCastCheck_DropedWeapon		[MAX_DROPEDWEAPON_COUNT];
	N_DROPEDWEAPONINFO	m_pCastPacket_DropedWeapon		[MAX_DROPEDWEAPON_COUNT];
	
	//NPC packet	
	bool				m_pbCastCheck_NPC				[MAX_NPC_COUNT];
	N_NPCINFO_DEF		m_pCastPacket_NPC				[MAX_NPC_COUNT];

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//난입한 유저에대한 처리 버퍼 
	// Client의 Stage info를 요청하는 경우에 필요한 처리	
	INT32				m_nStageInfoSendCount;
	STAGEINFO_REQ_STATE	m_nStageInfoReq;

	// 스테이지 정보 센드에서 마지막으로 보낸 오브젝트의 인덱스 
	INT32				m_nStageInfo_CharaPreSend;
	INT32				m_nStageInfo_MissionPreSend;

protected:
	// 게임 데이터를 버퍼에 저장합니다.
	void	_WriteGameInfo_Chara(			INT32 idx, GAMEINFO_CHARACTER		* pCharaInfo );
	void	_WriteGameInfo_CharaForAI(		INT32 idx, GAMEINFO_CHARACTER		* pCharaInfo );

	void	_WriteGameInfo_Grenade(			INT32 idx, GAMEINFO_THROW_WEAPON	* pGrenadeInfo);
	void	_WriteGameInfo_Object(			INT32 idx, GAMEINFO_OBJECT			* pObjectInfo);
	void	_WriteGameInfo_DropedWewapon(	INT32 idx, GAMEINFO_DROPED_WEAPON	* pDropedWeaponInfo);
	void	_WriteGameInfo_NPC(				INT32 idx, GAMEINFO_NPC				* pNPCInfo);

	//일반 패킷 데이터 만들기 
	void	_MergeSendData_Chara( void);
	bool	_MakePacket_Chara( UINT16 idx);
	UINT16	_MakePacket(INT32 iType, INT32 iIdx, char * pBuffer, INT32 iWritePos); 
	void	_MergeSendData( P2P_SUB_HEADTYPE type );
	
	//난입한 유저 패킷 만들기
	void	_ProcessMakeIntruderPacket( void);
	
	bool	_MakeStageInfo( void);
	bool	_MakeStageInfo_Mission( void);

	void	_ResetStageInfoBuff( void);

	void	_SendToUser( UINT8 idx );					
	void	_SendToBattle( void );

public:
	CMulticastSystem();
	virtual ~CMulticastSystem();

	void	Create( void );								//
	void	Destroy( void );							//
	void	ResetBuffer( INT32 idx);					// 유저가 나가면 리셋합니다.	
	void	Broadcast( REAL32 rDeltaTime);	//외부에서 호출됩니다. 	
	void	RequestIntrudeProcess( void);			//User에게 스테이지 정보를 보내도록 합니다.	
};

#endif	// __MULTICASTSYSTEM_H
