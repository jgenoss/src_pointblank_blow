/**
 * \file	GameUDPHandler.h
 *
 * Declares the game udp handler class.
 */


#if !defined( __GAME_UDP_HANDLER_H__)
#define __GAME_UDP_HANDLER_H__

#include "Network/ClientP2PDef.h"

class UDPPacketReceiver;
class CMulticastSystem;
class UDPPacketSender;

class CGameUDPHandler : public i3::shared_ginst<CGameUDPHandler>
{
private:
	// UDP Packet Receiver
	UDPPacketReceiver *		m_pUDPReceiver;

	// 네트워크assist용
	NET_ASSIST_INFO			m_AssistInfo[ SLOT_MAX_COUNT ];
	NET_ASSIST_SEND			m_SendAssist[ SLOT_MAX_COUNT];

	REAL32					m_fUDPConnectTime	[ USER_SLOT_COUNT ];	// 못 받는 시간동안의 타임( 이 시간이 초과시간을 넘으면 MS에 알려준다 )

	REAL32					m_rElapsedTime;

private:
	const GAMEINFO_CHARACTER *	getGameInfo_Chara( INT32 userIdx, CHARA::BUFFER_TYPE buffer_type = CHARA::BUFFER_USER) const;
	GAMEINFO_CHARACTER *	setGameInfo_Chara( INT32 userIdx, CHARA::BUFFER_TYPE buffer_type = CHARA::BUFFER_USER);
	const GAMEINFO_CHARA_NETDATA *	getNetInfo_Chara( INT32 userIdx, CHARA::BUFFER_TYPE buffer_type = CHARA::BUFFER_USER) const;
	GAMEINFO_CHARA_NETDATA *	setNetInfo_Chara( INT32 userIdx, CHARA::BUFFER_TYPE buffer_type = CHARA::BUFFER_USER);

protected:

	REAL32 	getElapsedTime( void )	const	{return m_rElapsedTime;	}	// UDPHandler에서만 이용되는 변수 및 함수이므로 이곳에 옮김
	void	setElapsedTime( REAL32 fTime )	{m_rElapsedTime = fTime;	}

	void	_P2PMergeRecvData( REAL32 rDeltatime );		// 받은테이터 처리 

	void	_ProcessRecvPacket_Chara_HP( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara );

	// for Client
	void	_ProcessRecvPacket_Chara_ForClient( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara );
	// for Host
	void	_ProcessRecvPacket_Chara_ForHost( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara);

	/* AI 패킷을 처리합니다. */
	void	_ProcessRecvPacket_AI_Respawn( GAMEINFO_AI_DINO* buf);
	void	_ProcessRecvPacket_AI_HP( INT32 idx, GAMEINFO_AI_DINO * pRecvData, GAMEINFO_CHARACTER * pCharaInfo, CGameCharaBase * pChara);
	void	_ProcessRecvPacket_AI( INT32 idx, GAMEINFO_AI_DINO* pRecvData, GAMEINFO_CHARACTER * pCharaInfo, CGameCharaBase * pChara, REAL32 rDeltatime);
	

//#ifdef DOMI_UDP_OBJECT
	void	_ProcessRecvPacket_DomiObject(GAMEINFO_DOMIOBJECT_CONT* pRecvData);
	void	_ProcessRecvPacket_DomiEvent(GAMEINFO_DOMIEVENT_CONT* pRecvData);
	void	_ProcessRecvPacket_SentryGun_TakingAim(GAMEINFO_SENTRYGUN_DEGREE* pRecvData);
//#endif
	/** \brief 수류탄 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Grenade( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime);

	/** \brief Static 오브젝트 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Obj( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime);

	/** \brief 월드에 떨어진 무기 패킷을 처리합니다. */
	void	_ProcessRecvPacket_DropedWeapon( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime);

	/** \brief 난입용 캐릭터 정보를 처리합니다. */
	bool	_ProcessRecvPacket_StageInfo_Chara( INT32 idx, RECV_P2P_DATA * pRecvData);

	///** \brief 난입용 오브젝트 정보를 처리합니다. */
	//void	_ProcessRecvPacket_StageInfo_Obj( i3Object * pObject, INT32 nState, REAL32 rLocalTime);

