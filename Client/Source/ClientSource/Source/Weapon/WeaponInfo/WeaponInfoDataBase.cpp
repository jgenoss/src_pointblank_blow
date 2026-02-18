#include "pch.h"
#include "WeaponInfoDataBase.h"
#include "WeaponInfo.h"
#include "WeaponGlobal.h"
#include "GlobalVariables.h"
#include "GunInfo.h"
#include "MeleeWeaponInfo.h"
#include "GrenadeInfo.h"
#include "BombInfo.h"
#include "SMGGunInfo.h"
#include "GrenadeGunInfo.h"
#include "M197Info.h"
#include "WeaponDinoInfo.h"
#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include <algorithm>

static const char * s_WeaponPropertyFieldName[] = {
	"Assault",	"Bomb", "Trigger", "Bow", "DinoWeapon", "Dual", "Throwing", "Throwing2", "GrenadeShell",
	"Knuckle", "MG", "Melee", "ObjectGun", "Pistol", "RocketLauncher", "SMG", "Shotgun", "Sniper",
	nullptr
};

I3_CLASS_INSTANCE( WeaponInfoDataBase);//, i3GameNode);

////////////////////////////////////////////////////////////////////////////////////////////////////
//
WeaponInfoDataBase::WeaponInfoDataBase()
{
	m_pGlobalInfo = nullptr;
}

WeaponInfoDataBase::~WeaponInfoDataBase()
{
	for( UINT32 i = 0; i < WEAPON_CLASS_COUNT; i++)
	{
		i3::cu::for_each_SafeRelease(m_WeaponInfoList[i]);		// 이쪽이 나을것 같아서 바꿔봄.소멸자내 clear무의미.(MISC:2015.02.03.수빈)
	}

	I3_SAFE_RELEASE( m_pGlobalInfo);

	for(size_t i = 0; i < m_ShiftKeyList.size(); i++)
	{
		UINT8 * pShiftTable = (UINT8*) m_ShiftKeyList[ i];
		I3MEM_SAFE_FREE( pShiftTable);
	}

	m_ShiftKeyList.clear();
	i3::cu::for_each_safe_release_clear(m_UITemplateWeaponInfos, &i3NamedElement::Release);
	
	m_SpecialDamageHudTexturePath.clear();
	m_SpecialDieHudTexturePath.clear();
	m_ScopeHudTexturePath.clear();
	
	i3::cu::for_each_delete(m_vecSubScopeInfo);
	m_vecSubScopeInfo.clear();

	m_vSndByWeaponKillFileName.clear();
}

UINT8 * WeaponInfoDataBase::_CreateShiftKeyTable( INT32 count)
{
	UINT8 * pNewTable = (UINT8*) i3MemAlloc( sizeof( UINT8) * count);

#if defined( MEM_ENCRYPTION)
	INT32 i;
	for( i = 0; i < count; ++i)
	{
		pNewTable[ i] = (UINT8) ((i3Math::Rand() % 7) + 1);
	}
#else
	i3mem::FillZero( pNewTable, sizeof( UINT8) * count);
#endif

	m_ShiftKeyList.push_back( pNewTable);

	return pNewTable;
}

struct	_WeaponInfo
{
public:
	_WeaponInfo(const string & _Name, INT32 _ClassType = 0, INT32 _Number = 0 )
	{
		this->Name		= _Name;
		this->ClassType	= _ClassType;
		this->Number	= _Number;
	}

	bool operator == (const _WeaponInfo & Right)
	{
		return (ClassType == Right.ClassType) && (Number ==  Right.Number);
	}

	string	Name;
	INT32	ClassType;
	INT32	Number;
};


typedef		i3::vector<_WeaponInfo>					WeaponInfoSet;


