#ifndef _Character_h
#define _Character_h

#include "Skill.h"
#include "WeaponSlot.h"
#include "MultiWeaponSlot.h"
#include "ThrowWeaponMgr.h"
/////////////////////////////////////////////////////////////////////////////////////////
// game character
//
#define HAVE_OBJECT_COUNT	4

class CCharacter
{
public:
	BOOL				GetAlive()										{ return m_bAlive; }
	void				SetAlive(BOOL bRet)								{ m_bAlive = bRet; }

	BOOL				GetOnLoad()										{ return m_bOnLoad; }
	void				SetOnLoad(BOOL bRet)							{ m_bOnLoad = bRet; }

	INT16				GetHP()											{ return m_i16HP; }
	void				SetHP(INT16 i16HP)								{ m_i16HP = i16HP; }

	INT16				GetMaxHP()										{ return m_i16MaxHP; }
	void				SetMaxHP( INT16 i16HP)							{ m_i16HP = m_i16MaxHP = i16HP;	}

	INT32				AddHP(INT16 i16Inc);
	INT32				SubHP(INT16 i16Dec)								{ return m_i16HP -= i16Dec; }

	void				SaveHP()										{ m_i16StoreHP = m_i16HP; m_bStoredHP = TRUE; }

	N_PCINFO_POSROTATION*	GetPosRot()									{ return &m_PosRot;	}
	void					SetPosRot( N_PCINFO_POSROTATION* pPosRot );

	REAL32				GetPosRecvTime() const							{ return m_r32PosRecvTime; }
	void				SetPosRecvTime( REAL32 r32PacketTime )			{ m_r32PosRecvTime = r32PacketTime;	}

	REAL32				CalcSpeed( N_POS* pos0, N_POS* pos1, REAL32 r32Elapsed );
	REAL32				CalcSpeed3D(N_POS* pos0, N_POS* pos1, REAL32 r32Elapsed);
	
	UINT16				GetOnLoadIndex()								{ return m_ui16OnLoadIndex; }
	void				SetOnLoadIndex(UINT16 ui16OnLoadIdx )			{ m_ui16OnLoadIndex = ui16OnLoadIdx; }

	N_PCINFO_CHARA_STATE* GetState()									{ return &m_state; }
	void				SetState(N_PCINFO_CHARA_STATE* pState )			{ I3ASSERT(pState!=NULL); if(pState) pState->CopyTo(&m_state); }

	//////////////////////////////////////////////////////////////////////////
	// Weapon functions

	UINT8				GetSyncWeaponInfo()								{ return m_ui8SyncWeapon; }

	BOOL				GetSyncExtension()								{ return (m_ui8SyncWeapon & 0x80) >> 7; }	// 1000 0000
	void				SetSyncExtension( BOOL bRet )					{ m_ui8SyncWeapon = ((m_ui8SyncWeapon & 0x7F) | (bRet << 7)); }

	UINT8				GetSyncWeaponSlot()								{ return (m_ui8SyncWeapon & 0x70) >> 4; }	// 0111 0000
	void				SetSyncWeaponSlot(UINT8 ui8WeaponSlot)			{ m_ui8SyncWeapon = ((m_ui8SyncWeapon & 0x8F) | (ui8WeaponSlot << 4)); }
	
	UINT8				GetSyncMultiWeaponSlot()						{ return m_ui8SyncWeapon & 0x0F; }	// 0000 1111
	void				SetSyncMultiWeaponSlot(UINT8 ui8MultiWeaponSlot){ m_ui8SyncWeapon = ((m_ui8SyncWeapon & 0xF0) | ui8MultiWeaponSlot);}

	void				ChangeMultiWeapon(UINT8 ui8Usage, UINT8 ui8MultiWeaponIdx);

	UINT32				GetStateOnFire()								{ return m_ui32StateOnFire; }
	void				SetStateOnFire( UINT32 ui32State )				{ m_ui32StateOnFire = ui32State; }

