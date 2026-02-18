#include "pch.h"
#include "Weapon.h"
#include "Character.h"
#include "CharacterTable.h"
#include "EquipmentTable.h"
#include "SkillTable.h"
#include "hms.h"

CCharacter::CCharacter()
{
	m_i16MaxHP = m_i16HP = 100; 
	m_i8HaveObjectCount		= 0;
	for( INT32 i = 0 ; i < WEAPON_SLOT_COUNT ; i++ )
	{
		m_pWeapon[ i ] = &m_SingleWeaponSlot[ i ];
	}
	Reset();
}

CCharacter::~CCharacter()
{
}

void CCharacter::Reset()
{
	ResetBattleInfo();
	m_ui32SlotItems = 0;
	m_bStoredHP = FALSE;
	m_r32JumpHoldSpeed = 0.0f;
	i3mem::FillZero( m_ui32WeaponRespawn,	sizeof(m_ui32WeaponRespawn) );
	i3mem::FillZero( m_aui32PartsID,		sizeof(m_aui32PartsID) );
	i3mem::FillZero( m_r32PartsAbility,		sizeof(m_r32PartsAbility) );

	for( INT32 i = 0 ; i < WEAPON_SLOT_COUNT ; i++ )
	{
		m_pWeapon[ i ]->Reset();
	}
}

void	CCharacter::ResetBattleInfo()
{
	m_PosRot.Reset();
	m_state.Reset();
	m_SpeedState.Reset();

	m_i16MaxHP			= GetCharaMaxHP();
	m_i16HP				= m_bStoredHP ? m_i16StoreHP : m_i16MaxHP;
	m_bStoredHP			= FALSE;

	m_bAlive			= FALSE;
	m_bOnLoad			= FALSE;
	m_ui8SyncWeapon		= 0;
	m_bOnLoad			= FALSE;
	m_bUseM197			= FALSE;

	m_r32PosRecvTime	= 0.0f;
	m_r32LastFireTime	= 0.0f;
	m_r32FireCheckTime	= 0.0f;
	m_ui16OnLoadIndex	= 0;
	m_ui32StateOnFire	= 0;
	m_ui32FireCount		= 0;

	m_i32HitCount			= 0;
	m_i32CriticalHitCount	= 0;

	for( INT32 i = 0 ; i < m_i8HaveObjectCount ; i++ )
	{
		m_pHaveObject[ i ]->ResetUse();
	}
	m_i8HaveObjectCount		= 0;

	m_ui16LastFireID	= 0;
	//i3mem::FillZero( m_ui16FireID,	sizeof(m_ui16FireID) );
	//i3mem::FillZero( m_ui16HitID,	sizeof(m_ui16HitID) );
	m_r32JumpHoldSpeed = 0.0f;
}