void WeaponInfoDataBase::CreateWeapon(i3RegKey* pRoot)
{
	INT32 nWeaponCount = pRoot->getChildCount();
	UINT8 * pShiftTable = _CreateShiftKeyTable( nWeaponCount);

	WeaponInfoSet			DuplicateCheckData;

	for( INT32 i = 0; i < nWeaponCount; ++i)
	{
		i3RegKey * pKey = ( i3RegKey*)pRoot->getChild( i);
		WEAPON_CLASS_TYPE typeClass = WEAPON_CLASS_UNKNOWN;
		T_ItemID Itemid = 0;

		FIND_REG_DATA( pKey, LS_STR("ITEMID"),		(INT32*)&Itemid);				//	Number
		if( Itemid == 0)
			continue;

		typeClass	= WEAPON::ItemID2Class( Itemid);
		WEAPON::RES_ID Number		= WEAPON::ItemID2ResID(Itemid);

		_WeaponInfo	NewWeaponInfo(pKey->GetName(), typeClass, Number);
		WeaponInfoSet::iterator iter = i3::find(DuplicateCheckData.begin(), DuplicateCheckData.end(), NewWeaponInfo);
		if ( iter != DuplicateCheckData.end() )
		{
			_WeaponInfo & Old = (*iter);
			I3PRINTLOG(I3LOG_FATAL, "중복되는 Weapon 정보가 있습니다.\n다음 정보가 중복됩니다. [%s / class:%d / number:%d] [%s / class:%d / number:%d]",
				Old.Name.c_str(), Old.ClassType, Old.Number, NewWeaponInfo.Name.c_str(), NewWeaponInfo.ClassType, NewWeaponInfo.Number );
			continue;
		}
		else
		{
			DuplicateCheckData.push_back(NewWeaponInfo);
		}

		CWeaponInfo * pInfo = CreateWeaponInfo(typeClass);
		if(nullptr == pInfo)
			continue;

		pInfo->ReadWeaponInfo(pKey);
		pInfo->OnEncriptInfo( pShiftTable[ i]);

		//I3TRACE("[CreateWeapon] add : %s (%s)\n", pInfo->GetName(), szName);

		i3::vu::set_value_force(m_WeaponInfoList[typeClass], Number, pInfo);
	}

}

namespace
{
	void FitAllocationWeaponTable(i3::vector<CWeaponInfo*> (&inout)[WEAPON_CLASS_COUNT] )
	{

		for (UINT32 i = 0 ; i < WEAPON_CLASS_COUNT ; ++i)
		{
			i3::vector<CWeaponInfo*>& ctnWeaponInfo = inout[i];
			i3::vector<CWeaponInfo*>(ctnWeaponInfo.begin(), ctnWeaponInfo.end()).swap(ctnWeaponInfo);
		}
	}

}

bool WeaponInfoDataBase::Create()
{
	i3RegKey * pWeaponRoot = nullptr;
	i3RegKey * pKey = nullptr;

	CWeaponInfo::InitializeAttributeKey();

	i3RegKey * pWeaponList = Config::_GetRegRoot( "Weapon/WeaponList" );

	pWeaponRoot = Config::FindKey("Weapon", pWeaponList);
	I3ASSERT( pWeaponRoot != nullptr);

	m_pGlobalInfo = CWeaponGlobal::new_object();
	m_pGlobalInfo->ReadInfo( pWeaponRoot);

	CreateWeapon(pWeaponRoot);

	for(INT32 i = 0; s_WeaponPropertyFieldName[i] != nullptr; i++)
	{
		pKey = Config::FindKey( s_WeaponPropertyFieldName[i], pWeaponRoot);
		I3ASSERT( pKey != nullptr);
		CreateWeapon(pKey);
	}

	// 재배치대신 resize규모가 큰것을 염두에 두고 m_WeaponList[n]의 과다 할당만 fitting합니다.
	FitAllocationWeaponTable(m_WeaponInfoList);

	_InitExtWeaponTable();	// 테이블 정보 초기화를 미리 해둔다.

	// 원본무기별 사용되는 확장 무기 ItemID를 전부 찾아 등록합니다.
	_SetCompatibleWeaponID();

	_ReadUITemplateInfo();

	_ReadSubScopeInfo();

	return true;
}

CWeaponInfo	* WeaponInfoDataBase::getWeaponInfo( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number, bool check /*=FALSE*/)
{
	if (WEAPON_CLASS_COUNT <= classType)
	{
		I3ASSERT(classType < WEAPON_CLASS_COUNT);
		return nullptr;
	}

	if( number < m_WeaponInfoList[ classType].size())
		return m_WeaponInfoList[ classType].at( number);

	if( check)	//	이게 활성화 안되면 없는 무기를 요청할시 nullptr을 반환하므로 이것을 호출하는 함수는 nullptr 포인터 접근하여 크래시하는 경우가 많습니다. komet
	{
		//	여기 들어오면 안됨....엉뚱한 무기를 찾고 있습니다. BC 파일에는 요구한 무기 정보가 없음
		char szMessage[MAX_STRING_COUNT] = "";
		sprintf_s( szMessage, "Cannot find weapon information.\n [classType %d : number %d]", (INT32)classType, (INT32)number);
		I3PRINTLOG(I3LOG_FATAL,  szMessage);
	}

	return nullptr;
}