	BOOL				IsDecreaseBullet( UINT16 ui16FireID, const GAMEINFO_CHARA_WEAPON* pWeapon, UINT16 ui16Dec, INT32 &i32BulletExtType);
	BOOL				DecreaseBullet( UINT16 ui16FireID, const GAMEINFO_CHARA_WEAPON* pWeapon, UINT32 ui32Dec );
	BOOL				IsDecreaseBullet( const GAMEINFO_CHARA_WEAPON* pWeapon, UINT16 ui16Dec, INT32 &i32BulletExtType);

	BOOL				DecreaseBullet( const GAMEINFO_CHARA_WEAPON* pWeapon, UINT32 ui32Dec );

	void				SetMultiWeaponSlot( UINT8 ui8MultiWeaponCount );
	void				SetSingleWeaponSlot();

	bool				IsFullSlot( UINT8 ui8Usage );

	UINT32				GetOwnWeapon( UINT32 ui32Slot, UINT8 ui8MultiWeaponSlot );
	void				InitWeapon( T_ItemID TWeaponID, UINT8 ui8Slot );
	UINT32				GetEquipWeaponID( UINT32 ui32Slot );
	void				SetEquipWeapon( T_ItemID TWeaponID, UINT8 ui8Slot );
	UINT8				GetEquipMultiSlotIdx( UINT8 ui8Usage );
	INT8				PickUpWeapon( T_ItemID itemID );
	INT8				PickUpWeapon( GAMEINFO_GETWEAPON * pWeapon );
	INT8				PickUpWeapon( GAMEINFO_DIRECTPICKUP * pWeapon );
	void				DropWeapon( UINT8 ui8Usage );
	void				DropWeapon( UINT8 ui8Usage, UINT8 ui8MultiSlot );

	UINT16				GetCharaMaxHP( UINT8 ui8PBVer = PB_VER_V15 );

	BOOL				IsUseM197()										{ return m_bUseM197; }
	void				SetUseM197(BOOL bUse)							{ m_bUseM197 = bUse; }

	// natural rpg get
	void				SetNaturalRPGGet(BOOL bNatural)					{ m_bNaturalRPGGet = bNatural; }
	BOOL				GetNaturalRPGGet(void)							{ return m_bNaturalRPGGet; }

	void				SetLastFireInfo( REAL32 r32PacketTime, T_ItemID TItemID, WEAPON::ATTACK_TYPE eAttackType )
	{
		m_r32LastFireTime		= r32PacketTime;
		m_TLastFireItemID		= TItemID;			// 마지막 ItemID
		m_eLastFireAttackType	= eAttackType;		// 마지막 AttackType
	}
	REAL32				GetLastFireTime() const							{ return m_r32LastFireTime; }
	T_ItemID			GetLastFireItemID() const						{ return m_TLastFireItemID; }
	WEAPON::ATTACK_TYPE	GetLastFireAttackType() const					{ return m_eLastFireAttackType; }
	
	CSpeedState*		GetSpeedSate()									{ return &m_SpeedState; }

	UINT32				GetFireCount()									{ return m_ui32FireCount; }
	void				SetFireCount( UINT32 ui32Count )				{ m_ui32FireCount = ui32Count; }
	void				ResetFireCount()								{ m_ui32FireCount = 0; }
	void				IncFireCount()									{ m_ui32FireCount++; }

	REAL32				GetFireCheckTime()								{ return m_r32FireCheckTime; }
	void				SetFireCheckTime(REAL32 r32GameTime)			{ m_r32FireCheckTime = r32GameTime;	}

	void				SetCharClass(CHARA_CLASS eCharClass)			{ m_eCharClass = eCharClass; }

	WEAPON::TARGET_TYPE	IsDinoState()									{ return GET_ITEM_SUBTYPE(m_aui32PartsID[CHAR_EQUIPMENT_PARTS_CHARA]) == CHARACTER_CLASS_CHARA_DINO ? WEAPON::TARGET_DINO : WEAPON::TARGET_HUMAN; }

