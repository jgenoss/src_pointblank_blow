#include "pch.h"
#include "SkillManager.h"
#include "GlobalVariables.h"
#include "../../CommonSource/ItemID/ItemTypeWeaponClass.h"
#include "../../CommonSource/CommonDef_Skill.h"

#if defined(SKILLSYSTEM)

I3_CLASS_INSTANCE(SkillManager);//, i3ElementBase);

char _strType[MAX_SKILL_TYPE][MAX_STRING_COUNT] = {
	"Skill/Primary",
	"Skill/Secondary",
	"Skill/Mode"
};

char _strClass[SKILL_TYPE_MAX][MAX_STRING_COUNT] = {
	"Assault",
	"SMG",
	"Sniper",
	"Machine",
	"Shotgun",
	"Handgun",
	"Knife",
	"Grenade",
	"Dual",
	"CCMode",
	"Domination",
	"G2",
};

INT32	g_MaxWarClassImage[SKILL_TYPE_MAX] = 
{
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
	11,
};

SkillManager::SkillManager(void)
{
	m_mySkill[0] = 0;
	m_mySkill[1] = 0;
	m_mySkill[2] = 0;

	i3mem::FillZero( &m_ClassSet, sizeof( m_ClassSet));
	i3mem::FillZero( &m_SkillString, sizeof( m_SkillString));
	i3mem::FillZero( &m_SlotSet, sizeof( m_SlotSet));

	for(INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		m_bFirstSwap[i] =	false;
		m_bFirstReload[i] =	false;
	}
}

SkillManager::~SkillManager(void)
{

}

void SkillManager::setSkillText( INT32 id, const char * pszStr)
{
	I3_BOUNDCHK( id, SKILL_LIST_MAX);

	INT32 len = i3::generic_string_size( pszStr);

	m_SkillString[id]._Type = 0;

	if( len > 0 && len < MAX_STRING_SKILL_COUNT)
		i3::string_ncopy_nullpad( m_SkillString[ id]._Text, pszStr, len);
	else
	{
		I3WARN("Skill string over index");
		m_SkillString[id]._Text[0] = '\0';
	}
}

UINT8 SkillManager::getCurrentClass(INT32 _type)
{
	UINT8 _ClassIdx = m_mySkillInfo[0][_type].m_ui8Class;
	return getSkillIndex(_type, _ClassIdx);
}

UINT8 SkillManager::getCurrentClass(INT32 _Chara, INT32 _type)
{
	UINT8 _ClassIdx = m_mySkillInfo[_Chara][_type].m_ui8Class;
	return getSkillIndex(_type, _ClassIdx);
}

UINT8 SkillManager::getCurrentClass(CHARA_SKILL *pSkillInfo, eSKILL_TYPE _type)
{
	UINT8 _ClassIdx = pSkillInfo->m_ui8Class;
	return getSkillIndex(_type, _ClassIdx);
}

UINT8 SkillManager::getSkillIndex(INT32 _type, UINT8 _ClassIdx)
{
	if(_ClassIdx == 255)
		return 255;
	switch(_type)
	{
	case SKILL_TYPE_PRIMARY:
		_ClassIdx += 0;
		break;
	case SKILL_TYPE_SECONDARY:
		_ClassIdx += (MAX_SKILL_CLASS_PRIMARY);
		break;
	case SKILL_TYPE_MODE:
		_ClassIdx += (MAX_SKILL_CLASS_PRIMARY + MAX_SKILL_CLASS_SECONDARY);
		break;
	}
	I3ASSERT(_ClassIdx < SKILL_TYPE_MAX);
	return _ClassIdx;
}

const UINT8 SkillManager::getCurrentClassForPacket(INT32 _Chara, INT32 _type)
{
	return m_mySkillInfo[_Chara][_type].m_ui8Class;
}

INT32 SkillManager::getSkillLastImageIndex(INT32 _charaIdx, INT32 _type)
{
	UINT8 _Class = getCurrentClass(_charaIdx, _type);
	if(_Class == 255)
		return -1;
	INT32 _StartIdx = getImageStartIndexByClass((eSKILL_CLASS)getCurrentClass(_charaIdx,_type));
	INT32 _LastIdx = GetLastSlot(_charaIdx, _type);
	//INT32 _LastIdx = GetLastSlot(_Class, getSkillValue(_chara, _type));
	if( _LastIdx == -1)
		return -1;
	else
		return _LastIdx + _StartIdx;
}

