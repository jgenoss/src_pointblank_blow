#include "pch.h"
#include "GameCharaDef.h"
#include "GlobalVariables.h"
#include "Weapon/WeaponItemID.h"

//#define RANK_COUNT 52



i3::rc_wstring GetCharaRank(INT32 rank)
{
	if( rank == -1 )
	{	
		// GM, NEWBIE, MOD를 제외한 랭크 표시를 위한 수정
		// rank = RANK_MAX - 1;
		// Rank가 왜 하드코딩이...
		rank = RANK_MAX - (RANK_MAX - RANK_51);
	}

	// 용병마크의 UI인덱스는 RANK_MAX를 넘어가므로 주석처리합니다. gyungju.kim
	//I3_BOUNDCHK( rank, RANK_MAX);

	i3::string strTemp;



	if( rank == RANK_GAMEMASTER )
		strTemp = "STR_TBL_CHAT_GM_TEXT";
	else
	{
		if(g_pEnvSet->IsBaseOnUIVersion_1_0() || g_pEnvSet->m_nUIVersion == UFV_1_1)
			i3::sprintf(strTemp, "STBL_IDX_V1_RANK_%d", rank);
		else
			i3::sprintf(strTemp, "STBL_IDX_RANK_%d", rank);
	}

	// 주어진 랭크보다 더 높을 경우, 이는 용병 마크입니다.
	if( rank > RANK_MAX)
		strTemp = "STR_MERC_TITLE";


	return GAME_RCSTRING(strTemp);
}

bool	GLOBAL_CHARA_INFO::GetAIToClient() const
{
	return _bAIToClient;
	/*CHARA::TYPE eType = GetType();

	return (eType == CHARA::TYPE_AI) || (eType == CHARA::TYPE_AI_FORCLIENT);*/
}

CHARA::TYPE GLOBAL_CHARA_INFO::GetType() const
{
	return _Type;
}

void	GLOBAL_CHARA_INFO::SetType(CHARA::TYPE val)
{
	_Type = val;
}

CHARACTER_TEAM_TYPE	GLOBAL_CHARA_INFO::GetTeam() const
{
	return _Team;
}

void	GLOBAL_CHARA_INFO::SetTeam( CHARACTER_TEAM_TYPE val)
{
	_Team = val;
}


//INT32	GLOBAL_CHARA_INFO::GetCharaID() const
//{
//#if defined( MEM_ENCRYPTION_LV1)
//	return GlobalFunc::DecryptValue( (INT32*) &_CharaID, ENC_SHIFT_CHARA_INFO);
//#else
//	return _CharaID;
//#endif
//}
//
//void	GLOBAL_CHARA_INFO::SetCharaID(INT32 val)
//{
//	_CharaID = val;
//
//#if defined( MEM_ENCRYPTION_LV1)
//	GlobalFunc::EncryptValue( (INT32*) &_CharaID, ENC_SHIFT_CHARA_INFO);
//#endif
//}

CHARA_RES_ID	GLOBAL_CHARA_INFO::GetResType() const
{
	return _ResType;
}

void	GLOBAL_CHARA_INFO::SetResType(CHARA_RES_ID val)
{
	_ResType = val;
}

REAL32	GLOBAL_CHARA_INFO::GetHP() const
{
	return _rHp;
}

void	GLOBAL_CHARA_INFO::SetHP(REAL32 val)
{
	_rHp = val;
}

void	GLOBAL_CHARA_INFO::DecreaseHP(REAL32 val)
{
	PB_VM_S

	_rHp = GetHP();

	_rHp = _rHp - val;

	PB_VM_E
}

INT32	GLOBAL_CHARA_INFO::GetNetIdx() const
{
	return _NetIdx;
}

void	GLOBAL_CHARA_INFO::SetNetIdx(INT32 val)
{
	_NetIdx = val;
}

INT32 GLOBAL_CHARA_INFO::GetRespawnCnt( void) const
{
	return _RespawnCnt;
}

void GLOBAL_CHARA_INFO::SetRespawnCnt( INT32 cnt)
{
	_RespawnCnt = cnt;
}

UINT32 CHARA_DAMAGEINFO::GetRealWeaponNumber()
{
	if ( _pWeapon && _pWeapon->getWeaponInfo() )
	{
		return _pWeapon->getWeaponInfo()->GetRealNumber();
	}

	return GetFireWeaponNumber();
}

void CHARA_DAMAGEINFO::SetDeathTypeByHitPart( CHARA_HIT_PART hitPart, CHARA_DEATH_TYPE deathType)
{
	//	헤드샷 죽음
	if( hitPart == CHARA_HIT_HEAD)
	{
		_DeathType = CHARA_DEATH_H;
	}
	else if( hitPart == CHARA_HIT_DEATH_TRAMPLE)
	{
		_DeathType = CHARA_DEATH_TRAMPLED;
		SetHitPart(CHARA_HIT_ROOT);
	}
	else if( hitPart == CHARA_HIT_DEATH_HOWL)
	{
		_DeathType = CHARA_DEATH_HOWL;	
		SetHitPart(CHARA_HIT_ROOT);
	}
	else
	{
		//	hit 패킷으로 넘겨 받은 데쓰 타입으로 설정한다.
		_DeathType = deathType;
	}
}