	void				InitBullets( UINT8 ui8PBVer );
	void				SetBullets( WEAPON_SLOT_TYPE eSlotType );
	void				SetBullets( WEAPON_SLOT_TYPE eSlotType, GAMEINFO_BULLET* pBullet );

	BOOL				HaveObject( CDSObject* pObject );
	//////////////////////////////////////////////////////////////////////////
	// Initializer
	void				Reset();
	void				ResetBattleInfo();

	UINT32				getRespawnWeapon(UINT8 ui8Slot)							{ return ui8Slot < WEAPON_SLOT_MELEE ? m_ui32WeaponRespawn[ui8Slot] : 0;	}
	void				setRespawnWeapon(UINT32 ui32WeaponID, UINT8 ui8Slot)	{ if( ui8Slot < WEAPON_SLOT_MELEE)	m_ui32WeaponRespawn[ ui8Slot ] = ui32WeaponID;	}

	//////////////////////////////////////////////////////////////////////////
	// Parts ( Equipment )
	UINT32*				GetAllParts()									{ return m_aui32PartsID; }
	UINT32				GetParts(INT32 i32PartsIdx)						{ return i32PartsIdx < CHAR_EQUIPMENT_PARTS_COUNT ? m_aui32PartsID[i32PartsIdx] : 0; }
	void				SetParts( INT32 i32PartsIdx, T_ItemID TItemID, UINT8 ui8PBVer )
	{
		if( CHAR_EQUIPMENT_PARTS_COUNT <= i32PartsIdx )	return;

		m_aui32PartsID[ i32PartsIdx ] = TItemID;

		EquipmentInfo EquipInfo;
		EquipmentTable::GetInstance().Find( TItemID, EquipInfo, (PB_VER)ui8PBVer );

		m_r32PartsAbility[ EQUIP::eABILITY_THROWCOUNT ]	+= EquipInfo.GetAbilityValue( EQUIP::eABILITY_THROWCOUNT );
		m_r32PartsAbility[ EQUIP::eABILITY_MAXBULLET ]	+= EquipInfo.GetAbilityValue( EQUIP::eABILITY_MAXBULLET );
	}

	UINT32				GetSlotItem()									{	return m_ui32SlotItems;	}
	void				SetSlotItem( UINT32 ui32SlotItem )				{	m_ui32SlotItems = ui32SlotItem;	}

	//////////////////////////////////////////////////////////////////////////
	// Damage functions
	REAL32				GetDamageRateByHitPart(CHARA_HIT_PART eHitPart);
	REAL32				GetDamageIncRate(WEAPON_SLOT_TYPE eWeaponUsage, WEAPON_CLASS_TYPE eWeaponClass, UINT8 ui8PBVer );
	REAL32				GetDamageIncPlus(WEAPON_SLOT_TYPE eWeaponUsage, WEAPON_CLASS_TYPE eWeaponClass, UINT8 ui8PBVer );
	REAL32				GetDefenceRate( CHARA_HIT_PART eHitPart, UINT8 ui8PBVer );

	REAL32				GetRangeRate();
	WEAPON_CLASS_TYPE	GetSpecialtyWeapon();

	//////////////////////////////////////////////////////////////////////////
	// Abilities functions
	void				ApplySkills(CHARA_SLOT_SKILL* pSkills);
	REAL32				GetDamageIncRateBySkill(WEAPON_SLOT_TYPE eWeaponUsage, WEAPON_CLASS_TYPE eWeaponClass);
	REAL32				GetDefenceRateBySkill();
	REAL32				GetAddSpeedRate( UINT8 ui8PBVer );
		
	//////////////////////////////////////////////////////////////////////////
	// Hacking check functions