INT32 SkillManager::getSkillLastImageIndex(CHARA_SKILL* _Chara, INT32 _type)
{
	UINT8 _ClassIdx = _Chara->m_ui8Class;
	UINT8 _Class = getSkillIndex(_type, _ClassIdx);

	if(_Class == 255)
		return -1;
	INT32 _StartIdx = getImageStartIndexByClass((eSKILL_CLASS)_Class);
	INT32 _LastIdx = GetLastSlot(_Chara, _type);

	if( _LastIdx == -1)
		return -1;
	else
		return _LastIdx + _StartIdx;
}

UINT32 SkillManager::getSkillPlayTime(INT32 _chara, INT32 _type)
{
	return m_mySkillInfo[_chara][_type].m_ui32PlayTime;
}

UINT16 SkillManager::getSkillPlayPoint(INT32 _chara, INT32 _type)
{
	return m_mySkillInfo[_chara][_type].m_ui16PlayTimePoint;
}

BOOL SkillManager::HasType(INT32 idx, INT32 type)
{
	UINT8 maxClass = getMaxClassByType(type);
	if(0 <= m_mySkillInfo[idx][type].m_ui8Class && m_mySkillInfo[idx][type].m_ui8Class < maxClass)
		return TRUE;
	return FALSE;
}

UINT8 SkillManager::getMaxClassByType(INT32 _type)
{
	switch(_type)
	{
	case SKILL_TYPE_PRIMARY:	return MAX_SKILL_CLASS_PRIMARY;
	case SKILL_TYPE_SECONDARY:	return MAX_SKILL_CLASS_SECONDARY;
	case SKILL_TYPE_MODE:		return MAX_SKILL_CLASS_MODE;
	default:					return 0;
	}
}

INT32 SkillManager::GetSlotState(INT32 avatar, INT32 type, INT32 slot)
{
	if( HasType(avatar,type) == FALSE)
		return SKILL_STATE_WRONG;
	UINT64 _Skill = getSkillValue(avatar, type);
	UINT8 v1 = (UINT8)GETBIT64(_Skill, slot * 2);
	UINT8 v2 = (UINT8)GETBIT64(_Skill, slot * 2 + 1);
	if ( v1 == 0 && v2 == 0)
		return SKILL_STATE_NONE;
	if ( v1 == 1 && v2 == 0)
		return SKILL_STATE_LEFT;
	if ( v1 == 0 && v2 == 1)
		return SKILL_STATE_RIGHT;
	else
		return SKILL_STATE_WRONG;
}

INT32 SkillManager::GetSlotState(CHARA_SKILL* chara, INT32 slot)
{
	UINT8 v1 = (UINT8)GETBIT64(chara->m_ui64SkillTree, slot * 2);
	UINT8 v2 = (UINT8)GETBIT64(chara->m_ui64SkillTree, slot * 2 + 1);
	if ( v1 == 0 && v2 == 0)
		return SKILL_STATE_NONE;
	if ( v1 == 0 && v2 == 1)
		return SKILL_STATE_RIGHT;
	if ( v1 == 1 && v2 == 0)
		return SKILL_STATE_LEFT;
	else
		return SKILL_STATE_WRONG;
}

INT32 SkillManager::GetLastSlot(INT32 chara, INT32 type)
{
	UINT64 _Skill = getSkillValue(chara, type);
	UINT8 _Class = getCurrentClass(chara, type);
	INT32 _skillCount = getMaxSkillCountByClass(_Class);
	INT32 slot = 0;

	for(slot = 0; slot < _skillCount; slot++)
	{
		UINT8 v1 = (UINT8)GETBIT64(_Skill, slot * 2);
		UINT8 v2 = (UINT8)GETBIT64(_Skill, slot * 2 + 1);

		if( v1 == 0 && v2 == 0)
		{
			return slot - 1;
		}
	}
	if(slot == _skillCount)
		return _skillCount-1;
	return -1;
}

INT32 SkillManager::GetLastSlot(CHARA_SKILL * pChara, INT32 type)
{
	UINT64 _Skill = pChara->m_ui64SkillTree;
	UINT8 _Class = pChara->m_ui8Class;
	INT32 _skillCount = getMaxSkillCountByClass(_Class);
	INT32 slot = 0;

	for(slot = 0; slot < _skillCount; slot++)
	{
		UINT8 v1 = (UINT8)GETBIT64(_Skill, slot * 2);
		UINT8 v2 = (UINT8)GETBIT64(_Skill, slot * 2 + 1);

		if( v1 == 0 && v2 == 0)
		{
			return slot - 1;
		}
	}
	if(slot == _skillCount)
		return _skillCount-1;
	return -1;
}