	/** \brief 난입용 Animation 오브젝트 정보를 처리합니다. */
	void	_ProcessRecvPacket_StageInfo_Obj_Anim( INT32 idx, RECV_P2P_DATA * pRecvData);

	/** \brief 난입용 Static 오브젝트 정보를 처리합니다. */
	void	_ProcessRecvPacket_StageInfo_Obj_Static( INT32 idx, RECV_P2P_DATA * pRecvData);

	/** \brief 난입용 Static 오브젝트 정보를 처리합니다. */
	void	_ProcessRecvPacket_StageInfo_Obj_Move( INT32 idx, RECV_P2P_DATA * pRecvData);

	/** \brief 난입용 Dynamic 오브젝트 정보를 처리합니다. */
	bool	_ProcessRecvPacket_StageInfo_Obj_Dynamic( INT32 idx, RECV_P2P_DATA * pRecvData);

	/** \brief 난입용 Mission 정보를 처리합니다.
		\note C4 정보를 받아 C4를 설치합니다... */
	bool	_ProcessRecvPacket_StageInfo_Mission( RECV_P2P_DATA * pRecvData );

	/** \brief 난입용 Controlled 오브젝트 정보를 처리합니다.
		\note RPG7 박스의 리스폰 타임을 동기화 시킵니다. */
	bool	_ProcessRecvPacket_StageInfo_ControledObject( INT32 idx, RECV_P2P_DATA * pRecvData);
	