	bool				IsAbleToCritialWeapon(INT32 i32HitRatioCnt){if( i32HitRatioCnt == 0 ) return false;	return true;}
	bool				IsAbleToCritialHit(INT32 i32HitRatioCnt)	{return (m_i32HitCount / i32HitRatioCnt)  <= m_i32CriticalHitCount;}
	void				IncHitCount()								{ m_i32HitCount++; }
	void				IncCritialHitCount()						{ m_i32CriticalHitCount++; }
	

	REAL32				GetLastClientTime()							{ return m_r32LastClientTime; }
	REAL32				GetLastDediTime()							{ return m_r32LastDediTime; }
	REAL32				GetLatencyTime()							{ return m_r32LatencyTime; }

	void				SetLastClientTime( REAL32 r32Time )			{ m_r32LastClientTime = r32Time; }
	void				SetLastDediTime( REAL32 r32Time )			{ m_r32LastDediTime = r32Time; }
	void				SetLatencyTime( REAL32 r32Time )			{ m_r32LatencyTime = r32Time; }
	
	REAL32				GetaverageLatency()							{ return m_r32averageLatency; }
	UINT32				GetaverageCount()							{ return m_ui32averageCount; }

	void				SetaverageLatency( REAL32 r32Time )			{ m_r32averageLatency = r32Time; }
	void				SetaverageCount( UINT32 ui32Count )			{ m_ui32averageCount = ui32Count; }

	void				PlusAverageLatency( REAL32 r32Time )		{ m_r32averageLatency += r32Time; }
	void				PlusAverageCount()							{ ++m_ui32averageCount; }
	
	REAL32				GetMinLatencyTime()							{ return m_r32MinLatencyTime; }
	void				SetMinLatencyTime( REAL32 r32Time )			{ m_r32MinLatencyTime = r32Time; }
	REAL32				GetMaxLatencyTime()							{ return m_r32MaxLatencyTime; }
	void				SetMaxLatencyTime( REAL32 r32Time )			{ m_r32MaxLatencyTime = r32Time; }

	BOOL				AddObject( CDSObject* pObject );
	BOOL				DeleteObject( CDSObject* pObject );
	void				DeleteAllObject( N_POS* pvec32Pos );
	BOOL				GetHaveObject()								{	return m_i8HaveObjectCount>0?TRUE:FALSE;	} 

	BOOL				IsEquipWeapon( T_ItemID );

//	BOOL				AddThrowWeapon( UINT16 ui16Idx, GAMEINFO_THROW_WEAPON_NETDATA* pThrow );

	void				SetJumpholdSpeed(REAL32 r32JumpSpeed)		{ m_r32JumpHoldSpeed = r32JumpSpeed; }
	REAL32				GetJumpholdSpeed()							{ return m_r32JumpHoldSpeed; }
	REAL32				GetAddSpeed();
public:
	CCharacter();
	virtual ~CCharacter();


private:
	CHARA_CLASS				m_eCharClass;

	BOOL					m_bAlive;					// 캐릭터 생/사 상태 ( Respawn 상태로 봐도 됨 )
	BOOL					m_bOnLoad;					// 움직이는 오브젝트 위에 있는지
														// 애니메이션 오브젝트의 경우 위치 계산이 안되고 월드좌표가 아닌 해당 오브젝트내의 로컬좌표로 오기 때문에 계산이 불가능

	INT16					m_i16HP;					// 캐릭터 HP
	INT16					m_i16MaxHP;					// 캐릭터 기본 HP
	INT16					m_i16StoreHP;				// 애시드자폭시 저장 HP
	BOOL					m_bStoredHP;				// 애시드자폭저장 HP 저장여부

	N_PCINFO_POSROTATION	m_PosRot;					// 최근 수신된 포지션
	REAL32					m_r32PosRecvTime;			// 최근 포지션 수신 시간
	UINT16					m_ui16OnLoadIndex;			// 캐릭터 오브젝트 탑승 인덱스
	UINT32					m_ui32StateOnFire;			// 캐릭터 Fire시 상태
	N_PCINFO_CHARA_STATE	m_state;					// 캐릭터 상태.