void SkillManager::setSkillTree(INT32 _avatar,INT32 _tab,INT32 _line,INT32 _dir,UINT64* data)
{
	*data = getSkillValue(_avatar, _tab);
	if(_dir == 1)							// 왼쪽 슬롯 01세팅
	{
		SETBIT64(*data, _line * 2);
		CLRBIT64(*data, _line * 2 + 1);
	}
	else if(_dir == 2)						// 오른쪽 슬롯 10세팅
	{
		SETBIT64(*data, _line * 2 + 1);
		CLRBIT64(*data, _line * 2);
	}
	else	// 오류 처리(강제로 잘못된값 전송)
	{
		SETBIT64(*data, _line * 2);
		SETBIT64(*data, _line * 2 + 1);
	}
	I3TRACE("SkillValue : %d\n", *data);
}

UINT64 SkillManager::getSkillValue(INT32 _chara, INT32 _type)
{
	return m_mySkillInfo[_chara][_type].m_ui64SkillTree;
}

UINT64 SkillManager::getMySkillValue(INT32 avatar, INT32 type)
{
	return m_mySkillInfo[avatar][type].m_ui64SkillTree;
}

bool SkillManager::BuildSkillData(INT32 slot, CHARA_SKILL* skillInfo, INT32 type)
{
	skillInfo->CopyTo(&m_mySkillInfo[slot][type]);
	return true;
}

void SkillManager::DeleteSkillData(INT32 idx)
{
	//INT32 i = 0;

	m_mySkillInfo[idx][0].Reset();
	m_mySkillInfo[idx][1].Reset();
	m_mySkillInfo[idx][2].Reset();

	//for(i=idx+1; i<MAX_MULTI_SLOT; i++)
	//{
	//	m_mySkillInfo[i][0].CopyTo(&m_mySkillInfo[i-1][0]);
	//	m_mySkillInfo[i][1].CopyTo(&m_mySkillInfo[i-1][1]);
	//	m_mySkillInfo[i][2].CopyTo(&m_mySkillInfo[i-1][2]);
	//}

	//m_mySkillInfo[i-1][0].Reset();
	//m_mySkillInfo[i-1][1].Reset();
	//m_mySkillInfo[i-1][2].Reset();
}

void SkillManager::ResetSkillData()
{
	for(INT32 i=0; i < MAX_MULTI_SLOT; i++)
	{
		for(INT32 j=0; j < MAX_SKILL_TYPE; j++)
			m_mySkillInfo[i][j].Reset();
	}
}

const INT32 SkillManager::getImageStartIndexByClass(eSKILL_CLASS warClass)
{
	INT32 StartIndex = 0;
	for(UINT8 i=0; i < warClass; i++)
		StartIndex += g_MaxWarClassImage[i];
	return StartIndex;
}

const INT32 SkillManager::getSkillCostByClass(INT32 warClass, INT32 line)
{
	INT32 _SkillType = SKILL_LIST_NONE;
	switch(warClass)
	{
	case SKILL_ASSAULT :	_SkillType = g_aui32SkillTable_Assault[line][SKILL_DATA_PER_LINE_POINT];		break;		// 돌격소총
	case SKILL_SMG :		_SkillType = g_aui32SkillTable_SMG[line][SKILL_DATA_PER_LINE_POINT];			break;		// 기관단총
	case SKILL_SNIPER :		_SkillType = g_aui32SkillTable_Sniper[line][SKILL_DATA_PER_LINE_POINT];			break;		// 저격총
	case SKILL_SHOTGUN :	_SkillType = g_aui32SkillTable_Shotgun[line][SKILL_DATA_PER_LINE_POINT];		break;		// 산탄총
	case SKILL_MACHINE :	_SkillType = g_aui32SkillTable_Machine[line][SKILL_DATA_PER_LINE_POINT];		break;		// 기관총
	case SKILL_HANDGUN :	_SkillType = g_aui32SkillTable_Handgun[line][SKILL_DATA_PER_LINE_POINT];		break;		// 권총
	case SKILL_KNIFE :		_SkillType = g_aui32SkillTable_Knife[line][SKILL_DATA_PER_LINE_POINT];			break;		// 단검
	case SKILL_GRENADE :	_SkillType = g_aui32SkillTable_Grenade[line][SKILL_DATA_PER_LINE_POINT];		break;		// 수류탄
	case SKILL_DUAL :		_SkillType = g_aui32SkillTable_Dual[line][SKILL_DATA_PER_LINE_POINT];			break;		// 듀얼
	case SKILL_CCMODE :		_SkillType = g_aui32SkillTable_CCMode[line][SKILL_DATA_PER_LINE_POINT];			break;		// CC 모드
	case SKILL_DOMINATION :	_SkillType = g_aui32SkillTable_Domination[line][SKILL_DATA_PER_LINE_POINT];		break;		// 제압 모드
	case SKILL_G2 :			_SkillType = g_aui32SkillTable_G2[line][SKILL_DATA_PER_LINE_POINT];				break;		// G2 모드
	}
	return _SkillType;
}

