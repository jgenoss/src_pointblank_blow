/**
 * \file	GameEvent.h
 *
 * Declares the game event class.
 */


#if !defined( __GAME_EVENT_H__)
#define __GAME_EVENT_H__

//-------------------------------------------------------------------------------------//
//			Network로 Sync를 맞추는 정보들에 대한 Function입니다.
//			Character
//			Throwing Weapon
//			Dropped Weapon
//			Object

namespace GAMEEVENT
{
	/** \brief 무기 공격 event를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] pTargetPos 목적지
		\param[in] pWeapon 무기 */
	void	Write_Chara_Fire( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon);

#ifdef DOMI_STING_POISON_GRENADE
	void	Write_Chara_Fire_DomiDinoGrenade( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon);
#endif

	/** \brief 타격 event를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nHitWho 맞춘 캐릭터 index
		\param[in] nHitPart 맞춘 부위
		\param[in] rDamage 공격량 */
	void	Write_Chara_Hit( CGameCharaBase * pChara, const CHARA_DAMAGEINFO * pDamageInfo, bool bByObj = false);

	/** \brief 무기 버리기 event를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] iWeaponSlot 무기 슬롯 번호
		\param[in] iLoadedBullet 장전 탄환 수
		\param[in] iDulaLoadedBullet Dual 탄창의 장전 탄환 수
		\param[in] iTotalBullet 총 탄환 수*/
	void	Write_Chara_DropWeapon( CGameCharaBase * pChara, WEAPON_SLOT_TYPE iWeaponSlot, INT32 iLoadedBullet, INT32 iDualLoadedBullet, INT32 iTotalBullet);

	/** \brief 무기 줍기 event를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] pWeapon 무기 */
	void	Write_Chara_PickupWeapon( CGameCharaBase * pChara, WeaponBase * pWeapon);

	/** \brief 무기 줍고 바로 버리는 event를 설정합니다.
	\param[in] pChara 캐릭터 주최
	\param[in] pDropWeapon 버리는 무기 
	\param[in] GetItemID 새로 가지는 무기*/
	void	Write_Chara_DirectPickupWeapon(CGameCharaBase * pChara, T_ItemID GetItemID);