CWeaponInfo	* WeaponInfoDataBase::getWeaponInfo( T_ItemID itemID, bool check /*=false*/)
{
	WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( itemID);
	WEAPON::RES_ID weaponID = WEAPON::ItemID2ResID( itemID);

	return getWeaponInfo( classType, weaponID, check);
}

#if !defined( I3_NO_PROFILE)
void WeaponInfoDataBase::OnReloadProperty()
{
	i3RegKey * pPef = Config::_GetRegRoot("Weapon/WeaponList");
	i3RegKey * pWeaponRoot = Config::FindKey("Weapon", pPef);
	I3ASSERT( pWeaponRoot != nullptr);

	if( m_pGlobalInfo != nullptr)
	{
		m_pGlobalInfo->ReadInfo( pWeaponRoot);
	}

	INT32 nWeaponCount = pWeaponRoot->getChildCount();
	for(INT32 i = 0; i < nWeaponCount; ++i)
	{
		i3RegKey * pKey = (i3RegKey*)pWeaponRoot->getChild( i);

		T_ItemID itemID = 0;
		FIND_REG_DATA( pKey, LS_STR("ItemID"), (INT32*)&itemID);

		WEAPON::RES_ID	nNumber = WEAPON::ItemID2ResID( itemID);
		WEAPON_CLASS_TYPE	ClassType = WEAPON::ItemID2Class( itemID);

		if( nNumber < m_WeaponInfoList[ClassType].size())
		{
			CWeaponInfo * pInfo = m_WeaponInfoList[ClassType].at(nNumber);

			if( pInfo != nullptr)
			{
				//다시 읽어들이려는 값과 이전데이터의 무기인덱스가 틀리다면 무언가 잘못된 데이터입니다.
				I3ASSERT( pInfo->isSame(ClassType, nNumber) );

				pInfo->ReadWeaponInfo( pKey);
			}
		}
	}

	i3RegKey* pUITemplateInfo = Config::_GetRegRoot("Weapon/UITemplate");
	I3ASSERT(pUITemplateInfo != nullptr);
	for ( INT32 i=0; i<pUITemplateInfo->getChildCount(); ++i)
	{
		i3RegKey* pKey = (i3RegKey*)pUITemplateInfo->getChild(i);
		CUITemplateInfo_Weapon* pInfo = m_UITemplateWeaponInfos[i];
		pInfo->ReadInfo(pKey);
	}
}
#endif

void WeaponInfoDataBase::OnUpdate(REAL32 rDeltaSeconds)
{

}

i3::rc_wstring WeaponInfoDataBase::GetWeaponCategory(CWeaponInfo * pInfo)
{
	i3::rc_wstring wstrCategory;

	switch(pInfo->GetTypeUsage())
	{
	case WEAPON_SLOT_PRIMARY:		wstrCategory = GAME_RCSTRING("STBL_IDX_WEAPON_SLOT_TITLE1");	break;
	case WEAPON_SLOT_SECONDARY:		wstrCategory = GAME_RCSTRING("STBL_IDX_WEAPON_SLOT_TITLE2");	break;
	case WEAPON_SLOT_MELEE:			wstrCategory = GAME_RCSTRING("STBL_IDX_WEAPON_SLOT_TITLE3");	break;
	case WEAPON_SLOT_THROWING1:		wstrCategory = GAME_RCSTRING("STBL_IDX_WEAPON_SLOT_TITLE4");	break;
	case WEAPON_SLOT_THROWING2:		wstrCategory = GAME_RCSTRING("STBL_IDX_WEAPON_SLOT_TITLE5");	break;
	default:
		wstrCategory = UNKNOWN_TEXTW;
		break;
	}

	return wstrCategory;
}