const char * SkillManager::getSkillTextByClass(INT32 warClass, SLOT_DIRECTION dir, INT32 line)
{
	INT32 _SkillType = SKILL_LIST_NONE;
	switch(warClass)
	{
	case SKILL_ASSAULT :	_SkillType = g_aui32SkillTable_Assault[line][dir];		break;		// 돌격소총
	case SKILL_SMG :		_SkillType = g_aui32SkillTable_SMG[line][dir];			break;		// 기관단총
	case SKILL_SNIPER :		_SkillType = g_aui32SkillTable_Sniper[line][dir];		break;		// 저격총
	case SKILL_SHOTGUN :	_SkillType = g_aui32SkillTable_Shotgun[line][dir];		break;		// 산탄총
	case SKILL_MACHINE :	_SkillType = g_aui32SkillTable_Machine[line][dir];		break;		// 기관총
	case SKILL_HANDGUN :	_SkillType = g_aui32SkillTable_Handgun[line][dir];		break;		// 권총
	case SKILL_KNIFE :		_SkillType = g_aui32SkillTable_Knife[line][dir];		break;		// 단검
	case SKILL_GRENADE :	_SkillType = g_aui32SkillTable_Grenade[line][dir];		break;		// 수류탄
	case SKILL_DUAL :		_SkillType = g_aui32SkillTable_Dual[line][dir];			break;		// 듀얼
	case SKILL_CCMODE :		_SkillType = g_aui32SkillTable_CCMode[line][dir];		break;		// CC 모드
	case SKILL_DOMINATION :	_SkillType = g_aui32SkillTable_Domination[line][dir];	break;		// 제압 모드
	case SKILL_G2 :			_SkillType = g_aui32SkillTable_G2[line][dir];			break;		// G2 모드
	}
	return getSkillName(_SkillType);
}

const INT32 SkillManager::getSkillByClass(INT32 warClass, SLOT_DIRECTION dir, INT32 line)
{
	INT32 _SkillType = SKILL_LIST_NONE;
	switch(warClass)
	{
	case SKILL_ASSAULT :	_SkillType = g_aui32SkillTable_Assault[line][dir];		break;		// 돌격소총
	case SKILL_SMG :		_SkillType = g_aui32SkillTable_SMG[line][dir];			break;		// 기관단총
	case SKILL_SNIPER :		_SkillType = g_aui32SkillTable_Sniper[line][dir];		break;		// 저격총
	case SKILL_SHOTGUN :	_SkillType = g_aui32SkillTable_Shotgun[line][dir];		break;		// 산탄총
	case SKILL_MACHINE :	_SkillType = g_aui32SkillTable_Machine[line][dir];		break;		// 기관총
	case SKILL_HANDGUN :	_SkillType = g_aui32SkillTable_Handgun[line][dir];		break;		// 권총
	case SKILL_KNIFE :		_SkillType = g_aui32SkillTable_Knife[line][dir];		break;		// 단검
	case SKILL_GRENADE :	_SkillType = g_aui32SkillTable_Grenade[line][dir];		break;		// 수류탄
	case SKILL_DUAL :		_SkillType = g_aui32SkillTable_Dual[line][dir];			break;		// 듀얼
	case SKILL_CCMODE :		_SkillType = g_aui32SkillTable_CCMode[line][dir];		break;		// CC 모드
	case SKILL_DOMINATION :	_SkillType = g_aui32SkillTable_Domination[line][dir];	break;		// 제압 모드
	case SKILL_G2 :			_SkillType = g_aui32SkillTable_G2[line][dir];			break;		// G2 모드
	}
	return _SkillType;
}