	BOOL					m_bUseM197;					// 오브젝트 사용상태 체크
	BOOL					m_bNaturalRPGGet;			// 정상적인 GetWeapon 으로 RPG를 습득하였는가

	REAL32					m_r32LastFireTime;			// 저속총에 발사 속도 검사용 (샷건, 저격총일 때 FIRE시간. CLIENT PACKET TIME)
	UINT32					m_ui32FireCount;			// 고속총에 대한 발사 속도 (1초간 누적하고 리셋)
	REAL32					m_r32FireCheckTime;			// check time
	T_ItemID				m_TLastFireItemID;			// 마지막 ItemID
	WEAPON::ATTACK_TYPE		m_eLastFireAttackType;		// 마지막 AttackType

	CSpeedState				m_SpeedState;				// 스피드 핵 검사 

	REAL32					m_r32PartsAbility[ EQUIP::eABILITY_COUNT ];
	// -------- 무기 슬롯은 m_pWeapon을 사용 --------
	WeaponSlotBase*			m_pWeapon[ WEAPON_SLOT_COUNT ];	
	WeaponSlot				m_SingleWeaponSlot[ WEAPON_SLOT_COUNT ];	// **직접 조작X** 위의 m_pWeapon을 사용해주세요!!
	MultiWeaponSlot			m_MultiWeaponSlot;				// **직접 조작X** 다중무기모드일 경우, m_pWeapon[WEAPON_SLOT_PRIMARY]이 MultiSlot으로 변경 됨
	// ----------------------------------------------
	UINT32					m_ui32WeaponRespawn[WEAPON_SLOT_MELEE];			// 리스폰 패킷에 의해서 초기화 - 터치 다운 진행 시 리스폰 무기로 변경할때 사용
																			// 주무기, 보조무기만 Get/Drop 으로 변경 될 수 있으므로 주무기, 보조무기만 저장함

	//UINT16					m_ui16WeaponIdx[WEAPON_SLOT_COUNT];			// 무기가 변경된 값
	//UINT16					m_ui16FireID[WEAPON_SLOT_COUNT];			// 마지막으로 발사한 fireID
	//UINT16					m_ui16HitID[WEAPON_SLOT_COUNT];				// 마지막으로 발생한 HitID

	UINT16					m_ui16LastFireID;			// 마지막으로 발사한 Fire ID

	INT8					m_i8HaveObjectCount;
	CDSObject*				m_pHaveObject[ HAVE_OBJECT_COUNT ];
	BOOL					m_bHaveObjectPos;

	// --------m_ui8SyncWeapon----------
	// | 1 | 2 | 2 | 2 | 3 | 3 | 3 | 3 |	// 1. Extension상태(1bit), 2. Usage(3bit), MultiSlotIdx(4bit)
	// ---------------------------------
	UINT8					m_ui8SyncWeapon;			// 싱크용 무기 정보(위의 그림 참조)

	UINT32					m_aui32PartsID[ CHAR_EQUIPMENT_PARTS_COUNT ];// 장비 정보 ( 무기 제외, 캐릭터 포함 )
	UINT32					m_ui32SlotItems;								// 슬롯 아이템 정보 ( 탄알 수 증가 아이템, 할로우탄 같은... )

	INT32					m_i32HitCount;
	INT32					m_i32CriticalHitCount;

	REAL32					m_r32LastClientTime;
	REAL32					m_r32LastDediTime;
	REAL32					m_r32LatencyTime;
	
	REAL32					m_r32TenSecTime;
	REAL32					m_r32TenSecLatency;
	UINT32					m_ui32LatencyCount;

	REAL32					m_r32averageLatency;
	UINT32					m_ui32averageCount;

	REAL32					m_r32MinLatencyTime;
	REAL32					m_r32MaxLatencyTime;

	CDediSkill				m_aSkills[SKILL_CARTEGORY_MAX];

	REAL32					m_r32JumpHoldSpeed;
};

#endif
