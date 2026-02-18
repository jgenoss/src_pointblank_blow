#include "pch.h"
#include "ValidCheck.h"
#include "Designation/DesigApplyInfo.h"
#include "Designation/DesigUtil.h"
#include "MyRoomInfoContext.h"

NET_CHARA_INFO::NET_CHARA_INFO() : _apSkillSystem_Apply_Info(nullptr), _pDesigApplyInfo(nullptr)
{

}

NET_CHARA_INFO::~NET_CHARA_INFO()
{
	delete [] this->_apSkillSystem_Apply_Info;
	delete this->_pDesigApplyInfo;
}

void NET_CHARA_INFO::SetSkillsystem( const CHARA_SLOT_SKILL& skill, const char * pszFile, INT32 line)
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( ValidCheck::Skill::isPrimary( skill.m_PrimarySkill) == false)
	{
		I3assertForCoverity();
		i3Error::SetEnv( pszFile, line, __FUNCSIG__, I3LOG_FATAL);
		i3Error::Log( "Invalid primary skill %d", skill.m_PrimarySkill);
	}
	if( ValidCheck::Skill::isCommon( skill.m_CommonSkill) == false)
	{
		I3assertForCoverity();
		i3Error::SetEnv( pszFile, line, __FUNCSIG__, I3LOG_FATAL);
		i3Error::Log( "Invalid common skill %d", skill.m_CommonSkill);
	}
	if( ValidCheck::Skill::isMain( skill.m_MainSkill) == false)
	{
		I3assertForCoverity();
		i3Error::SetEnv( pszFile, line, __FUNCSIG__, I3LOG_FATAL);
		i3Error::Log( "Invalid main skill %d", skill.m_MainSkill);
	}
	if( ValidCheck::Skill::isAssist( skill.m_AssistSkill) == false)
	{
		I3assertForCoverity();
		i3Error::SetEnv( pszFile, line, __FUNCSIG__, I3LOG_FATAL);
		i3Error::Log( "Invalid main skill %d", skill.m_MainSkill);
	}
#endif

	if (_apSkillSystem_Apply_Info == nullptr)
	{
		_apSkillSystem_Apply_Info = new SKILLSYSTEM_APPLY_INFO[CATEGORYLIST_MAX];			// 필요할때 생성됨..
		for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
			_apSkillSystem_Apply_Info[i].Reset();
	}

	_apSkillSystem_Apply_Info[CATEGORYLIST_COMMON].setSkillData(0, skill.m_CommonSkill,	skill.m_ui8CommonSkillLv);
	_apSkillSystem_Apply_Info[CATEGORYLIST_MAIN].setSkillData(1, skill.m_MainSkill,		skill.m_ui8MainSkillLv);
	_apSkillSystem_Apply_Info[CATEGORYLIST_ASSIST].setSkillData(2, skill.m_AssistSkill,	skill.m_ui8AssistSkillLv);
}

void NET_CHARA_INFO::setWeaponItemID( WEAPON_SLOT_TYPE iSlot, T_ItemID ItemID, T_ItemDBIdx ItemIdx, const char * pszFile, INT32 line)
{
#if !defined(USE_I3EXEC)
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	I3_BOUNDCHK( iSlot, WEAPON_SLOT_COUNT);

	if( ValidCheck::Item::Weapon::isWeaponItemID( iSlot, ItemID) == false)
	{
		if( pszFile != nullptr)
		{
			i3Error::SetEnv( pszFile, line, __FUNCSIG__, I3LOG_FATAL);
			i3Error::Log( "Invalid Weapon ID %d", ItemID);
		}
		else
		{
			if( !( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) ) )
			I3PRINTLOG(I3LOG_FATAL,  "Invalid Weapon ID %d", ItemID);
		}
	}
#endif
#endif // end of if !defined(USE_I3EXEC)


	_WeaponList[ iSlot].m_TItemID = ItemID;

	if( ItemIdx != -1)
		_WeaponList[ iSlot].m_TItemDBIdx = ItemIdx;
}

void		NET_CHARA_INFO::ResetSkill( INT32 slot)
{
	if (_apSkillSystem_Apply_Info == nullptr) return;
	I3_BOUNDCHK( slot, CATEGORYLIST_MAX);
	_apSkillSystem_Apply_Info[ slot].Reset();
}