// 캐릭터의 Max HP를 얻는다 (슬롯 아이템 포함)
UINT16 CCharacter::GetCharaMaxHP( UINT8 ui8PBVer )
{
	// vv3 - 체크
	CharacterInfo charaInfo;
	CharacterTable::GetInstance().Find( m_aui32PartsID[CHAR_EQUIPMENT_PARTS_CHARA], charaInfo );

	UINT16 iExtraHp = 0; 
	if( m_ui32SlotItems & SLOT_ITEM_MEGA_HP5  )			iExtraHp = (UINT16)(charaInfo.m_ui16HP * HP_RATE_MEGA_HP5);
	if( m_ui32SlotItems & SLOT_ITEM_MEGA_HP10 )			iExtraHp = (UINT16)(charaInfo.m_ui16HP * HP_RATE_MEGA_HP10);
	if( m_ui32SlotItems & SLOT_ITEM_UPGRADE_REINFORCED	)		
	{
		iExtraHp = (UINT16)(charaInfo.m_ui16HP * HP_RATE_UPGRADE_REINFORCED);
	}
	if( m_ui32SlotItems & SLOT_ITEM_VALENTINE_CHOCOLATE)iExtraHp += HP_UP_VALENTINE_CHOCOLATE;
	if( m_ui32SlotItems & SLOT_ITEM_KETUPAT	)			iExtraHp += HP_UP_KETUPAT;
	
	
	// vv3 - 체크
	EquipmentInfo EquipInfo;
	for( INT32 i = CHAR_EQUIPMENT_PARTS_HEAD; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
	{
		if( m_aui32PartsID[i] == 0 ) continue;

		EquipmentTable::GetInstance().Find( m_aui32PartsID[i], EquipInfo, (PB_VER)ui8PBVer );
		REAL32 r32Ability = EquipInfo.GetAbilityValue( EQUIP::eABILITY_HPINCREASE );
		
		if( r32Ability > 0.001f )
		{
			iExtraHp += (UINT16)(charaInfo.m_ui16HP * ( r32Ability + 0.0001f ));
		}
	}

	// 상한선 0.2f. 기획의도입니다. 2014.06.16 노준영
	iExtraHp = (UINT16)(MIN( iExtraHp, (charaInfo.m_ui16HP * MAX_RATE_UPGREADE_STATS ) ));

	return charaInfo.m_ui16HP + iExtraHp;
}

INT32 CCharacter::AddHP(INT16 i16Inc)
{
	m_i16HP = min(m_i16MaxHP, m_i16HP + i16Inc);

	return m_i16HP;
}

REAL32 CCharacter::CalcSpeed(N_POS* pos0, N_POS* pos1, REAL32 r32Elapsed)
{
	if( r32Elapsed < I3_EPS )
		return 0.0f;

	I3ASSERT( IsOverflow(pos0) == FALSE );
	I3ASSERT( IsOverflow(pos1) == FALSE );

	return static_cast<REAL32>( sqrt(distance_squared2D(pos0, pos1)) / r32Elapsed );
}

REAL32 CCharacter::CalcSpeed3D(N_POS* pos0, N_POS* pos1, REAL32 r32Elapsed)
{
	if (r32Elapsed < I3_EPS)
		return 0.0f;

	I3ASSERT(IsOverflow(pos0) == FALSE);
	I3ASSERT(IsOverflow(pos1) == FALSE);

	return static_cast<REAL32>(sqrt(distance_squared(pos0, pos1)) / r32Elapsed);
}

REAL32 CCharacter::GetDamageRateByHitPart(CHARA_HIT_PART eHitPart)
{
	// Character base pointer 들고 있도록 수정
	// vv3 - 체크
	CharacterInfo stCharBase;
	if( !CharacterTable::GetInstance().Find(m_aui32PartsID[CHAR_EQUIPMENT_PARTS_CHARA], stCharBase) )
	{
		DSERROR("[ERROR][CCharacter::GetDamageRateByHitPart] Not found character base data. (Char ID : %d)", m_aui32PartsID[CHAR_EQUIPMENT_PARTS_CHARA]);
		return 0.0f;
	}

	return stCharBase.GetHitPositionRate(eHitPart);
}

REAL32 CCharacter::GetDefenceRate( CHARA_HIT_PART eHitPart, UINT8 ui8PBVer )
{	
	//방어력 증가 장비 계산
	REAL32 r32EquipRate = 0.0f;
	
	for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
	{
		if( i == CHAR_EQUIPMENT_PARTS_CHARA )
			continue;

		EquipmentInfo stEquipment;
		if( !EquipmentTable::GetInstance().Find(m_aui32PartsID[i], stEquipment, (PB_VER)ui8PBVer) )
			continue;

		r32EquipRate += stEquipment.GetDamageReduceRate(eHitPart);
	}

	//방어력 증가 스킬 계산
	REAL32 r32SkillRate = 0.0f;

	r32SkillRate = GetDefenceRateBySkill();

	
	//방어력 증가 아이템 계산
	REAL32 r32ItemRate = 0.0f;

	if( m_ui32SlotItems & SLOT_ITEM_UPGRADE_DEFENSE )
	{
		r32ItemRate = DEFENCE_RATE_UPGRADE_DEFENSE;
	}

	//방탄조끼 방어력 계산(사용중인 아이템중 가장 높은 수치만 적용됩니다.)
	REAL32 r32BulletVestRate = 0.0f;
		
	if( m_ui32SlotItems & SLOT_ITEM_BULLET_PROOF_VEST_METAL )
	{
		r32BulletVestRate = DEFENCE_RATE_VEST_METAL;
	}
	else if( m_ui32SlotItems & SLOT_ITEM_BULLET_PROOF_VEST_PLUS )
	{
		r32BulletVestRate = DEFENCE_RATE_VEST_PLUS;
	}
	else if( m_ui32SlotItems & SLOT_ITEM_BULLET_PROOF_VEST )
	{
		r32BulletVestRate = DEFENCE_RATE_VEST;
	}

	REAL32 r32ReduceRate = 0.0f;

	//유저 버전에 따라 방어력 증가량을 계산한다.
	switch (ui8PBVer)
	{

	case PB_VER_V10: //각비율간 곱연산(복리)
		r32ReduceRate = ( 1.0f - r32EquipRate ) * ( 1.0f - r32SkillRate ) * ( 1.0f - r32ItemRate ) * ( 1.0f - r32BulletVestRate );
		break;

	case PB_VER_V15: //각비율간 합연산(단리)
	default:
	{
		r32ReduceRate = r32EquipRate + r32SkillRate + r32ItemRate + r32BulletVestRate;
		// 상한선 0.2f. 기획의도입니다. 2014.06.16 노준영 - V2한정
		r32ReduceRate = MIN(r32ReduceRate, MAX_RATE_UPGREADE_STATS);
		r32ReduceRate = 1.0f - r32ReduceRate;
	}	
	break;
		
	}

	// GM 방탄복은  우선 대상에서 제외합니다.
	if( m_ui32SlotItems & SLOT_ITEM_BULLET_PROOF_VEST_GM )
		r32ReduceRate = DEFENCE_RATE_VEST_GM;

	return r32ReduceRate;
}

REAL32 CCharacter::GetRangeRate()
{
	REAL32 r32AddRange = 0.0f;
		
	for( INT32 i = 0; i < SKILL_CARTEGORY_MAX; ++i )
		r32AddRange += m_aSkills[i].GetValue(SKILLLIST_RANGE);

	return r32AddRange;
}

REAL32 CCharacter::GetDamageIncRate(WEAPON_SLOT_TYPE eWeaponUsage, WEAPON_CLASS_TYPE eWeaponClass, UINT8 ui8PBVer )
{
	//공격력 증가 장비 계산
	REAL32 r32EquipRate = 0.0f;
		
	for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
	{
		if( CHAR_EQUIPMENT_PARTS_CHARA == i ) continue;

		EquipmentInfo stEquipment;
		if( !EquipmentTable::GetInstance().Find(m_aui32PartsID[i], stEquipment, (PB_VER)ui8PBVer) )
			continue;
		
		r32EquipRate += stEquipment.GetAbilityValue(EQUIP::eABILITY_DAMAGERATE);
	}

	//공격력 증가 스킬 계산
	REAL32 r32SkillRate = 0.0f;

	r32SkillRate = GetDamageIncRateBySkill(eWeaponUsage, eWeaponClass);

	//공격력 증가 아이템 계산
	REAL32 r32ItemRate = 0.0f;
		
	if( m_ui32SlotItems & SLOT_ITEM_UPGRADE_DAMAGE )
	{
		r32ItemRate = DAMAGE_RATE_UPGRADE_DAMAGE;
	}

	//공격력 증가 특수탄 계산(PBTN 마스크 예외처리 포함)
	REAL32 r32AmmoRate = 0.0f;
	
	//Hollow Point Ammo
	if( m_ui32SlotItems & SLOT_ITEM_HOLLOW_POINT_AMMO )
	{
		r32AmmoRate += DAMAGE_RATE_HOLLOW_POINT;
	}

	//Hollow Point Ammo Plus
	if( m_ui32SlotItems & SLOT_ITEM_HOLLOW_POINT_AMMO_PLUS )
	{
		r32AmmoRate += DAMAGE_RATE_ENHANCED_HOLLOW_POINT;
	}

	//Jacketed Hallow Point Ammo
	if( m_ui32SlotItems & SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO )
	{
		r32AmmoRate += DAMAGE_RATE_JACKETED_HOLLOW_POINT;
	}

	//Full metal Jacket Ammo
	if( m_ui32SlotItems & SLOT_ITEM_FULLMETALJACKETAMMO )
	{
		r32AmmoRate += DAMAGE_RATE_ARMOR_PIERCING;
	}

	//PBTN Mask예외처리
	if ((PB_VER_V10 == ui8PBVer) && (1104003077 == m_aui32PartsID[CHAR_EQUIPMENT_PARTS_HEAD]))
	{
		r32AmmoRate += DAMAGE_RATE_ENHANCED_HOLLOW_POINT;
	}

	REAL32 r32IncreaseRate;

	//유저 버전에 따라 공격력 증가량을 계산한다.
	switch (ui8PBVer)
	{

	case PB_VER_V10: //각비율간 곱연산(복리)
		r32IncreaseRate = (1.0f + r32EquipRate) * (1.0f + r32SkillRate) * (1.0f + r32ItemRate) * (1.0f + r32AmmoRate);
		break;

	case PB_VER_V15: //각비율간 합연산(단리)
	default:
	{
		r32IncreaseRate = r32EquipRate + r32SkillRate + r32ItemRate + r32AmmoRate;
		// 상한선 0.2f. 기획의도입니다. 2014.06.16 노준영
		r32IncreaseRate = MIN(r32IncreaseRate, MAX_RATE_UPGREADE_STATS);
		r32IncreaseRate += 1.0f;
	}
	break;
	}
	
	return r32IncreaseRate;
}

REAL32 CCharacter::GetDamageIncPlus(WEAPON_SLOT_TYPE eWeaponUsage, WEAPON_CLASS_TYPE eWeaponClass, UINT8 ui8PBVer )
{
	REAL32 r32IncreasePlus = 0.0f;

	// vv3 - 체크
	for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
	{
		if( CHAR_EQUIPMENT_PARTS_CHARA == i ) continue;

		EquipmentInfo stEquipment;
		if( !EquipmentTable::GetInstance().Find(m_aui32PartsID[i], stEquipment, (PB_VER)ui8PBVer) )
			continue;
		
		r32IncreasePlus += stEquipment.GetAbilityValue(EQUIP::eABILITY_DAMAGE);
	}

	return r32IncreasePlus;
}

REAL32 CCharacter::GetAddSpeedRate( UINT8 ui8PBVer )
{
	REAL32 r32AddSpeedValue = 0.0f;
	EquipmentInfo stitem;

	// 스킬로 인한 증가값
	for( INT32 i = 0; i < SKILL_CARTEGORY_MAX; ++i )
	{
		r32AddSpeedValue += m_aSkills[i].GetValue(SKILLLIST_RUN);
	}
	
	// vv3 - 체크
	// 장비로 인한 증가값
	for( INT32 i = CHAR_EQUIPMENT_PARTS_HEAD; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
	{
		if( m_aui32PartsID[i] == 0 ) continue;

		EquipmentTable::GetInstance().Find( m_aui32PartsID[i], stitem, (PB_VER)ui8PBVer );
		r32AddSpeedValue += stitem.GetAbilityValue( EQUIP::eABILITY_MOVESPEED_FRONT );
	}

	// 아이템으로 인한 증가값
	if( m_ui32SlotItems & SLOT_ITEM_UPGRADE_REINFORCED	)		
	{
		r32AddSpeedValue += HP_RATE_UPGRADE_REINFORCED;
	}
	
	// 상한선 0.2f. 기획의도입니다. 2014.06.16 노준영
	r32AddSpeedValue = MIN( r32AddSpeedValue, MAX_RATE_UPGREADE_STATS );

	return r32AddSpeedValue;
}

REAL32 CCharacter::GetDamageIncRateBySkill(WEAPON_SLOT_TYPE eWeaponUsage, WEAPON_CLASS_TYPE eWeaponClass)
{
	REAL32 r32SkillValue = 0.0f;

	for( INT32 i = 0; i < SKILL_CARTEGORY_MAX; ++i )
	{
		if( !(m_aSkills[i].GetApplyWeaponSlot() &  (1 << eWeaponUsage)) )
			continue;

		if( static_cast<SKILL_CARTEGORY>(i) == SKILL_TYPE_MAIN )
		{
			if( GetSpecialtyWeapon() != eWeaponClass )
				continue;
		}

		r32SkillValue += m_aSkills[i].GetValue(SKILLLIST_DAMAGE);
	}

	return r32SkillValue;
}

REAL32 CCharacter::GetDefenceRateBySkill()
{
	REAL32 r32SkillValue = 0.0f;

	for( INT32 i = 0; i < SKILL_CARTEGORY_MAX; ++i )
		r32SkillValue += m_aSkills[i].GetValue(SKILLLIST_DEFENSE);

	return r32SkillValue;
}

WEAPON_CLASS_TYPE CCharacter::GetSpecialtyWeapon()
{
	switch( m_eCharClass )
	{
	case CHARA_CLASS_ASSAULT:
		return WEAPON_CLASS_ASSAULT;

	case CHARA_CLASS_SMG:
		return WEAPON_CLASS_SMG;

	case CHARA_CLASS_SNIPER:
		return WEAPON_CLASS_SNIPER;

	case CHARA_CLASS_SHOTGUN:
		return WEAPON_CLASS_SHOTGUN;

	case CHARA_CLASS_MACHINE:
		return WEAPON_CLASS_MG;
	}

	return WEAPON_CLASS_UNKNOWN;
}

void CCharacter::ApplySkills(CHARA_SLOT_SKILL* pSkill)
{
	SkillTable& rSkillTable = SkillTable::GetInstance();

	m_aSkills[SKILL_TYPE_COMMON].SetBaseData( rSkillTable.FindSkillInfo(SKILL_TYPE_COMMON, pSkill->m_CommonSkill) );
	m_aSkills[SKILL_TYPE_COMMON].SetSkillLevel( pSkill->m_ui8CommonSkillLv );

	m_aSkills[SKILL_TYPE_MAIN].SetBaseData( rSkillTable.FindSkillInfo(SKILL_TYPE_MAIN, pSkill->m_MainSkill) );
	m_aSkills[SKILL_TYPE_MAIN].SetSkillLevel( pSkill->m_ui8MainSkillLv );

	m_aSkills[SKILL_TYPE_ASSIST].SetBaseData( rSkillTable.FindSkillInfo(SKILL_TYPE_ASSIST, pSkill->m_AssistSkill) );
	m_aSkills[SKILL_TYPE_ASSIST].SetSkillLevel( pSkill->m_ui8AssistSkillLv );
}

void CCharacter::ChangeMultiWeapon(UINT8 ui8Usage, UINT8 ui8MultiWeaponIdx)
{
	m_pWeapon[ui8Usage]->SetEquip(ui8MultiWeaponIdx);
}

void CCharacter::SetMultiWeaponSlot( UINT8 ui8MultiWeaponCount )
{	// 주무기를 멀티슬롯으로 변경
	m_MultiWeaponSlot.Init(ui8MultiWeaponCount);
	m_pWeapon[WEAPON_SLOT_PRIMARY] = &m_MultiWeaponSlot;
}

void CCharacter::SetSingleWeaponSlot()
{	// 주무기를 기본 슬롯으로 변경
	m_pWeapon[WEAPON_SLOT_PRIMARY] = &m_SingleWeaponSlot[WEAPON_SLOT_PRIMARY];
}

bool CCharacter::IsFullSlot( UINT8 ui8Usage )
{
	return m_pWeapon[ui8Usage]->IsFullSlot();
}

UINT32 CCharacter::GetOwnWeapon( UINT32 ui32Slot, UINT8 ui8MultiWeaponSlot )					
{ 
	if( ui32Slot < WEAPON_SLOT_COUNT ) 
	{
		CWeapon *pWeapon = m_pWeapon[ui32Slot]->GetOwn(ui8MultiWeaponSlot);
		if( pWeapon != NULL )
			return pWeapon->GetWeaponID();
	}
	return 0; 
}

void CCharacter::InitWeapon( T_ItemID TWeaponID, UINT8 ui8Slot )
{
	if( WEAPON_SLOT_COUNT <= ui8Slot )	return;

	m_pWeapon[ ui8Slot ]->InitWeapon( TWeaponID, ui8Slot );
}

UINT32 CCharacter::GetEquipWeaponID( UINT32 ui32Slot )					
{ 
	if( ui32Slot < WEAPON_SLOT_COUNT ) 
	{
		return m_pWeapon[ui32Slot]->GetEquip()->GetWeaponID();
	}
	else return 0; 
}

void CCharacter::SetEquipWeapon( T_ItemID TWeaponID, UINT8 ui8Slot )
{
	if( WEAPON_SLOT_COUNT <= ui8Slot )	return;

	m_pWeapon[ ui8Slot ]->GetEquip()->SetWeapon( TWeaponID, ui8Slot );
}

UINT8  CCharacter::GetEquipMultiSlotIdx( UINT8 ui8Usage )
{
	return m_pWeapon[ ui8Usage ]->GetEquipIdx();
}

INT8 CCharacter::PickUpWeapon( T_ItemID itemID )
{
	WeaponInfo* pWeaponInfo = Table::GetWeaponInfo(itemID);

	if (pWeaponInfo != NULL)
	{
		WEAPON_SLOT_TYPE eSlot = (WEAPON_SLOT_TYPE)(GET_ITEM_TYPE(itemID) - 1);

		CWeapon WeaponInfo;
		WeaponInfo.SetWeapon( itemID, eSlot );
		WeaponInfo.SetBullets( NULL, 0 );

		return m_pWeapon[eSlot]->Add(&WeaponInfo);
	}
	return -1;
}

INT8 CCharacter::PickUpWeapon( GAMEINFO_GETWEAPON * pWeapon )
{
	CWeapon WeaponInfo;
	INT32 i32Usage = pWeapon->getWeaponUsage();

	if( i32Usage == WEAPON_SLOT_UNKNOWN ) 
	{
		return -1;
	}

	WeaponInfo.SetWeapon( pWeapon->getWeaponID(), i32Usage );
	WeaponInfo.SetBullets( pWeapon->GetBullet() );

	return m_pWeapon[i32Usage]->Add(&WeaponInfo);
}

INT8 CCharacter::PickUpWeapon( GAMEINFO_DIRECTPICKUP * pWeapon )
{
	UINT8 ui8Slot = pWeapon->getWeaponUsage();

	CWeapon WeaponInfo;
	WeaponInfo.SetWeapon( pWeapon->getWeaponID(), ui8Slot );
	WeaponInfo.SetBullets( NULL, 0 );

	return m_pWeapon[ui8Slot]->Add(&WeaponInfo);
}

void CCharacter::DropWeapon( UINT8 ui8Usage )
{
	m_pWeapon[ui8Usage]->Drop( );
}

void  CCharacter::DropWeapon( UINT8 ui8Usage, UINT8 ui8MultiSlot )
{
	m_pWeapon[ui8Usage]->Drop( ui8MultiSlot );
}

void CCharacter::InitBullets( UINT8 ui8PBVer )
{
	for( INT32 i = 0 ; i < WEAPON_SLOT_COUNT ; i++ )
	{
		m_pWeapon[ i ]->GetEquip()->SetBullets( m_r32PartsAbility, m_ui32SlotItems );
	}
}

BOOL CCharacter::IsDecreaseBullet( UINT16 ui16FireID, const GAMEINFO_CHARA_WEAPON* pWeapon, UINT16 ui16Dec, INT32 &i32BulletExtType)
{
	if( ui16FireID == m_ui16LastFireID )	return FALSE;

	return IsDecreaseBullet( pWeapon, ui16Dec, i32BulletExtType);
}
BOOL CCharacter::DecreaseBullet( UINT16 ui16FireID, const GAMEINFO_CHARA_WEAPON* pWeapon, UINT32 ui32Dec )
{
	if( ui16FireID == m_ui16LastFireID )	return FALSE;

	m_ui16LastFireID = ui16FireID;

	return DecreaseBullet( pWeapon, ui32Dec);
}
BOOL CCharacter::IsDecreaseBullet( const GAMEINFO_CHARA_WEAPON* pWeapon, UINT16 ui16Dec, INT32 &i32BulletExtType)
{
	WEAPON_SLOT_TYPE eWeaponSlotType = pWeapon->getWeaponUsage();

	if( eWeaponSlotType < 0)					return FALSE;
	if( eWeaponSlotType >= WEAPON_SLOT_COUNT)	return FALSE;

	return m_pWeapon[ eWeaponSlotType ]->GetEquip()->IsDecreaseBullet( pWeapon, ui16Dec, i32BulletExtType);
}

BOOL CCharacter::DecreaseBullet( const GAMEINFO_CHARA_WEAPON* pWeapon, UINT32 ui32Dec )
{
	WEAPON_SLOT_TYPE eWeaponSlotType = pWeapon->getWeaponUsage();

	if( eWeaponSlotType < 0)					return FALSE;
	if( eWeaponSlotType >= WEAPON_SLOT_COUNT)	return FALSE;

	return m_pWeapon[ eWeaponSlotType ]->GetEquip()->DecreaseBullet( pWeapon, ui32Dec);
}

void CCharacter::SetBullets( WEAPON_SLOT_TYPE eSlotType )
{
	m_pWeapon[ eSlotType ]->GetEquip()->SetBullets( NULL, 0 );
}

void CCharacter::SetBullets( WEAPON_SLOT_TYPE eSlotType, GAMEINFO_BULLET* pBullet )
{
	m_pWeapon[ eSlotType ]->GetEquip()->SetBullets( pBullet );
}

void CCharacter::SetPosRot( N_PCINFO_POSROTATION* pPosRot )
{
	pPosRot->CopyTo( &m_PosRot );
}

BOOL CCharacter::AddObject( CDSObject* pObject )
{
	if( HAVE_OBJECT_COUNT <= m_i8HaveObjectCount )	return FALSE;
	
	m_pHaveObject[ m_i8HaveObjectCount ]	= pObject;

	m_i8HaveObjectCount++;
	
	return TRUE;
}

BOOL CCharacter::DeleteObject( CDSObject* pObject )
{
	INT32 i32Count;
	for( INT32 i = 0 ; i < m_i8HaveObjectCount ; i++ )
	{
		if( pObject != m_pHaveObject[ i ] )	continue;

		pObject->ResetUse( &m_PosRot._Pos );

		i32Count = m_i8HaveObjectCount - i - 1;
		
		if( 0 < i32Count )
		{
			memcpy( m_pHaveObject[ i ], m_pHaveObject[ i+1 ], sizeof(CDSObject)*i32Count );
		}

		m_i8HaveObjectCount--;

		return TRUE;
	}

	return FALSE;
}

void CCharacter::DeleteAllObject( N_POS* pvec32Pos )
{
	CDSObject* pObject;
	for( INT32 i = 0 ; i < m_i8HaveObjectCount ; i++ )
	{
		pObject = m_pHaveObject[ i ];
		pObject->ResetUse( pvec32Pos );
	}
	m_i8HaveObjectCount = 0;
	m_bHaveObjectPos	= FALSE;
}

BOOL CCharacter::HaveObject( CDSObject* pObject )
{
	for( INT32 i = 0 ; i < m_i8HaveObjectCount ; i++ )
	{
		if( m_pHaveObject[i] == pObject )	return TRUE;
	}
	return FALSE;
}  

BOOL CCharacter::IsEquipWeapon( T_ItemID TItemID )
{
	UINT32 ui32Usage = Weapon::GetUsageFromItemID( TItemID );
	
	if( ui32Usage < 0 )										return FALSE;
	if( ui32Usage >= WEAPON_SLOT_COUNT )					return FALSE;
	
	if( TItemID == m_pWeapon[ ui32Usage ]->GetEquip()->GetWeaponID() )	return TRUE;
	
	if( m_bNaturalRPGGet )									return TRUE;

	return FALSE;
}

//BOOL CCharacter::AddThrowWeapon( UINT16 ui16Idx, GAMEINFO_THROW_WEAPON_NETDATA* pThrow )
//{
//	BOOL bFirstPacket;
//
//	if( FALSE == m_ThrowWeaponMgr.AddWeapon( ui16Idx, pThrow, bFirstPacket ) )	return FALSE;
//
//	if( bFirstPacket )
//	{
//		return DecreaseBullet( 0, pThrow->GetWeapon(), 1 );
//	}
//
//	return TRUE;
//}
REAL32 CCharacter::GetAddSpeed()
{
	REAL32 r32AddSpeed = 0.0f;

	if (GetSlotItem() & SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO)
		r32AddSpeed = 0.92f;
	else
	{
		// 가지고 있는 무기에서 이동속도가 가장 빠른 무기로 계산함.
		// 무기 스위칭이 너무 빨라서 현재 들고있는 무기로 체크가 어렵기 때문.
		WeaponInfo * pWeaponInfo;
		REAL32 r32WeaponMoveSpeed = 0.0f;
		for (INT32 i = 0; i < WEAPON_SLOT_COUNT; ++i)
		{
			pWeaponInfo = Table::GetWeaponInfo(GetEquipWeaponID((UINT8)i));

			if (NULL == pWeaponInfo) continue;
			if (pWeaponInfo->m_r32MoveSpeed > r32WeaponMoveSpeed) r32WeaponMoveSpeed = pWeaponInfo->m_r32MoveSpeed;
		}

		if (r32WeaponMoveSpeed != 0)
			r32AddSpeed += (r32WeaponMoveSpeed / 100);
	}

	return r32AddSpeed;
}