	/** \brief 미션 정보를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nState 미션 상태
		\param[in] nArea 미션 지역 */
	void	Write_Chara_Mission( CGameCharaBase * pChara, NET_MISSION_STATE nState, BOMB_AREA nArea, T_ItemID ItemId 
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
		\note object index가 Damage를 주는 경우 Object Index가 설정됩니다.
			default는 0xFF값*/
	void	Write_Chara_SuicideDamage( CGameCharaBase * pChara, CHARA_DEATH_TYPE type,
		CHARA_HIT_PART part, UINT16 nDamage = 100, VEC3D * pDir = nullptr,
		WEAPON_CLASS_TYPE WeaponClass = WEAPON_CLASS_UNKNOWN, INT32 WeaponNum = -1,
		UINT16 iByObjectIdx = 0xFFFF, INT32 KillerIdx = -1,
		WEAPON_SLOT_TYPE WeaponUsage = WEAPON_SLOT_PRIMARY);

	/** \brief RadioChat 정보를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nRadio radio chat 정보값 */
	void	Write_Chara_RadioChat( CGameCharaBase * pChara, UINT8 nRadio);

	/** \brief Object 사용에 관한 정보를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nObjIdx Object Index
		\param[in] nActionKey 사용 종류 */
	void	Write_Chara_UseObject( CGameCharaBase * pChara, i3Object * pObj, UINT8 nActionKey);

	/** \brief Object Sync용 정보를 설정합니다.
		\param[in] pChara 캐릭터 주최
		\param[in] nObjIdx Object Index
		\param[in] nSyncActionKey Sync 상태 key
		\note for Only Host */
	void	Write_Chara_SyncUseObject( CGameCharaBase * pChara, i3Object * pObj, UINT8 nSyncActionKey);
	
	/** \brief 캐릭터 죽음에 대한 정보를 저장합니다.
		\note for Only Host
		\param[in] pDeathInfo 죽음에 대한 정보 structure */
	void	Write_Chara_Death( GAMETOCONTEXT_INFO_CHARA_DEATH * pDeathInfo);

	/** \brief 캐릭터 피격에 대한 정보를 저장합니다.
		\note for Only Host
		\param[in] pChara 캐릭터 주최
		\param[in] type 죽는 타입
		\param[in] nShooter 죽인 캐릭터 index
		\param[in] nHitPart 맞은 부위		*/
	void	Write_Chara_Suffering( CGameCharaBase * pChara, CHARA_DEATH_TYPE type, INT32 nShooter, CHARA_HIT_PART nHitPart, bool bFlag);

	/** \brief 월드에 던져진 수류탄 정보를 설정합니다.
		\note BindNetGrenadeInfo 가 호출되기전에 먼저 호출하면 안됩니다.
		\param[in] idx Network index
		\param[in] pThrowWeaponInfo 정보 structrue		*/
	void	Write_ThrowingWeapon( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo);

#ifdef DOMI_STING_POISON_GRENADE
	void	Write_DomiDinoThrowWeapon( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo);
#endif

//#ifdef DOMI_UDP_OBJECT
	void	Write_DomiSkillObjectInstall(INT32 customerIdx, INT8 objectIdx);
	void	Write_DomiSkillObjectRepair(INT32 customerIdx, INT8 objectIdx);
	void	Write_DomiSkillObjectUpgrade(INT32 customerIdx, INT8 objectIdx, INT8 lv);
	void	Write_DomiSkillObjectSupplyUse(INT32 customerIdx, INT8 objectIdx);
	void	Write_DomiDropItemCatched(INT32 userIdx, INT8 objectIdx);
//#endif

	/** \brief 오브젝트 정보를 저장합니다.
		\note for Only Host
		\param[in] idx network index
		\param[in] nHP HP
		\param[in] pPos 위치
		\param[in] pRot 회전		*/
	void	Write_Object( INT32 idx, INT32 EventSender, UINT16 nHP, VEC3D * pPos = nullptr, QUATERNION * pRot = nullptr );

	/** \brief 월드에 떨어진 무기 정보
		\param[in] idx network index
		\param[in] pPos 위치
		\param[in] pMtxRot 회전*/
	void	Write_DroppedWeapon( INT32 idx, VEC3D * pPos, MATRIX * pMtxRot);

	/** \brief animation object 정보를 설정합니다.
		\note 애니메이션 오브젝트는 키프레임만 세팅합니다.
			rBroadcastTime 마다 싱크를 맞추도록 합니다.
		\param[in] idx network index
		\param[in] nHP HP
		\param[in] rKeyFrame key값			*/
	void	Write_Object_Animate( INT32 idx, INT32 EventSender, UINT16 nHP, REAL32 rKeyFrame = 0.0f);

	/** \brief Outpost 헬기용 object 정보를 설정합니다.
		\note 애니메이션 오브젝트는 키프레임만 세팅합니다.
		\param[in] idx network index
		\param[in] nPathKey Path key값
		\param[in] nHP HP
		\param[in] rKeyFrame key값
		\param[in] bSend 정보 Send 유무
				bSend가 false이면 sync time마다 정보를 보내게 됩니다.*/
	void	Write_Object_AnimateForOutpost( INT32 idx, INT32 EventSender, UINT8 nPathKey, UINT16 nHP, REAL32 rKeyFrame, bool bSend);
	
	// 던져진 수류탄을 네트워크 버퍼에 등록합니다.
	void	BindNetGrenadeInfo( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos);

	// 던져진 C5를 네트워크 버퍼에 등록합니다.
	void	BindNetC5Info( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos);

	/** \터치다운을 데디로 알립니다.
		\터치다운 오브젝트와 충돌했을 경우 보냅니다.
		\param[in] pChara			캐릭터 주최
		\param[in] ui16ObjectIdx	충돌한 터치다운 오브젝트 인덱스 */
	void	Write_Chara_TouchDown( CGameCharaBase * pChara, UINT16 ui16ObjectIdx );

	// Server - Client 에서 사용할 키값을 얻어 옵니다.
	INT64	GetSecretKey( UINT64 ui64MacAddress );
};

#endif