const SKILLSYSTEM_APPLY_INFO* NET_CHARA_INFO::getSkillsystem( INT32 iSlot) const
{
	if (_apSkillSystem_Apply_Info == nullptr) return nullptr;
	I3_BOUNDCHK( iSlot , CATEGORYLIST_MAX);
	return &_apSkillSystem_Apply_Info[ iSlot];
}

SKILLSYSTEM_APPLY_INFO *	NET_CHARA_INFO::setSkillsystem( INT32 iSlot)
{
	if (_apSkillSystem_Apply_Info == nullptr)
	{
		_apSkillSystem_Apply_Info = new SKILLSYSTEM_APPLY_INFO[CATEGORYLIST_MAX];			// 필요할때 생성됨..
		for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
			_apSkillSystem_Apply_Info[i].Reset();
	}

	I3_BOUNDCHK( iSlot , CATEGORYLIST_MAX);
	return &_apSkillSystem_Apply_Info[ iSlot];
}

void	NET_CHARA_INFO::ApplyEquipUserTitle(const UINT8 (&ui8EquipUserTitle)[ MAX_EQUIP_USER_TITLE ])
{
	// 이 시점에서 memcmp하고 계산들어가도 좋을듯...같다면 스킵함..
	if ( ::memcmp(ui8EquipUserTitle, _aDesigSetType, sizeof(UINT8)*MAX_EQUIP_USER_TITLE) == 0)
		return;

	::memcpy(_aDesigSetType, ui8EquipUserTitle, sizeof(UINT8)*MAX_EQUIP_USER_TITLE);

	if (_pDesigApplyInfo == nullptr)
		_pDesigApplyInfo = new DesigApplyInfo;

	ResetDesigApplyInfo(_pDesigApplyInfo);

	for (INT32 i = 0 ; i < MAX_EQUIP_USER_TITLE ; ++i)
		DesigUtil::EnableDesigApplyInfo(ui8EquipUserTitle[i], _pDesigApplyInfo);
}

void	NET_CHARA_INFO::ApplyEquipUserTitle(INT32 slotIdx, UINT8 desigSetType)
{
	if (GetDesigSetTypeBySlot(slotIdx) == desigSetType)
		return;
	
	_aDesigSetType[slotIdx] = desigSetType;

	if (_pDesigApplyInfo == nullptr)
		_pDesigApplyInfo = new DesigApplyInfo;	

	ResetDesigApplyInfo(_pDesigApplyInfo);	
	
	for (INT32 i = 0 ; i < MAX_EQUIP_USER_TITLE ; ++i)
		DesigUtil::EnableDesigApplyInfo(_aDesigSetType[i], _pDesigApplyInfo);
	
}

void	NET_CHARA_INFO::DisableEquipUserTitle(INT32 slotIdx)
{
	if (GetDesigSetTypeBySlot(slotIdx) < 1)
		return;

	_aDesigSetType[slotIdx] = 0;

	if (_pDesigApplyInfo == nullptr)
		_pDesigApplyInfo = new DesigApplyInfo;	
	
	ResetDesigApplyInfo(_pDesigApplyInfo);	

	for (INT32 i = 0 ; i < MAX_EQUIP_USER_TITLE ; ++i)
		DesigUtil::EnableDesigApplyInfo(_aDesigSetType[i], _pDesigApplyInfo);
}



void NET_CHARA_INFO::Reset( void)
{
	_CharacterParts.Reset();
	_CommonParts.Reset();
	memset(_WeaponList, 0 , sizeof(ITEM_INFO) * WEAPON_SLOT_COUNT);			// 주석풀고 초기화 복구.(2012.09.28.수빈)


	_Hp = 100;
	_RespawnIdx = 0;
	_MyRespawnCnt = 0;
	_CurrentCharaIdx = 0;
	_RespawnObjIdx = -1;													// -1 이면 rand으로 포지션을 결정.
	i3Vector::Zero( &_RespawnPos);
	
	::memset(_aDesigSetType, 0, sizeof(UINT8)*MAX_EQUIP_USER_TITLE);

	if (_pDesigApplyInfo != nullptr)
		ResetDesigApplyInfo(_pDesigApplyInfo);
	
	if (_apSkillSystem_Apply_Info != nullptr)
	{
		for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
			_apSkillSystem_Apply_Info[i].Reset();
	}

	for( INT32 i = 0 ; i < WEAPON_SLOT_COUNT ; i++ )
		_WeaponEndurance[i] = 100;

	for( INT32 i = 0 ; i < TEAM_COUNT ; i++ )
		_CharacterID[i] = 0;
}