#if defined(LOCALE_RUSSIA)
// 이노바 요청에 의해 무기 종류만 표시합니다.
void WeaponInfoDataBase::GetWeaponCategoryEx( CWeaponInfo * pInfo, i3::rc_wstring& out)
{
	out = GetWeaponClass( pInfo);
}
#else
void WeaponInfoDataBase::GetWeaponCategoryEx( CWeaponInfo * pInfo, i3::rc_wstring& out)
{

	i3::rc_wstring wstrCategory = GetWeaponCategory( pInfo);

	switch(pInfo->GetTypeClass())
	{
	case WEAPON_CLASS_ASSAULT:		//	돌격소총
	case WEAPON_CLASS_SMG:			//	기관단총
	case WEAPON_CLASS_DUALSMG:		//	기관단총
	case WEAPON_CLASS_SNIPER:		//	저격소총	
	case WEAPON_CLASS_SHOTGUN:		//	산탄총
	case WEAPON_CLASS_MG:			//	기관총
		{
			i3::stack_wstring wstr;
			i3::sprintf( wstr, L"%s (%s)", wstrCategory, GetWeaponClass( pInfo));
			out = wstr;
		}
		break;
	default:
		{
			out = wstrCategory;
		}
		break;
	}
}
#endif

i3::rc_wstring WeaponInfoDataBase::GetWeaponClass(CWeaponInfo * pInfo)
{
	I3ASSERT( pInfo != nullptr);

	return GetWeaponClass( pInfo->GetTypeClass());
}

i3::rc_wstring WeaponInfoDataBase::GetWeaponClass( WEAPON_CLASS_TYPE type)
{
	I3_BOUNDCHK( type, (INT32) WEAPON_CLASS_COUNT);

	switch( type)
	{
	case WEAPON_CLASS_UNKNOWN			:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_UNKNOWN");
	case WEAPON_CLASS_KNIFE				:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_KNIFE");	
	case WEAPON_CLASS_KNUCKLE			:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_KNUCKLE");	// revision 32837 너클 Class 스트링 라벨 수정 [STBL_IDX_WEAPON_CLASS_KNIFE -> STBL_IDX_WEAPON_CLASS_KNUCKLE]
	case WEAPON_CLASS_HANDGUN			:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_HANDGUN");	
	case WEAPON_SUBCLASS_ASSAULT		:
	case WEAPON_CLASS_ASSAULT			:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ASSAULT");
	case WEAPON_SUBCLASS_BOW:
	case WEAPON_CLASS_BOW				:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ASSAULT");	
	case WEAPON_SUBCLASS_SMG			:
	case WEAPON_CLASS_SMG				:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SMG");	
	case WEAPON_SUBCLASS_SNIPER			:
	case WEAPON_CLASS_SNIPER			:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SNIPER");
	case WEAPON_SUBCLASS_SHOTGUN		:
	case WEAPON_CLASS_SHOTGUN			:	
	case WEAPON_CLASS_DUALSHOTGUN		:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SHOTGUN");	
	case WEAPON_CLASS_THROWING_GRENADE	:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_THROWING");	
	case WEAPON_CLASS_THROWING_CONTAIN	:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ITEM");	
	case WEAPON_CLASS_THROWING_ATTACK	:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ITEM");	
	case WEAPON_CLASS_THROWING_HEAL		:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ITEM");	
	case WEAPON_CLASS_MISSION			:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_MISSION");	
	case WEAPON_CLASS_MG				:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_MG");	
	case WEAPON_CLASS_GRENADESHELL		:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_GRENADESHELL");	
	case WEAPON_CLASS_BOMBTRIGGER		:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_BOMBTRIGGER");	
	case WEAPON_CLASS_CIC				:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_CIC");	
	case WEAPON_CLASS_DUALHANDGUN		:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_DUALHANDGUN");	
	case WEAPON_CLASS_DUALKNIFE			:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_DUALKNIFE");	
	case WEAPON_CLASS_ROCKET_LAUNCHER	:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_ROCKET_LAUNCHER");	
	case WEAPON_CLASS_OBJECT_GUN		:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_OBJECT_GUN");	
	case WEAPON_CLASS_DUALSMG			:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_SMG");
	case WEAPON_CLASS_DINO				:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_DINO");
	default								:	return GAME_RCSTRING("STBL_IDX_WEAPON_CLASS_UNKNOWN");	
	}
}