const UINT32 SkillManager::getMaxSkillCountByClass(UINT8 _Count)
{
	UINT32 _SkillCount = 0;
	switch(_Count)
	{
	case SKILL_ASSAULT:		_SkillCount = MAX_SKILL_CLASS_ASSAULT_LINE_COUNT;		break;
	case SKILL_SMG:			_SkillCount = MAX_SKILL_CLASS_SMG_LINE_COUNT;			break;
	case SKILL_SNIPER:		_SkillCount = MAX_SKILL_CLASS_SNIPER_LINE_COUNT;		break;
	case SKILL_SHOTGUN:		_SkillCount = MAX_SKILL_CLASS_SHOTGUN_LINE_COUNT;		break;
	case SKILL_MACHINE:		_SkillCount = MAX_SKILL_CLASS_MACHINE_LINE_COUNT;		break;
	case SKILL_HANDGUN:		_SkillCount = MAX_SKILL_CLASS_HANDGUN_LINE_COUNT;		break;
	case SKILL_KNIFE:		_SkillCount = MAX_SKILL_CLASS_KNIFE_LINE_COUNT;			break;
	case SKILL_GRENADE:		_SkillCount = MAX_SKILL_CLASS_GRENADE_LINE_COUNT;		break;
	case SKILL_DUAL:		_SkillCount = MAX_SKILL_CLASS_DUAL_LINE_COUNT;			break;
	case SKILL_CCMODE:		_SkillCount = MAX_SKILL_CLASS_CCMODE_LINE_COUNT;		break;
	case SKILL_DOMINATION:	_SkillCount = MAX_SKILL_CLASS_DOMINATION_LINE_COUNT;	break;
	case SKILL_G2:			_SkillCount = MAX_SKILL_CLASS_G2_LINE_COUNT;			break;
	}

	return _SkillCount;
}

const char * SkillManager::getSkillName(INT32 _Skill)
{
	switch(_Skill)
	{
	case SKILL_LIST_NONE :								return GAME_STRING("STR_SKILL_TEXT_TYPE_NONE");

	case SKILL_LIST_UP_RAN_LV1 :						return GAME_STRING("STR_SKILL_TEXT_TYPE1");
	case SKILL_LIST_DW_WEP_CNG_LV1 :					return GAME_STRING("STR_SKILL_TEXT_TYPE2");
	case SKILL_LIST_DW_WEP_REL_LV1 :					return GAME_STRING("STR_SKILL_TEXT_TYPE3");
	case SKILL_LIST_UP_RPM_LV1 :						return GAME_STRING("STR_SKILL_TEXT_TYPE4");
	case SKILL_LIST_UP_ACC_LV1 :						return GAME_STRING("STR_SKILL_TEXT_TYPE5");
	case SKILL_LIST_UP_PEN_LV1 :						return GAME_STRING("STR_SKILL_TEXT_TYPE6");
	case SKILL_LIST_UP_SPD_LV1 :						return GAME_STRING("STR_SKILL_TEXT_TYPE7");
	case SKILL_LIST_UP_ATT_LV1 :						return GAME_STRING("STR_SKILL_TEXT_TYPE8");
	case SKILL_LIST_DW_BOMB_TIME_LV1 :					return GAME_STRING("STR_SKILL_TEXT_TYPE9");
	case SKILL_LIST_DW_DRAW_VOL_LV1 :					return GAME_STRING("STR_SKILL_TEXT_TYPE10");
	case SKILL_LIST_UP_BOMB_RAN_LV1 :					return GAME_STRING("STR_SKILL_TEXT_TYPE11");

	case SKILL_LIST_UP_RAN_LV2 :						return GAME_STRING("STR_SKILL_TEXT_TYPE1");
	case SKILL_LIST_DW_WEP_CNG_LV2 :					return GAME_STRING("STR_SKILL_TEXT_TYPE2");
	case SKILL_LIST_DW_WEP_REL_LV2 :					return GAME_STRING("STR_SKILL_TEXT_TYPE3");
	case SKILL_LIST_UP_RPM_LV2 :						return GAME_STRING("STR_SKILL_TEXT_TYPE4");
	case SKILL_LIST_UP_ACC_LV2 :						return GAME_STRING("STR_SKILL_TEXT_TYPE5");
	case SKILL_LIST_UP_PEN_LV2 :						return GAME_STRING("STR_SKILL_TEXT_TYPE6");
	case SKILL_LIST_UP_SPD_LV2 :						return GAME_STRING("STR_SKILL_TEXT_TYPE7");
	case SKILL_LIST_UP_ATT_LV2 :						return GAME_STRING("STR_SKILL_TEXT_TYPE8");
	case SKILL_LIST_DW_BOMB_TIME_LV2 :					return GAME_STRING("STR_SKILL_TEXT_TYPE9");
	case SKILL_LIST_DW_DRAW_VOL_LV2	:					return GAME_STRING("STR_SKILL_TEXT_TYPE10");
	case SKILL_LIST_UP_BOMB_RAN_LV2	:					return GAME_STRING("STR_SKILL_TEXT_TYPE11");
	
	}
	return "";
}