	/** \brief 오브젝트 사용 패킷을 처리합니다.
		\note 오브젝트 탑승 유무등에 관한 동기화 패킷입니다. */
	void	_ProcessRecvPacket_Chara_ActionForObjectSync( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief 캐릭터 Action 상태 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_ActionState( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	void	_ProcessRecvPacket_Dino_FullBody(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief 캐릭터 위치 및 방향 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_PosRotation(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara, bool bRotated = false );

	/** \brief 오브젝트 사용 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_ActionKey( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief 오브젝트 탑승 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_OnLoadObject( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief 무기 정보 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_WeaponSync( GAMEINFO_CHARA_WEAPON * pRecvBuf, GAMEINFO_CHARA_STATE * pRecvCharaStateBuf, CGameCharaBase * pChara);

	/** \brief Radio chat 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_RadioChat( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief 미션 정보 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_MissionDataForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara );

	/** \brief 미션 정보 패킷을 처리합니다. (for host) */
	void	_ProcessRecvPacket_Chara_MissionDataForHost( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief 자살 패킷을 처리합니다.
		\note 낙하, 오브젝트에 의한 사망도 처리됩니다. */
	void	_ProcessRecvPacket_Chara_Suicide( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief 무기 줍기 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_GetWeaponForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief 무기 줍기 패킷을 처리합니다. (for host) */
	void	_ProcessRecvPacket_Chara_GetWeaponForHost( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief 무기 버리기 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_DropWeaponForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief 무기 버리기 패킷을 처리합니다. (for host) */
	void	_ProcessRecvPacket_Chara_DropWeaponForHost( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief 다이렉트 무기 PickUp 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_DirectPickUp(GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);

	/** \brief 무기 발사 패킷을 처리합니다. */
	bool	_ProcessRecvPacket_Chara_FireData( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief 무기 발사 패킷을 처리합니다.
		\note 오브젝트 탑승시 */
	void	_ProcessRecvPacket_Chara_FireDataOnObject( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameObjectWithWeapon* pObj);

	/** \brief 캐릭터 히트 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_FireNHitData( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara );
	
	/** \brief 오브젝트 히트 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_FireNHitDataOnObject( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameObjectWithWeapon* pObj);

	/** \brief 캐릭터 사망 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_DeathDataForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara );
	
	/** \brief 캐릭터 피격 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_SufferDataForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara);
	
	/** \brief 수류탄 투척 패킷을 처리합니다. */
	void	_ProcessRecvPacket_Chara_ThrowGrenadeForLocalClient( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf);

	// 핑 처리
	UINT8	_GetPingValue(REAL32 rPingTime); 
	void	_CalcPing_CS( REAL32 rDeltatime ); 
	void	_CalcPing_DediCation( REAL32 rDeltaTime ); 

	// UDP 타임 아웃 처리
	void	_CalcUDPTimeOut( REAL32 rDeltatime );

	/** Game Event를 Net으로 보낼지 확인합니다. */
	void	_CheckGameDataToSend( CGameCharaBase * pChara);
	void	_CheckGameDataToSendForHost(void);
	void	_CheckGameDataToSendForAi(void);

public:
	CGameUDPHandler();
	virtual ~CGameUDPHandler();

	void	UDPConnect( bool bHost);
	void	UDPDisconnect( void);
	void	ChangeHost( bool bHost);

	void	InitRound( void);
	void	InitUserInfo( INT32 userIdx);
	void	StartBattle( INT32 userIdx);
	void	ResetRecvTime( INT32 userIdx);
	void	RespawnChara( INT32 userIdx);
	void	RequestIntrudeProcess( void);


	/** \brief 무기 공격 event를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] pTargetPos 목적지
		\param[in] pWeapon 무기 */
	void	WriteGameEvent_Fire( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon);

#ifdef DOMI_STING_POISON_GRENADE
	void	WriteGameEvent_Fire_DomiDinoGrenade( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon);
#endif

	/** \brief 타격 event를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nHitWho 맞춘 캐릭터 index
		\param[in] nHitPart 맞춘 부위
		\param[in] rDamage 공격량 */
	void	WriteGameEvent_Hit( CGameCharaBase * pChara, const CHARA_DAMAGEINFO * pDamageInfo, bool bByObj);

	/** \brief 무기 버리기 event를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] iWeaponSlot 무기 슬롯 번호
		\param[in] iLoadedBullet 장전 탄환 수
		\param[in] iDulaLoadedBullet Dual 탄창의 장전 탄환 수
		\param[in] iTotalBullet 총 탄환 수*/
	void	WriteGameEvent_DropWeapon( CGameCharaBase * pChara, WEAPON_SLOT_TYPE iWeaponSlot, INT32 iLoadedBullet, INT32 iDualLoadedBullet, INT32 iTotalBullet);

	/** \brief 무기 줍기 event를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] pWeapon 무기 */
	void	WriteGameEvent_PickupWeapon( CGameCharaBase * pChara, WeaponBase * pWeapon);

	/** \brief 무기 줍고 바로 버리는 event를 설정합니다.
	\param[in] pChara 캐릭터 주최
	\param[in] pDropWeapon 버리는 무기
	\param[in] GetItemID 새로 가지는 무기*/
	void	WriteGameEvent_DirectPickupWeapon(CGameCharaBase * pChara, T_ItemID GetItemID);

	/** \brief 미션 정보를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nState 미션 상태
		\param[in] nArea 미션 지역 */
	void	WriteGameEvent_Mission( CGameCharaBase * pChara, NET_MISSION_STATE nState, BOMB_AREA nArea, T_ItemID ItemId 
#ifdef ALTER_DOMI_INSTALL_OBJ
		, DOMI_TYPE ObjType, INT16 ObjIdx
#else
		, INT32 ObjIdx,REAL32 fTime
#endif
		);

	/** \brief 자살 Damge를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nDamage 공격량
		\param[in] iByObjectIdx Object에 의한 경우의 Object Index
		\note object index가 Damage를 주는 경우 Object Index가 설정됩니다. default는 0xFF값
		\param[in] KillerIdx 수류탄에 의한 경우 상대 유저 Index. */
	void	WriteGameEvent_SuicideDamage( CGameCharaBase * pChara, CHARA_DEATH_TYPE type,
		CHARA_HIT_PART part, UINT16 nDamage, VEC3D * pDir,
		WEAPON_CLASS_TYPE WeaponClass, INT32 WeaponNum, UINT16 iByObjectIdx,
		INT32 KillerIdx, WEAPON_SLOT_TYPE WeaponUsage);

	/** \brief RadioChat 정보를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nRadio radio chat 정보값 */
	void	WriteGameEvent_RadioChat( CGameCharaBase * pChara, UINT8 nRadio);

	/** \brief Object 사용에 관한 정보를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nObjIdx Object Index
		\param[in] nActionKey 사용 종류 */
	void	WriteGameEvent_UseObject( CGameCharaBase * pChara, i3Object * pObj, UINT8 nActionKey);

	/** \brief Object 사용에 관한 정보를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] state */
	void	WriteGameEvent_UsurpationObject( CGameCharaBase * pChara, UINT8 nstate);
	

	/** \brief Object Sync용 정보를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nObjIdx Object Index
		\param[in] nSyncActionKey Sync 상태 key
		\note for Only Host */
	void	WriteGameEvent_UseObjectSync( CGameCharaBase * pChara, i3Object * pObj, UINT8 nSyncActionKey);

	/** \brief 캐릭터 피격에 대한 정보를 저장합니다.
		\note for Only Host
		\param[in] pChara 캐릭터 주최
		\param[in] type 죽는 타입
		\param[in] nShooter 죽인 캐릭터 index
		\param[in] nHitPart 맞은 부위		*/
	void	WriteGameEvent_SufferingDamage( CGameCharaBase * pChara, CHARA_DEATH_TYPE type, INT32 nShooter, CHARA_HIT_PART nHitPart, bool bIsNeedDamageMotion);

	/** \brief 월드에 던져진 수류탄 정보를 설정합니다.
		\note BindNetGrenadeInfo 가 호출되기전에 먼저 호출하면 안됩니다.
		\param[in] idx Network index
		\param[in] pThrowWeaponInfo 정보 structrue		*/
	void	WriteGameEvent_ThrowWeaponData( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo);

#ifdef DOMI_STING_POISON_GRENADE
	void	WriteGameEvent_DomiDinoThrowWeaponData( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo );
#endif

//#ifdef DOMI_UDP_OBJECT
	void	WriteGameEvent_DomiSkillObject_Install(INT32 customerIdx, INT8 objectIdx);
	void	WriteGameEvent_DomiSkillObject_Repair(INT32 customerIdx, INT8 objectIdx);
	void	WriteGameEvent_DomiSkillObject_Upgrade(INT32 customerIdx, INT8 objectIdx, INT8 lv);
	void	WriteGameEvent_DomiSkillObject_SupplyUse(INT32 customerIdx, INT8 objectIdx);
	void	WriteGameEvent_DomiDropItem_Catched(INT32 userIdx, INT8 objectIdx);
//#endif

	/** \brief 오브젝트 정보를 저장합니다.
		\note for Only Host
		\param[in] idx network index
		\param[in] nHP HP
		\param[in] pPos 위치
		\param[in] pRot 회전		*/
	void	WriteGameEvent_ObjectData( INT32 idx, INT32 EventSender, UINT16 nHP, VEC3D * pPos, QUATERNION * pRot );


	/** \brief 월드에 떨어진 무기 정보
		\param[in] idx network index
		\param[in] pPos 위치
		\param[in] pMtxRot 회전*/
	void	WriteGameEvent_DropedWeaponData( INT32 idx, VEC3D * pPos, MATRIX * pMtxRot);

	/** \brief animation object 정보를 설정합니다.
		\note 애니메이션 오브젝트는 키프레임만 세팅합니다.
			rBroadcastTime 마다 싱크를 맞추도록 합니다.
		\param[in] idx network index
		\param[in] nHP HP
		\param[in] rKeyFrame key값			*/
	void	WriteGameEvent_AnimObjectData( INT32 idx, INT32 EventSender, UINT16 nHP, REAL32 rKeyFrame);

	/** \brief Outpost 헬기용 object 정보를 설정합니다.
		\note 애니메이션 오브젝트는 키프레임만 세팅합니다.
		\param[in] idx network index
		\param[in] nPathKey Path key값
		\param[in] nHP HP
		\param[in] rKeyFrame key값
		\param[in] bSend 정보 Send 유무
				bSend가 false이면 sync time마다 정보를 보내게 됩니다.*/
	void	WriteGameEvent_AnimObjectDataForOutpost( INT32 idx, INT32 EventSender, UINT8 nPathKey, UINT16 nHP, REAL32 rKeyFrame, bool bSend);
	
	// 던져진 수류탄을 네트워크 버퍼에 등록합니다.
	void	BindNetGrenadeInfo( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos);
	// 던져진 C5를 네트워크 버퍼에 등록합니다.
	void	BindNetC5Info( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos);

	// 터치 다운 사실을 UDP 로 데디에게 전송
	void	WriteGameEvent_TouchDown( CGameCharaBase* pChara, UINT16 idx );


	void	P2PUpdate( REAL32 rDeltatime );			// P2P 업데이트	

};


#endif