void	NET_CHARA_INFO::CopyTo( NET_CHARA_INFO * pDest) 
{
	_CharacterParts.CopyTo( &pDest->_CharacterParts);
	memcpy(&pDest->_CommonParts, &_CommonParts, sizeof(COMMON_PARTS_INFO));
	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		pDest->_WeaponList[i]		= _WeaponList[i];
		pDest->_WeaponEndurance[i]	= _WeaponEndurance[i];
	}	

	pDest->_Hp = _Hp;
	pDest->_RespawnIdx = _RespawnIdx;
	pDest->_RespawnObjIdx = _RespawnObjIdx;
	pDest->_MyRespawnCnt = _MyRespawnCnt;
	i3Vector::Copy( &pDest->_RespawnPos, &_RespawnPos);

	::memcpy(pDest->_aDesigSetType, this->_aDesigSetType, sizeof(UINT8)*MAX_EQUIP_USER_TITLE);

	if (this->_pDesigApplyInfo)
	{
		if (pDest->_pDesigApplyInfo == nullptr)
			pDest->_pDesigApplyInfo = new DesigApplyInfo;

		*pDest->_pDesigApplyInfo = *this->_pDesigApplyInfo;
	}
	else
	{
		if (pDest->_pDesigApplyInfo != nullptr)
		{
			delete pDest->_pDesigApplyInfo;		pDest->_pDesigApplyInfo = nullptr;
		}
	}

	if (this->_apSkillSystem_Apply_Info != nullptr)
	{
		if (pDest->_apSkillSystem_Apply_Info == nullptr)
			pDest->_apSkillSystem_Apply_Info = new SKILLSYSTEM_APPLY_INFO[CATEGORYLIST_MAX];
		
		for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
			pDest->_apSkillSystem_Apply_Info[i] = this->_apSkillSystem_Apply_Info[i];
	}
	else
	{
		if (pDest->_apSkillSystem_Apply_Info != nullptr)
		{
			delete [] pDest->_apSkillSystem_Apply_Info;	pDest->_apSkillSystem_Apply_Info = nullptr;
		}
	}
}

void	NET_CHARA_INFO::CopyTo( UI_CHARA_INFO * pDest, INT team ) 
{	
	if( team == TEAM_BOTH )
	{
		for(int i = 0 ; i < TEAM_COUNT ; i++)
			_CharacterParts.CopyTo( const_cast<CHARA_PARTS_INFO*>(pDest->getCharaParts(i)) );
	}
	else
		_CharacterParts.CopyTo( const_cast<CHARA_PARTS_INFO*>(pDest->getCharaParts(team)) );
	
	for( INT32 i = 0; i < WEAPON_SLOT_COUNT; i++)
	{
		*(const_cast<ITEM_INFO*>(pDest->getWeaponList(i)))	= _WeaponList[i];
	}

	memcpy( const_cast<COMMON_PARTS_INFO*>(pDest->getCommonParts()), &_CommonParts, sizeof(COMMON_PARTS_INFO));
}

REAL32 NET_CHARA_INFO::getWeaponSkillValue( WEAPON_CLASS_TYPE weapontype, SKILL_TYPE skilltype) const
{
	if (_apSkillSystem_Apply_Info == nullptr)
		return 0.f;

	REAL32 value = 0.f;
	for(INT32 i = 0; i < CATEGORYLIST_MAX; i++)
	{
		value += _apSkillSystem_Apply_Info[i].getSkillValue(weapontype, skilltype);
	}
	return value;
}

REAL32 NET_CHARA_INFO::getCharaSkillValue( SKILL_TYPE skilltype) const
{
	if (_apSkillSystem_Apply_Info == nullptr)
		return 0.f;

	REAL32 value = 0.f;
	for(INT32 i = 0; i < CATEGORYLIST_MAX; i++)
	{
		value += _apSkillSystem_Apply_Info[i].getSkillValue(skilltype);
	}
	return value;
}