const char * SkillManager::getSkillSubName(INT32 _Skill)
{
	switch(_Skill)
	{
	case SKILL_LIST_NONE :								return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE_NONE");

	case SKILL_LIST_UP_RAN_LV1 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE1");
	case SKILL_LIST_DW_WEP_CNG_LV1 :					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE2");
	case SKILL_LIST_DW_WEP_REL_LV1 :					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE3");
	case SKILL_LIST_UP_RPM_LV1 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE4");
	case SKILL_LIST_UP_ACC_LV1 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE5");
	case SKILL_LIST_UP_PEN_LV1 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE6");
	case SKILL_LIST_UP_SPD_LV1 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE7");
	case SKILL_LIST_UP_ATT_LV1 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE8");
	case SKILL_LIST_DW_BOMB_TIME_LV1 :					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE9");
	case SKILL_LIST_DW_DRAW_VOL_LV1 :					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE10");
	case SKILL_LIST_UP_BOMB_RAN_LV1 :					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE11");

	case SKILL_LIST_UP_RAN_LV2 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE1");
	case SKILL_LIST_DW_WEP_CNG_LV2 :					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE2");
	case SKILL_LIST_DW_WEP_REL_LV2 :					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE3");
	case SKILL_LIST_UP_RPM_LV2 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE4");
	case SKILL_LIST_UP_ACC_LV2 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE5");
	case SKILL_LIST_UP_PEN_LV2 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE6");
	case SKILL_LIST_UP_SPD_LV2 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE7");
	case SKILL_LIST_UP_ATT_LV2 :						return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE8");
	case SKILL_LIST_DW_BOMB_TIME_LV2 :					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE9");
	case SKILL_LIST_DW_DRAW_VOL_LV2	:					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE10");
	case SKILL_LIST_UP_BOMB_RAN_LV2	:					return GAME_STRING("STR_SKILL_TEXT_SUB_TYPE11");
	}
	return "";
}

const char * SkillManager::getSkillComment(INT32 _Skill)
{
	switch(_Skill)
	{
	case SKILL_LIST_NONE :								return GAME_STRING("STR_SKILL_TEXT_COM_TYPE_NONE");

	case SKILL_LIST_UP_RAN_LV1 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE1");
	case SKILL_LIST_DW_WEP_CNG_LV1 :					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE2");
	case SKILL_LIST_DW_WEP_REL_LV1 :					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE3");
	case SKILL_LIST_UP_RPM_LV1 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE4");
	case SKILL_LIST_UP_ACC_LV1 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE5");
	case SKILL_LIST_UP_PEN_LV1 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE6");
	case SKILL_LIST_UP_SPD_LV1 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE7");
	case SKILL_LIST_UP_ATT_LV1 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE8");
	case SKILL_LIST_DW_BOMB_TIME_LV1 :					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE9");
	case SKILL_LIST_DW_DRAW_VOL_LV1 :					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE10");
	case SKILL_LIST_UP_BOMB_RAN_LV1 :					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE11");

	case SKILL_LIST_UP_RAN_LV2 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE1");
	case SKILL_LIST_DW_WEP_CNG_LV2 :					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE2");
	case SKILL_LIST_DW_WEP_REL_LV2 :					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE3");
	case SKILL_LIST_UP_RPM_LV2 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE4");
	case SKILL_LIST_UP_ACC_LV2 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE5");
	case SKILL_LIST_UP_PEN_LV2 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE6");
	case SKILL_LIST_UP_SPD_LV2 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE7");
	case SKILL_LIST_UP_ATT_LV2 :						return GAME_STRING("STR_SKILL_TEXT_COM_TYPE8");
	case SKILL_LIST_DW_BOMB_TIME_LV2 :					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE9");
	case SKILL_LIST_DW_DRAW_VOL_LV2	:					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE10");
	case SKILL_LIST_UP_BOMB_RAN_LV2	:					return GAME_STRING("STR_SKILL_TEXT_COM_TYPE11");
	}
	return "";
}

const char * SkillManager::getClassTypeName(INT32 idx)
{
	switch(idx)
	{
	case SKILL_TYPE_PRIMARY:	return GAME_STRING("STR_TBL_UI_SKILL_MODE_TYPE1");
	case SKILL_TYPE_SECONDARY:	return GAME_STRING("STR_TBL_UI_SKILL_MODE_TYPE2");
	case SKILL_TYPE_MODE:		return GAME_STRING("STR_TBL_UI_SKILL_MODE_TYPE3");
	}
	return "";
}