bool WeaponInfoDataBase::IsUsedWeapon(const USER_INFO_EQUIPMENT_WEAPON* pEquipWeapon, INT32 nWeaponFlag)
{
	if( (UINT32)nWeaponFlag == pEquipWeapon->m_TItemID[ CHAR_EQUIPMENT_WEAPON_PRIMARY ] )		return true;
	if( (UINT32)nWeaponFlag == pEquipWeapon->m_TItemID[ CHAR_EQUIPMENT_WEAPON_SECONDARY ] )	return true;
	if( (UINT32)nWeaponFlag == pEquipWeapon->m_TItemID[ CHAR_EQUIPMENT_WEAPON_MELEE ] )		return true;	
	if( (UINT32)nWeaponFlag == pEquipWeapon->m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING1 ] )	return true;
	if( (UINT32)nWeaponFlag == pEquipWeapon->m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING2 ] )	return true;

	return false;
}

bool WeaponInfoDataBase::BeDescriptionWeapon(CWeaponInfo* pInfo)
{
#if 1	// 상세보기가 존재하는 무기인지 확인한다.
	
	return pInfo->isDescriptionImage();

#else

	return IsExtensionWeapon(pInfo);

#endif

	//	2008.10.27일자로 기획상 모든 무기는 Description을 볼 수 있도록 변경.
	//return true;
}

bool WeaponInfoDataBase::IsExtensionWeapon( CWeaponInfo * pInfo)
{
	return (bool) (pInfo->getExtensionType(0) != WEAPON::EXT_NONE);
}

void WeaponInfoDataBase::_InitExtWeaponTable( void)
{ 
	for( UINT32 type = 1; type < WEAPON_CLASS_COUNT; type++)
	{
		for( size_t i = 0; i < m_WeaponInfoList[type].size(); ++i)
		{
			CWeaponInfo * pWeaponInfo = m_WeaponInfoList[type].at(i);
			if( pWeaponInfo == nullptr) continue;

			T_ItemID extItemID = pWeaponInfo->GetSubExtensionItemID();

			if( extItemID == 0)
				continue;

			// 보통 2번째 배열에 보조 무기 타입이 있다.
			WEAPON::EXT_TYPE extensionType = pWeaponInfo->getExtensionType(1);

			if( extensionType == WEAPON::EXT_NONE)
				extensionType = pWeaponInfo->getExtensionType(0);

			// 보조 유탄 발사 같은 익스텐션 무기의 정보를 테이블에 입력
			CWeaponInfo * pExtInfo = g_pWeaponInfoDataBase->getWeaponInfo( extItemID);

			if( pExtInfo == nullptr)
			{	// 보조 무기 정보가 없을 경우 유탄등 보조무기로 적을 킬할 경우 도전과제가 정상적으로 완료되지 않을 수 있다.
				char str[MAX_STRING_COUNT] = "";
				i3::rc_wstring wstrName;
				pWeaponInfo->GetWeaponName( wstrName);

				i3::snprintf( str, MAX_STRING_COUNT, "ExtensionInfo [ %d ] Cannot find Weapon.PEF info from [ %s ].\r\nPlease check!!", extItemID, wstrName);
				I3PRINTLOG(I3LOG_FATAL,  str);
			}
		}
	}
}

void WeaponInfoDataBase::_SetCompatibleWeaponID(void)
{
	for( UINT32 type = 1; type < WEAPON_CLASS_COUNT; type++)
	{
		for( size_t i = 0; i < m_WeaponInfoList[type].size(); ++i)
		{
			CWeaponInfo * pWeaponInfo = m_WeaponInfoList[type].at(i);
			if( pWeaponInfo != nullptr)
				_RegisterCompatibleWeaponID(pWeaponInfo, 0, pWeaponInfo->GetItemID());
		}
	}
}

void WeaponInfoDataBase::_RegisterCompatibleWeaponID(CWeaponInfo * pDestInfo, INT32 idx, T_ItemID ItemID)
{
	CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(ItemID, true);

	for(int i = 0 ; i < MAX_WEAPON_EXTENSION ; ++i)
	{
		T_ItemID extItemID = pWeaponInfo->GetExtensionItemID(i);
		if(extItemID == 0) continue;

		pDestInfo->SetCompatibleItemID(idx++, extItemID, pWeaponInfo->isUseExtShape(i) );

		if( ItemID != extItemID )
			_RegisterCompatibleWeaponID(pDestInfo, idx, extItemID);
	}
}