bool CHARA_DAMAGEINFO::IsCalculateOwned( bool bDedicatedMode)
{
	bool bCheck =  ( isFireWeaponType(WEAPON_CLASS_THROWING_GRENADE) 
		|| isFireWeaponType(WEAPON_CLASS_THROWING_CONTAIN) || isFireWeaponType(WEAPON_CLASS_THROWING_ATTACK) 
		|| isFireWeaponType(WEAPON_CLASS_THROWING_HEAL) || isFireWeaponType(WEAPON_CLASS_GRENADESHELL)
		|| ( isFireWeaponType(WEAPON_CLASS_DINO) && isFireWeaponNumber(WEAPON::DINO_TANK_BUTT, true) ) 
		|| isFireThrowingKnife() ||	_DeathType == CHARA_DEATH_O );

	if( bDedicatedMode == true)
	{
		// 폭탄처리는 Client가 한다.
		// 오브젝트 폭발 킬은 Dedi server 작업이 필요하여 일단 자살 패킷으로 처리..
		// 차후에 Hit 패킷으로 처리하는 방식으로 변경해야됨...
		return false;
	}

	return bCheck;
}

bool CHARA_DAMAGEINFO::isFireThrowingKnife( void)
{
	if ( _pWeapon != nullptr )
	{
		return _pWeapon->getWeaponInfo()->IsThrowWeapon();
	}

	if (_pWeaponInfo != nullptr)
	{
		return _pWeaponInfo->IsThrowWeapon();
	}

	return false;;
}

void CHARA_DAMAGEINFO::SetWeaponItemID(WeaponBase * pWeapon)
{
	SetWeaponItemID( pWeapon->getWeaponInfo(), pWeapon->getBaseWeaponInfo() );
}

void CHARA_DAMAGEINFO::SetWeaponItemID(CWeaponInfo * pCurWeaponInfo, CWeaponInfo * pBaseWeaponInfo )
{
	_ui32FireWeaponID	= pCurWeaponInfo->GetItemID();
	_ui32BaseWeaponID	= pBaseWeaponInfo->GetItemID();

	/*if(pBaseWeaponInfo != pCurWeaponInfo)
	{
		INT32 i32ExtState = pBaseWeaponInfo->GetCompatibleWeaponIdx( pCurWeaponInfo->GetItemID() ) + 1;
		_ui32BaseWeaponID = SET_ITEM_EXT_STATE(i32ExtState, _ui32BaseWeaponID );
	}*/
}

bool CHARA_DAMAGEINFO::isFireWeaponNumber( INT32 i32Idx, bool isWit/* = false*/)
{
	INT32 i32Number = i32Idx;

	if( isWit )
		i32Number = WEAPON::getItemIndex( (WEAPON::eINDEX)i32Idx);

	return GetFireWeaponNumber() == i32Number;
}

bool CHARA_DAMAGEINFO::isRealWeaponNumber( INT32 i32Idx)
{
	INT32 i32Number = WEAPON::getItemIndex( (WEAPON::eINDEX)i32Idx);
	return (INT32)GetRealWeaponNumber() == i32Number; 
}

bool CHARA_DAMAGEINFO::isValid( void)
{
	if( _DeathType == CHARA_DEATH_UN || _DeathType >= CHARA_DEATH_MAXCOUNT)
		return false;

	if( _DamageType == WEAPON::DAMAGE_UNKNOWN || _DamageType >= WEAPON::DAMAGE_COUNT)
		return false;

	if( !_IsNPC )
	{
		WEAPON_CLASS_TYPE FireWeaponClass = GetFireWeaponClass();
		WEAPON_SLOT_TYPE FireWeaponSlot = GetFireWeaponSlot();
		INT32 FireWeaponNumber = GetFireWeaponNumber();

		if( FireWeaponClass == WEAPON_CLASS_UNKNOWN || FireWeaponClass >= WEAPON_CLASS_COUNT)
			return false;

		if( _nKillerIdx == -1 || _nKillerIdx >= MAX_COUNT_DOMI_DINO)
			return false;

		if( FireWeaponSlot == WEAPON_SLOT_UNKNOWN || FireWeaponSlot >= WEAPON_SLOT_COUNT)
			return false;

		if( FireWeaponNumber == -1 )
			return false;
	}

	if( _rDamage < 0.f && !(isFireWeaponType(WEAPON_CLASS_THROWING_GRENADE) && isFireWeaponNumber( WEAPON::RPG7_BOSS)) )
		return false;

	if( GetHitPart() == CHARA_HIT_UNKNOWN || GetHitPart() >= CHARA_HIT_MAXCOUNT)
		return false;

	return true;
}