const char * SkillManager::getClassName(INT32 idx)
{
	switch(idx)
	{
	case SKILL_ASSAULT:		return GAME_STRING("STR_TBL_CLASS_NAME1");
	case SKILL_SMG:			return GAME_STRING("STR_TBL_CLASS_NAME2");
	case SKILL_SNIPER:		return GAME_STRING("STR_TBL_CLASS_NAME3");
	case SKILL_SHOTGUN:		return GAME_STRING("STR_TBL_CLASS_NAME5");
	case SKILL_MACHINE:		return GAME_STRING("STR_TBL_CLASS_NAME4");
	case SKILL_HANDGUN:		return GAME_STRING("STR_TBL_CLASS_NAME6");
	case SKILL_KNIFE:		return GAME_STRING("STR_TBL_CLASS_NAME7");
	case SKILL_GRENADE:		return GAME_STRING("STR_TBL_CLASS_NAME8");
	case SKILL_DUAL:		return GAME_STRING("STR_TBL_CLASS_NAME9");
	case SKILL_CCMODE:		return GAME_STRING("STR_TBL_CLASS_NAME10");
	case SKILL_DOMINATION:	return GAME_STRING("STR_TBL_CLASS_NAME11");
	case SKILL_G2:			return GAME_STRING("STR_TBL_CLASS_NAME12");
	}
	return "";
}

const char * SkillManager::getSkillExplain(INT32 idx)
{
	switch(idx)
	{
	case SKILL_ASSAULT:		return GAME_STRING("STR_TBL_CLASS_EXP1");
	case SKILL_SMG:			return GAME_STRING("STR_TBL_CLASS_EXP2");
	case SKILL_SNIPER:		return GAME_STRING("STR_TBL_CLASS_EXP3");
	case SKILL_SHOTGUN:		return GAME_STRING("STR_TBL_CLASS_EXP4");
	case SKILL_MACHINE:		return GAME_STRING("STR_TBL_CLASS_EXP5");
	case SKILL_HANDGUN:		return GAME_STRING("STR_TBL_CLASS_EXP6");
	case SKILL_KNIFE:		return GAME_STRING("STR_TBL_CLASS_EXP7");
	case SKILL_GRENADE:		return GAME_STRING("STR_TBL_CLASS_EXP8");
	case SKILL_DUAL:		return GAME_STRING("STR_TBL_CLASS_EXP9");
	case SKILL_CCMODE:		return GAME_STRING("STR_TBL_CLASS_EXP10");
	case SKILL_DOMINATION:	return GAME_STRING("STR_TBL_CLASS_EXP11");
	case SKILL_G2:			return GAME_STRING("STR_TBL_CLASS_EXP12");
	}
	return "";
}

void SkillManager::setPacketClass(PACKET_SET_SPECIALITY_CLASS_REQ *classData)
{
	m_ClassSet.m_ui8Slot = classData->m_ui8Slot;
	m_ClassSet.m_ui8Type = classData->m_ui8Type;
	m_ClassSet.m_ui8Class = classData->m_ui8Class;
}
void SkillManager::setPacketSlot(PACKET_SET_SPECIALITY_REQ *slotData)
{
	m_SlotSet.m_ui64SkillTree = slotData->m_ui64SkillTree;
	m_SlotSet.m_ui8Class = slotData->m_ui8Class;
	m_SlotSet.m_ui8Slot = slotData->m_ui8Slot;
	m_SlotSet.m_ui8Type = slotData->m_ui8Type;
}
void SkillManager::getPacketClass(PACKET_SET_SPECIALITY_CLASS_REQ *classData)
{
	classData->m_ui8Slot = m_ClassSet.m_ui8Slot;
	classData->m_ui8Type = m_ClassSet.m_ui8Type;
	classData->m_ui8Class = m_ClassSet.m_ui8Class;
}
void SkillManager::getPacketSlot(PACKET_SET_SPECIALITY_REQ *slotData)
{
	slotData->m_ui64SkillTree = m_SlotSet.m_ui64SkillTree;
	slotData->m_ui8Class = m_SlotSet.m_ui8Class;
	slotData->m_ui8Slot = m_SlotSet.m_ui8Slot;
	slotData->m_ui8Type = m_SlotSet.m_ui8Type;
}

void SkillManager::SetNewClass(PACKET_SET_SPECIALITY_CLASS_ACK* packet_data)
{
	CHARA_SKILL * _Chara = &m_mySkillInfo[packet_data->m_ui8Slot][packet_data->m_ui8Type];
	_Chara->m_ui8Class = packet_data->m_ui8Class;
}