CWeaponInfo * WeaponInfoDataBase::CreateWeaponInfo( WEAPON_CLASS_TYPE ClassType )
{
	CWeaponInfo * pInfo = nullptr;
	switch(ClassType) 
	{
	case WEAPON_CLASS_KNIFE:
	case WEAPON_CLASS_KNUCKLE:
	case WEAPON_CLASS_DUALKNIFE :
			pInfo = CMeleeWeaponInfo::new_object();
			break;
	case WEAPON_CLASS_DINO:
		{
			pInfo = CWeaponDinoInfo::new_object();	
		}
		break;
	case WEAPON_CLASS_HANDGUN:
	case WEAPON_SUBCLASS_ASSAULT :
	case WEAPON_CLASS_ASSAULT:
	case WEAPON_SUBCLASS_BOW:
	case WEAPON_CLASS_BOW:
	case WEAPON_SUBCLASS_SNIPER :
	case WEAPON_CLASS_SNIPER:
	case WEAPON_SUBCLASS_SHOTGUN:
	case WEAPON_CLASS_SHOTGUN:
	case WEAPON_CLASS_MG:
	case WEAPON_CLASS_CIC:
	case WEAPON_CLASS_DUALHANDGUN:
	case WEAPON_CLASS_DUALSMG :
	case WEAPON_CLASS_ROCKET_LAUNCHER :
	case WEAPON_SUBCLASS_G_LAUNCHER:
	case WEAPON_CLASS_DUALSHOTGUN :
		{
			pInfo = CGunInfo::new_object();
		}
		break;

	case WEAPON_SUBCLASS_SMG :
	case WEAPON_CLASS_SMG:
		pInfo = CSMGGunInfo::new_object();
		break;

	case WEAPON_CLASS_OBJECT_GUN:
		{
			pInfo = CM197Info::new_object();
		}
		break;
	case WEAPON_CLASS_THROWING_GRENADE:
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_THROWING_HEAL:
		{
			pInfo = CGrenadeInfo::new_object();
		}
		break;
	case WEAPON_CLASS_MISSION:
		{
			pInfo = CBombInfo::new_object();
		}
		break;
	case WEAPON_CLASS_BOMBTRIGGER:
		{
			pInfo = CWeaponInfo::new_object();
		}
		break;
	case WEAPON_CLASS_GRENADESHELL:
		{
			pInfo = CGrenadeGunInfo::new_object();
		}
		break;
	}
	return pInfo;
}

void WeaponInfoDataBase::_ReadUITemplateInfo()
{
	i3RegKey* rootKey = Config::_GetRegRoot("Weapon/UITemplate");
	I3ASSERT(rootKey != nullptr);

	if (rootKey)
	{
		INT32 count = rootKey->getChildCount();
		for (INT32 i=0; i<count; i++)
		{
			i3RegKey* subKey = static_cast<i3RegKey*>(rootKey->getChild(i));

			CUITemplateInfo_Weapon* info = CUITemplateInfo_Weapon::new_object();
			info->ReadInfo(subKey);

			m_UITemplateWeaponInfos.push_back(info);
		}
	}
}

void WeaponInfoDataBase::_ReadSubScopeInfo()
{
	i3RegKey * pSubScopeInfo = Config::_GetRegRoot("Weapon/SubScopeHudInfo");
	I3ASSERT(pSubScopeInfo != nullptr);
	
	if (pSubScopeInfo == nullptr) return;

	INT32 cnt = pSubScopeInfo->getChildCount();

	for (INT32 i = 0; i < cnt; ++i)
	{
		i3RegKey * pKey = (i3RegKey*)pSubScopeInfo->getChild(i);
		if (pKey == nullptr) continue;

		INT32 nTemp = 0;
		FIND_REG_DATA(pKey, "idx",				&(nTemp));

		if (i3::vu::is_value_valid(m_vecSubScopeInfo, nTemp))
			continue;

		SUB_SCOPE_INFO* newData = new SUB_SCOPE_INFO();
		newData->m_nIdx = nTemp;

		nTemp = 0;
		FIND_REG_DATA(pKey, "EnableOffDotLaser",	&nTemp);
		newData->m_bOffDotLaser = (nTemp != 0);
		
		nTemp = 0;
		FIND_REG_DATA(pKey, "EnableOnScopeSound", &nTemp);
		newData->m_bOnScopeSound = (nTemp != 0);

		FIND_REG_DATA(pKey, "DotStartPos",		&(newData->m_vec2DotStartPos));
		FIND_REG_DATA(pKey, "DotWidthHeight",	&(newData->m_vec2DotWidthHeight));
		
		i3::vu::set_value_force(m_vecSubScopeInfo, newData->m_nIdx, newData);
	}
}