void SkillManager::SetNewSlot(PACKET_SET_SPECIALITY_ACK* packet_data)
{
	CHARA_SKILL * _Chara = &m_mySkillInfo[packet_data->m_ui8Slot][packet_data->m_ui8Type];
	_Chara->m_ui8Class = packet_data->m_ui8Class;
	_Chara->m_ui64SkillTree = packet_data->m_ui64SkillTree;
	_Chara->m_ui32PlayTime = packet_data->m_ui32PlayTime;
	_Chara->m_ui16PlayTimePoint = packet_data->m_ui16PlayTimePoint;
}

char * SkillManager::GetPefRoot(eSKILL_TYPE _type)
{
	return _strType[_type];
}

char * SkillManager::GetPefSubRoot(eSKILL_CLASS _class)
{
	return _strClass[_class];
}

UINT8 SkillManager::getStageTypeToSkill(STAGE_TYPE _type)
{
	UINT8 _Stage = (UINT8)SKILL_NONE;
	switch(_type)
	{
	case STAGE_TYPE_ESCAPE:
		_Stage = SKILL_DOMINATION;
		break;
	case STAGE_TYPE_DOMINATION:
		break;
	}
	return _Stage;
}

UINT8 SkillManager::getWeaponTypeToSkill(WEAPON_CLASS_TYPE _type)
{
	UINT8 _Class = (UINT8)SKILL_NONE;
	switch(_type)
	{
	case WEAPON_CLASS_ASSAULT :
		_Class = SKILL_ASSAULT;
		break;
	case WEAPON_CLASS_SMG :
		_Class = SKILL_SMG;
		break;
	case WEAPON_CLASS_SNIPER:
		_Class = SKILL_SNIPER;
		break;
	case WEAPON_CLASS_MG:
		_Class = SKILL_MACHINE;
		break;
	case WEAPON_CLASS_SHOTGUN:
		_Class = SKILL_SHOTGUN;
		break;
	case WEAPON_CLASS_HANDGUN:
		_Class = SKILL_HANDGUN;
		break;
	case WEAPON_CLASS_KNIFE:
		_Class = SKILL_KNIFE;
		break;
	case WEAPON_CLASS_THROWING1:
		_Class = SKILL_GRENADE;
		break;
	case WEAPON_CLASS_DUALHANDGUN:
	case WEAPON_CLASS_DUALKNIFE:
	case WEAPON_CLASS_DUALSMG:
		_Class = SKILL_DUAL;
		break;
	default:
		_Class = (UINT8)SKILL_NONE;
		break;


		//WEAPON_CLASS_KNIFE		//	나이프
		//WEAPON_CLASS_HANDGUN,	//	권총
		//WEAPON_CLASS_ASSAULT,	//	소총
		//WEAPON_CLASS_SMG,		//	서브머신건
		//WEAPON_CLASS_SNIPER,	//	저격총
		//WEAPON_CLASS_SHOTGUN,	//	산탄총
		//WEAPON_CLASS_THROWING1,	//	투척 무기
		//WEAPON_CLASS_THROWING2,	//	특수무기(C4,연막)
		//WEAPON_CLASS_MISSION,	//	미션 아이템
		//WEAPON_CLASS_MG,		//	머신건

		//// 게임에서만 사용하는 클래스 ( 서버에는 없습니다.)
		//WEAPON_CLASS_GRENADESHELL,		// 유탄
		//WEAPON_CLASS_BOMBTRIGGER,		// 폭탄 스위치

		//// 새로 추가되는 무기 2008.07.09
		//WEAPON_CLASS_CIC,
		//WEAPON_CLASS_DUALHANDGUN,
		//WEAPON_CLASS_DUALKNIFE,

		//WEAPON_CLASS_ROCKET_LAUNCHER,
		//WEAPON_CLASS_OBJECT_GUN,		// 특정 오브젝트에 붙어 있는 총 ( M197....)

		//WEAPON_CLASS_DUALSMG,

		//WEAPON_CLASS_DINO,		
		//WEAPON_CLASS_DINO_LEFT_SCRATCH,	// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
		//WEAPON_CLASS_TREX,				// 왼쪽 손 할퀴기 이펙트를 위해 만듬.
		//WEAPON_CLASS_STING,
		//WEAPON_CLASS_KNUCKLE,			// 너클
		//WEAPON_CLASS_ELITE_RIGHT_SCRATCH,
		//WEAPON_CLASS_ELITE_LEFT_SCRATCH,
		//WEAPON_CLASS_BOW,
		//WEAPON_CLASS_COUNT,
	}
	return _Class;
}
#endif