size_t WeaponInfoDataBase::GetUITemplateInfoCount() const
{
	return m_UITemplateWeaponInfos.size();
}

const CUITemplateInfo_Weapon* WeaponInfoDataBase::GetUITemplateInfo(const size_t i) const
{
	return m_UITemplateWeaponInfos.at(i);
}

INT32 WeaponInfoDataBase::InsertSpecialDamageHUDTexturePath(const i3::rc_wstring texture_path )
{
	int find_index = i3::vu::int_index_of( m_SpecialDamageHudTexturePath, texture_path );
	
	if( find_index == -1 )
	{
		m_SpecialDamageHudTexturePath.push_back( texture_path );
		return m_SpecialDamageHudTexturePath.size() - 1;
	}
	return find_index;
}

bool WeaponInfoDataBase::GetSpecialDamageHUDTexturePath(INT32 index, i3::rc_wstring& out)
{
	if( i3::vu::is_value_valid( m_SpecialDamageHudTexturePath, index) )
	{
		out = m_SpecialDamageHudTexturePath[index];
		return true;
	}
	return false;
}

INT32 WeaponInfoDataBase::InsertSpecialDieHUDTexturePath(const i3::rc_wstring texture_path)
{
	int find_index = i3::vu::int_index_of(m_SpecialDieHudTexturePath, texture_path);

	if (find_index == -1)
	{
		m_SpecialDieHudTexturePath.push_back(texture_path);
		return m_SpecialDieHudTexturePath.size() - 1;
	}
	return find_index;
}

bool WeaponInfoDataBase::GetSpecialDieHUDTexturePath(INT32 index, i3::rc_wstring& out)
{
	if (i3::vu::is_value_valid(m_SpecialDieHudTexturePath, index))
	{
		out = m_SpecialDieHudTexturePath[index];
		return true;
	}
	return false;
}

INT32 WeaponInfoDataBase::InsertScopeHUDTexturePath(const i3::rc_wstring texture_path)
{
	int find_index = i3::vu::int_index_of(m_ScopeHudTexturePath, texture_path);

	if ( (texture_path.size() > (size_t)0) && find_index == -1)
	{
		m_ScopeHudTexturePath.push_back(texture_path);
		return m_ScopeHudTexturePath.size() - 1;
	}
	return find_index;
}

bool WeaponInfoDataBase::GetScopeHUDTexturePath(INT32 index, i3::rc_wstring & out)
{
	if (i3::vu::is_value_valid(m_ScopeHudTexturePath, index))
	{
		out = m_ScopeHudTexturePath[index];
		return true;
	}
	return false;
}

const SUB_SCOPE_INFO * WeaponInfoDataBase::GetSubScopeInfo(INT32 index)
{
	if (index >= 0)
	{
		return m_vecSubScopeInfo.at(index);
	}
	return nullptr;
}

INT32 WeaponInfoDataBase::InsertWeaponKillSound(const i3::rc_wstring soundname)
{
	i3::rc_wstring filename;
	i3::sprintf(filename, L"%s.wav", soundname);

	int find_index = i3::vu::int_index_of(m_vSndByWeaponKillFileName, filename);

	if (find_index == -1)
	{
		m_vSndByWeaponKillFileName.push_back(filename);
		return m_vSndByWeaponKillFileName.size() - 1;
	}

	return find_index;
}

INT32 WeaponInfoDataBase::GetWeaponKillSoundCount()
{
	return (INT32)m_vSndByWeaponKillFileName.size();
}

i3::string WeaponInfoDataBase::GetWeaponSkillSoundFileName(INT32 index)
{
	i3::string strname;
	i3::utf16_to_mb(m_vSndByWeaponKillFileName[index], strname);
	
	return strname;
}
