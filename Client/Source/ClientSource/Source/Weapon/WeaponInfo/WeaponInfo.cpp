#include "pch.h"
#include "WeaponInfo.h"

#include "WeaponDualMagInfo.h"
#include "WeaponScopeInfo.h"
#include "WeaponSilencerInfo.h"
#include "WeaponZoomInfo.h"
#include "MyRoomInfoContext.h"
#include "../WeaponItemID.h"
#include "i3Base/string/ext/utf16_to_mb.h"


#if defined( I3_DEBUG)
#define ASSERT_WI(a)	I3ASSERT(a)
#else
#define ASSERT_WI(a)	__noop
#endif

const char g_chBlank = '\0';

I3_CLASS_INSTANCE( CWeaponInfo); //, i3NamedElement);

CWeaponInfo::CWeaponInfo(void) :
	//m_TypeSlot(WEAPON_SLOT_UNKNOWN),
	m_nDamageOnlyUI(0),
	m_rWeight(0.f), m_rMoveNormalFire(0.f), m_rMoveNormalSit(0.f),
	m_rMoveNormalStand(0.f), m_rValue(0.f), 
	m_rPhysixForceRate(1.f), 
	m_TracerType(TRACER_TYPE_BULLET), m_rCrouchParalysis(0.f),
	m_rScaleRate(0.0f), m_rXAxis(0.0f), m_rYAxis(0.0f), m_rZAxis(0.0f), m_rBrightness(1.0f), m_rIdleEffectLoopTime(0.0f)
	, m_nLevel(0), m_nMaxLevel(0)
	, m_TerrainHitSoundIndex(-1)
	, m_CharaHitSoundIndex(-1), m_CharaHitSoundIndex2(-1)
	, m_nUIShapeIndex(0)
	, m_bExtFireEffectOffset(false), m_vPosExtFireEffectOffset(VEC3D())
	, m_Shop3DViewObjQuat(QUATERNION(0.f, 0.f, 0.f, 1.f))
	, m_Shop3DViewObjPos(VEC3D(0.f, 0.f, 0.f))
	, m_Shop3DViewSilencer(0)
	, m_Shop3DViewDist(0.0f)
	, m_UnDecalEffFlag(0), m_UnBlowEffFlag(0)
	, m_nSpecialDamageHUDIndex(-1), m_nSpecialDieHUDIndex(-1)
	, m_UnColorEffectFlag(0), m_IsLimitedByDesigSlot(0)
	, m_KillSoundIndex(-1)
	, m_bUseHazeEffect(0)

{
	m_nRealNumber			= 0;
	m_nItemID				= 0;
	m_Attribute				= 0;
	m_TypeFire				= (WEAPON::FIRE_UNKNOWN);
	m_TypeTrigger			= (WEAPON::TRIGGER_UNKNOWN);
	m_TypeDamage			= (WEAPON::DAMAGE_UNKNOWN); 
	m_nDamage				= 0; 
	m_nMaxDamage			= 0;
	m_rFireDelay			= 0.f;
	m_rFireDelay2			= 0.f;
	m_rRange				= 0.f;
	m_rHelmetPenetrate		= 0.f;
	m_rHelmetPenetrate_Str	= 0.f;
	m_nIndicatorType		= (WEAPON_INDICATOR_UNKNOWN);
	m_rIndicateDistance		= 0.f;
	m_nDamageForDino		= 0;
	m_nMaxDamageForDino		= 0; 
	m_nCriticalDamageChance = 0;
	m_fCriticalDamageRate	= 0.f;
	m_bParalysis			= false; 
	m_rJumpParalysis		= 0.f; 
	m_rWalkParalysis		= 0.f;
	m_rRunParalysis			= 0.f;
	m_WeaponOriginalItemID	= 0;

	for( INT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		m_pExtInfo[i] = nullptr;
		m_stCompatibleInfo[i].Reset();
	}	

	i3mem::FillZero( m_RotateAngleTable, sizeof( m_RotateAngleTable));
	i3mem::FillZero( m_u8EncryptKey, sizeof( m_u8EncryptKey));
}

CWeaponInfo::~CWeaponInfo(void)
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		I3_SAFE_RELEASE( m_pExtInfo[ i]);
	}
}

void CWeaponInfo::_ReadDamageKey( i3RegKey * pKey)
{
	INT32 nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("_DamageType"), &nTemp);
	m_TypeDamage = (WEAPON::DAMAGE_TYPE)nTemp;

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("HelmetPenetrate"),		m_rHelmetPenetrate);
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("HelmetPenetrate_Str"), m_rHelmetPenetrate_Str);

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("Damage"),					&nTemp);
	I3_BOUNDCHK( nTemp, 65535);
	m_nDamage = (UINT16) nTemp;

	nTemp = m_nDamage;
	FIND_REG_DATA( pKey, LS_STR("DamageForDino"),			&nTemp);
	I3_BOUNDCHK( nTemp, 65535);
	m_nDamageForDino = (UINT16) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("DamageMax"),				&nTemp);
	I3_BOUNDCHK( nTemp, 65535);
	m_nMaxDamage = (UINT16) nTemp;

	nTemp = m_nMaxDamage;
	FIND_REG_DATA( pKey, LS_STR("DamageMaxForDino"),		&nTemp);
	I3_BOUNDCHK( nTemp, 65535);
	m_nMaxDamageForDino = (UINT16) nTemp;

	// revision 53433
	// 마비효과
	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("Paralysis"),				&nTemp);
	m_bParalysis = (nTemp>0);
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("ParalysisJump"),			m_rJumpParalysis);				// 마비시간
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("ParalysisWalk"),			m_rWalkParalysis);				// 마비시간
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("ParalysisRun"),			m_rRunParalysis);				// 마비시간

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CriticalDamageChance"),	m_nCriticalDamageChance);
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CriticalDamageRate"),		m_fCriticalDamageRate);

	REAL32 rTemp = 0.0f;
	FIND_REG_DATA(pKey, "Damage_OnlyUI", &rTemp);
	I3_BOUNDCHK(rTemp, 65535);
	m_nDamageOnlyUI = (REAL32)rTemp;
}

/*virtual*/ void CWeaponInfo::_ReadMovementKey( i3RegKey * pKey)
{
	FIND_REG_DATA( pKey, LS_STR("MoveNormal_Fire"),			&m_rMoveNormalFire);			// 무기 소지시 앉은상태의 기본 이동 속도
	FIND_REG_DATA( pKey, LS_STR("MoveNormal_Sit"),			&m_rMoveNormalSit);				// 무기 견착시 앉은상태의 이동속도
	FIND_REG_DATA( pKey, LS_STR("MoveNormal_Stand"),		&m_rMoveNormalStand);			// 무기 견착+발사시 앉은상태의 이동속도
}

void CWeaponInfo::__ReadGatchaKey( i3RegKey * pKey)
{
	REAL32 fTemp = 1.0f;
	FIND_REG_DATA( pKey, "ScaleRate",				&fTemp);
	m_rScaleRate = (REAL32)fTemp;

	fTemp = 1.0f;
	FIND_REG_DATA( pKey, "GachaBrigtness",			&fTemp);
	m_rBrightness = (REAL32)fTemp;

	fTemp = 0.0f;
	FIND_REG_DATA( pKey, "XRotateAxis",				&fTemp);
	m_rXAxis = (REAL32)fTemp;

	fTemp = 0.0f;
	FIND_REG_DATA( pKey, "YRotateAxis",				&fTemp);
	m_rYAxis = (REAL32)fTemp;

	fTemp = 0.0f;
	FIND_REG_DATA( pKey, "ZRotateAxis",				&fTemp);
	m_rZAxis = (REAL32)fTemp;
}

struct tagExtType {
	WEAPON::EXT_TYPE		_type;
	const wchar_t*			_wszExtensionTypeName;
	i3ClassMeta *			_ClassMeta;
};

tagExtType sExtensions[ WEAPON::EXT_MAX] = {
	WEAPON::EXT_NONE,				L"",						nullptr,									// EXT_NONE
	WEAPON::EXT_DUAL_MAG,			L"DualMag",					CWeaponDualMagInfo::static_meta(),		// EXT_DUAL_MAG
	WEAPON::EXT_SILENCER,			L"Silencer",				CWeaponSilencerInfo::static_meta(),		// EXT_SILENCER
	WEAPON::EXT_ZOOM,				L"Zoom",					CWeaponZoomInfo::static_meta(),			// EXT_ZOOM
	WEAPON::EXT_DOT_SIGHT,			L"DotSight",				CWeaponScopeInfo::static_meta(),		// EXT_DOT_SIGHT
	WEAPON::EXT_STEADY_AIM,			L"SteadyAim",				CWeaponZoomInfo::static_meta(),			// EXT_STEADY_AIM
	WEAPON::EXT_SCOPE,				L"Scope",					CWeaponScopeInfo::static_meta(),		// EXT_SCOPE2
	WEAPON::EXT_THROW_SHELL,		L"ThrowShell",				CWeaponExtInfoBase::static_meta(),		// EXT_RLAUNCHER_SHELL
	WEAPON::EXT_GLAUNCHER,			L"GrenadeLauncher",			CWeaponExtInfoBase::static_meta(),		// EXT_GLAUNCHER
	WEAPON::EXT_SHOTGUN_LAUNCHER,	L"ShotGunLauncher",			CWeaponExtInfoBase::static_meta(),		// EXT_SHOTGUN_LAUNCHER
	WEAPON::EXT_BAYONET,			L"Bayonet",					CWeaponExtSubInfo::static_meta(),		// EXT_BAYONET
	WEAPON::EXT_CROSSFIRE,			L"CrossFire",				CWeaponExtGunInfo::static_meta(),		// EXT_CROSSFIRE
	WEAPON::EXT_DUAL,				L"Dual",					CWeaponExtGunInfo::static_meta(),		// EXT_DUAL				
	WEAPON::EXT_DUAL_LEFT,			L"Dual_Left",				CWeaponExtInfoBase::static_meta(),		// EXT_DUAL_LEFT
	WEAPON::EXT_DIRECT_MELEE,		L"DirectMelee",				CWeaponExtSubInfo::static_meta(),		// EXT_DIRECT_MELEE
	WEAPON::EXT_THROW,				L"Throw",					CWeaponExtInfoBase::static_meta(),		// EXT_THROW
	WEAPON::EXT_DINO,				L"Dino",					CWeaponExtInfoBase::static_meta(),		// EXT_THROW
	WEAPON::EXT_DUAL_LEFT_DIF,		L"Dual_Left_Dif",			CWeaponExtInfoBase::static_meta(),		// EXT_DUAL_LEFT_DIF
};

void CWeaponInfo::_ReadExtensionKey( i3RegKey * pKey, INT32 idx)
{
	i3::rc_wstring	strTemp;

	FIND_REG_DATA( pKey, "Type", strTemp );

	if( i3::generic_string_size( strTemp) > 0)
	{
		for( INT32 i = 1; i < WEAPON::EXT_MAX; i++)
		{
			if( i3::generic_is_iequal( strTemp, sExtensions[i]._wszExtensionTypeName) )
			{
				m_pExtInfo[ idx] = (CWeaponExtInfoBase*) sExtensions[i]._ClassMeta->create_instance();
				I3_MUST_ADDREF(m_pExtInfo[ idx]);

				m_pExtInfo[ idx]->OnReadInfo( pKey, sExtensions[i]._type);
				break;
			}
		}
				
		//Extention1이 Ext_FireEffectOffset을 사용하고
		//Extention2이 Ext_FireEffectOffset을 사용하지 않을 경우
		//값이 덮어씌워지는 버그가 있어서 먼저 검사
		if (!m_bExtFireEffectOffset)
		{
			INT32 nTemp = 0;
			FIND_REG_DATA(pKey, "Ext_FireEffectOffset", &nTemp);
			m_bExtFireEffectOffset = nTemp > 0;
			if (m_bExtFireEffectOffset)
			{
				VEC3D temp_vec;
				FIND_REG_DATA(pKey, "Ext_PosFireEffectOffset", &temp_vec);
				m_vPosExtFireEffectOffset.x = (temp_vec.x * 0.01f);
				m_vPosExtFireEffectOffset.y = (temp_vec.z * 0.01f);
				m_vPosExtFireEffectOffset.z = (temp_vec.y * 0.01f);
			}
		}
	}

}

void CWeaponInfo::ReadShop3DViewKey( i3RegKey * pKey )
{
	FIND_REG_DATA( pKey, "PosX",		&m_Shop3DViewObjPos.x);
	FIND_REG_DATA( pKey, "PosY",		&m_Shop3DViewObjPos.y);
	FIND_REG_DATA( pKey, "PosZ",		&m_Shop3DViewObjPos.z );
	
	FIND_REG_DATA( pKey, "QuatX",		&m_Shop3DViewObjQuat.x);
	FIND_REG_DATA( pKey, "QuatY",		&m_Shop3DViewObjQuat.y);
	FIND_REG_DATA( pKey, "QuatZ",		&m_Shop3DViewObjQuat.z);
	FIND_REG_DATA( pKey, "QuatW",		&m_Shop3DViewObjQuat.w);

	FIND_REG_DATA( pKey, "SilencerDisable",		&m_Shop3DViewSilencer);

	FIND_REG_DATA( pKey, "Dist",		&m_Shop3DViewDist);
}

void CWeaponInfo::_ReadEffectKey( i3RegKey * pRootKey, i3RegKey * pEffRegKey)
{
	INT32 nTemp = 0;

	FIND_REG_DATA( pEffRegKey, "_FireEffect",		m_strFireEffect[0] );				//	발사 이펙트
	FIND_REG_DATA( pRootKey, "_FireEffectExt",		m_strFireEffectExt );

	FIND_REG_DATA(pEffRegKey, "HazeEffect", &nTemp);				//	Haze효과 사용유무
	m_bUseHazeEffect = nTemp > 0 ? true : false;

	for(INT32 i=1; i<MAX_FIRE_EFFECT; i++)
	{
		char tmp[256] = "";
		i3::snprintf( tmp, 256, "_FireEffect%d", i+1);
		FIND_REG_DATA( pEffRegKey, tmp,					m_strFireEffect[i] );				//	발사 이펙트
	}

	// 히트 이펙트
	FIND_REG_DATA( pEffRegKey, "ObjHitEffect",			m_strObjHitEffect );
	FIND_REG_DATA( pEffRegKey, "CharaHitEffect",		m_strCharaHitEffect );
	FIND_REG_DATA( pEffRegKey, "CharaHitEffect_1PV",	m_strCharaHitEffect1PV );
	FIND_REG_DATA( pEffRegKey, "IdleEffect",			m_strIdleEffect);

	nTemp = 0;
	FIND_REG_DATA( pEffRegKey, "UnBlowEffect_Decal",	&nTemp );//GD 요청에의한 ->UnBlowEffect_Decal 엘리먼트 명칭 변경
	m_UnBlowEffFlag = (UINT16)nTemp;
	FIND_REG_DATA( pEffRegKey, "UnBlowEffect_BulletMark",&nTemp );//GD 요청에의한 ->UnBlowEffect_BulletMark 엘리먼트 명칭 변경
	m_UnDecalEffFlag = (UINT16)nTemp;
	FIND_REG_DATA( pEffRegKey, "BulletMarkExt",			m_strDecalEffectExt );
	FIND_REG_DATA(pEffRegKey, "UnColorEffectFlag", &nTemp);
	m_UnColorEffectFlag = (UINT16)nTemp;

	REAL32 rTemp = 0.0f;
	FIND_REG_DATA(pEffRegKey, "IdleEffectLoopTime",		&rTemp);
	m_rIdleEffectLoopTime = rTemp;
}

namespace {
	i3::unordered_map<i3::string, WEAPON_ATTRIBUTE>	s_WeaponAttributeInfoList;

	void InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE attr, const char * pszFieldName)
	{
		s_WeaponAttributeInfoList.insert( i3::unordered_map<i3::string, WEAPON_ATTRIBUTE>::value_type(pszFieldName, attr));
	}

	WEAPON_ATTRIBUTE	GetWeaponAttribute( const char * pszFieldName)
	{
		i3::unordered_map<i3::string, WEAPON_ATTRIBUTE>::iterator it = s_WeaponAttributeInfoList.find( pszFieldName);
		if( it != s_WeaponAttributeInfoList.end())
			return it->second;

		return 0;
	}
};

void CWeaponInfo::InitializeAttributeKey( void)
{
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_FASTZOOM,					"FastZoom");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_USINGPAIRWEAPON,			"UsePairData");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_ATTACHEDSUB,				"AttachedSubWeapon");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_ALPHABLEND,				"TurnOnAlphaBlend");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_USINGDESCRIPTION,			"DescriptionImage");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_PUMPACTION,				"PumpAction");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_FASTARELOAD,				"FastReloadAnimation");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_SHOTGUNMASTERC,			"ShotgunMasterC");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_LOADBULLETLEFTBARREL,		"LoadBulletLeftBarrel");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_LOADMAGLEFTBARREL,		"LoadMagazineLeftBarrel");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_LOADMAGREADY,				"LoadMagazineReady");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_CIC,						"CIC");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_LOCKEDDOTLASER,			"LockedDotLaser");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_NEEDTOLOADBULLET,			"NeedToLoadBullet");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_THROW_WEAPON,				"IsThrowWeapon");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_MODE_WEAPON,				"ModeWeapon");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_EXCEPTION_ENDURANCE,		"ExceptionEndurance");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_WORLD_WEAPON,				"IsWorldWeapon");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_NO_EXJECTSHELL,			"NoExjectShell");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_NOTOFF_EXTENSION_AT_CHANGE, "NoOffExtAtChange");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_DUAL_WEAPON,				"IsDualWeapon");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_HITED_ANIMATION,			"UseHitedAnimation");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_BOUNCED_ATTACK,			"EnableBounce");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_DELAYED_FIRE_SOUND,		"DelayedFireSound");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_NOMORE_SECONDARY_SOUND,	"NoMoreSecSound");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_DO_NOT_THROW_LAST,		"DoNotThrowLast");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_SPRAY_SECONDARYATTACK,	"SpraySecAttack");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_DELAYED_THROWING,			"IsDelayedThrowing");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_USE_BOMBTRIGGER,			"UseBombTrigger");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_LOADMAG_TO_LOADBULLET,	"LoadMagToLoadBullet");
	::InsertWeaponAttributeInfo( WEAPON_ATTRIBUTE_RELOAD_LOOP_ANIMATION,	"ReloadLoopAnimation");
}

bool CWeaponInfo::_FindAttributeKey( i3RegKey * pKey, const char * pszFieldName)
{
	BOOL bTemp = FALSE;
	FIND_REG_DATA( pKey, pszFieldName, &bTemp);
	if( bTemp)
	{
		WEAPON_ATTRIBUTE attr = ::GetWeaponAttribute( pszFieldName);
		if( attr > 0)
		{
			m_Attribute |= attr;
			return true;
		}
	}

	return false;
}

void CWeaponInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	I3ASSERT( pKey != nullptr);

	BOOL	bTemp = FALSE;
	INT32	nTemp = 0;

	i3::rc_wstring wstrResName;
	i3::rc_wstring wstrChangeI3SName;

	FIND_REG_DATA( pKey, "_ResName", wstrResName );
	FIND_REG_DATA( pKey, "_ResName_I3S", wstrChangeI3SName);

	INT32 len = wstrResName.size();

	if( len > 0)
	{
		i3::rc_string resName;

		if (!wstrChangeI3SName.empty())
		{
			bool is_success = i3CharaSGChangeManager::i()->AddData(wstrResName, wstrChangeI3SName, resName );
		
			if (!is_success)
			{
				I3ASSERT( is_success );
				i3::utf16_to_mb(wstrResName, resName);
			}
		}
		else
		{
			i3::utf16_to_mb(wstrResName, resName);
		}

		GlobalFunc::EncryptValue( resName, ENC_SHIFT_WEAPONINFO_RESNAME);
		SetName( resName);
	}
	
	FIND_REG_DATA( pKey, "_Name", m_strWeaponName );

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("_NumberReal"), &nTemp);
	m_nRealNumber = (UINT32)nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("ITEMID"), &nTemp);			//	아이템아이디
	m_nItemID = (UINT32)nTemp;

	// RealNumber가 없으면 _Number가 RealNumber입니다.
	if ( m_nRealNumber == 0 )	
		m_nRealNumber = WEAPON::ItemID2ResID( m_nItemID);

	_FindAttributeKey( pKey, "UsePairData");

	i3RegKey* pEffRegKey = Config::FindKey( "Effect", pKey); // 2015. 01. 13 EFFECT  - 노현서
	if( pEffRegKey != nullptr)
		_ReadEffectKey( pKey, pEffRegKey);
	
	FIND_REG_DATA( pKey, "_FireSound",				m_strFireSound );				//	발사 사운드	
	FIND_REG_DATA( pKey, "_FireSoundExt",			m_strFireSoundExt );
	FIND_REG_DATA( pKey, "_SwapSound",				m_strSwapSound );				//	교체 사운드
	FIND_REG_DATA( pKey, "_IdleSound",				m_strIdleSound);				//	Idle Sound

	FIND_REG_DATA( pKey, "LinkedToCharaAI",			m_strLinkedToCharaAI);
	FIND_REG_DATA( pKey, "ClassMeta",				m_strClassMetaName);
	
	nTemp = 0;
	FIND_REG_DATA( pKey, "_FireType", &nTemp);			//	공격 방식
	m_TypeFire = (WEAPON::FIRE_TYPE)nTemp;
	
	nTemp = 0;
	FIND_REG_DATA( pKey,  "_TriggerType", &nTemp);		//	충돌 방식
	m_TypeTrigger = (WEAPON::TRIGGER_TYPE)nTemp;

	FIND_REG_DATA( pKey, "Weight",					&m_rWeight);					//	무기 무게	

	REAL32 rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("FireDelay"),				&rTemp);					// 발사 간격
	m_rFireDelay = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("FireDelay2"),				&rTemp);				// 발사 간격
	m_rFireDelay2 = rTemp;

	FIND_REG_DATA( pKey, "Value",					&m_rValue);					// 무기의 값어치
	
	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Range"),					&rTemp);					//  유효 사정거리 : 무기를 맞았을때 타격을 입힐수 있는거리
	m_rRange = rTemp;
	
	FIND_REG_DATA( pKey, "UiPath",					m_strUiPath);
	FIND_REG_DATA( pKey, "_UIShapeIndex",           &m_nUIShapeIndex);
	
	i3::rc_wstring temp_string;
	FIND_REG_DATA( pKey, "_SpecialDamageHud",		temp_string );
	if( temp_string.size() > 0 )
	{
		m_nSpecialDamageHUDIndex = g_pWeaponInfoDataBase->InsertSpecialDamageHUDTexturePath( temp_string );
	}

	i3::rc_wstring die_string;
	FIND_REG_DATA(pKey, "_SpecialDieHud", die_string);
	if (die_string.size() > 0)
	{
		m_nSpecialDieHUDIndex = g_pWeaponInfoDataBase->InsertSpecialDieHUDTexturePath(die_string);
	}

	die_string.clear();
	FIND_REG_DATA(pKey, "KillSound", die_string);
	if (die_string.size() > 0)
	{
		m_KillSoundIndex = g_pWeaponInfoDataBase->InsertWeaponKillSound(die_string);
	}

	bTemp = FALSE;
	FIND_REG_DATA( pKey, "AttachableItem",			&bTemp);
	if( bTemp)
	{
		WEAPON::addSettableItem( WEAPON::ItemID2ResID( m_nItemID));
	}

	// 상세설명 이미지
	if( _FindAttributeKey( pKey, "DescriptionImage"))
	{
		GetKeyDataString( pKey, "ExtensionName",		m_strExtName);		// 익스텐션 명
		GetKeyDataString( pKey, "ExtensionFeatures",	m_strExtFeatures);	// 익스텐션 기능
	}
	GetKeyDataString( pKey, "Description", m_strDescription);	// 무기 설명

	_FindAttributeKey( pKey, LS_STR("FastZoom"));

	FIND_REG_DATA( pKey, "_QPS_Use",				m_strQuestPostfix_Use );		//Quest에서 사용되는 접미어, 로/으로 구분문자열. 예) mp7으로, k-2로 적을 사살하라.
	FIND_REG_DATA( pKey, "_QPS_Do",					m_strQuestPostfix_Do ); 		//Quest에서 사용된느 접미어, 을/를 구분 문자열. 예) c4를, m4a1을 바닥에 버려라!!
	FIND_REG_DATA( pKey, "PhysixForceRate",			&m_rPhysixForceRate);

	_FindAttributeKey( pKey, "AttachedSubWeapon");
	
	// 수류탄
	{
		i3::rc_wstring	wstrIndicator;
		FIND_REG_DATA( pKey, "IndicatorType", wstrIndicator );

		if( i3::generic_is_iequal(wstrIndicator, L"DefaultGrenade") )
		{
			m_nIndicatorType = WEAPON_INDICATOR_GRENADE;
		}
		else if( i3::generic_is_iequal(wstrIndicator, L"C5") )
		{
			m_nIndicatorType = WEAPON_INDICATOR_C5;
		}
		else
		{
			m_nIndicatorType = WEAPON_INDICATOR_UNKNOWN;
		}

		FIND_REG_DATA( pKey, "IndicatorDistance", &m_rIndicateDistance);
	}

	{
		FIND_REG_DATA( pKey, "TracerType", (INT32*) &m_TracerType);
	}
	
	_FindAttributeKey( pKey, "TurnOnAlphaBlend");

	// Lock Dotlaser
	_FindAttributeKey( pKey, "LockedDotLaser");

	
	i3RegKey * pGatchaKey = Config::FindKey( "Gatcha2", pKey);
	if( pGatchaKey != nullptr)
		__ReadGatchaKey( pGatchaKey);
		
	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("OriginalWeaponID"),	&nTemp);
	if(nTemp > 0)
		m_WeaponOriginalItemID = (UINT32)nTemp;
	
	nTemp = 0;
	FIND_REG_DATA(pKey, "IsLimitedByDesigSlot", &nTemp);
	m_IsLimitedByDesigSlot = (nTemp > 0) ? 1 : 0;

	// revision 61755
	// KEYWORD : ROTATE_GRENADE
	FIND_REG_DATA( pKey, "AttachedWall_RotateX",				&m_RotateAngleTable[0]);				// C5, MedicalKit의 모델링 회전을 위한 값 -> 벽에 붙었을때 X축 회전각도 ( 0 = 0 , 1 = I3_PI2 , 2 = I3_PI , 3 = -I3_PI2 )
	FIND_REG_DATA( pKey, "AttachedWall_RotateY",				&m_RotateAngleTable[1]);				// 벽에 붙었을때 Y축 회전각도
	FIND_REG_DATA( pKey, "AttachedFloor_RotateX",				&m_RotateAngleTable[2]);				// 바닥에 붙었을때 X축 회전각도
	FIND_REG_DATA( pKey, "AttachedFloor_RotateY",				&m_RotateAngleTable[3]);	

	FIND_REG_DATA( pKey, "Level", &m_nLevel);
	FIND_REG_DATA( pKey, "MaxLevel", &m_nMaxLevel);

	_FindAttributeKey( pKey, "IsThrowWeapon");
	_FindAttributeKey( pKey, "ExceptionEndurance");
	_FindAttributeKey( pKey, "IsWorldWeapon");
	_FindAttributeKey( pKey, "NoExjectShell");
	_FindAttributeKey( pKey, "NoOffExtAtChange");

	FIND_REG_DATA( pKey, "TerrainHitSound",			&m_TerrainHitSoundIndex);
	FIND_REG_DATA( pKey, "CharaHitSound",			&m_CharaHitSoundIndex);
	FIND_REG_DATA( pKey, "CharaHitSound2",			&m_CharaHitSoundIndex2);

	_FindAttributeKey( pKey, "ModeWeapon");
	_FindAttributeKey( pKey, "IsDelayedThrowing");

	GlobalFunc::VAR_FIND_REG_DATA(pKey, LS_STR("CrouchParalysis"), m_rCrouchParalysis);				// 마비시간

	i3RegKey * pChildKey = Config::FindKey(LS_STR("Damage"), pKey);
	if( pChildKey != nullptr)
		_ReadDamageKey( pChildKey);

	pChildKey = Config::FindKey(LS_STR("Movement"), pKey);
	if( pChildKey != nullptr)
		_ReadMovementKey( pChildKey);

	// Extension
	for( INT32 i=0; i<MAX_WEAPON_EXTENSION; i++)
	{
		char szKey[256] = "";
		sprintf_s( szKey, LS_STR("Extension%d"), i+1);

		pChildKey = Config::FindKey( szKey, pKey);
		if( pChildKey != nullptr)
		{
			_ReadExtensionKey( pChildKey, i);
		}
	}

	pChildKey = Config::FindKey( "Shop3DView", pKey);
	if( pChildKey != nullptr)
		ReadShop3DViewKey( pChildKey);
	
	//	code에 weapon class meta가 등록됬는지 사전에 검사합니다. 
	//	등록되지 않을 경우 무기 생성시 크래시하므로 반드시 무기 클래스가 RegisterMeta되어야 합니다.
	if( i3::generic_string_size( m_strClassMetaName) > 0)
	{
		i3::string strMeta;	i3::utf16_to_mb(m_strClassMetaName, strMeta);

		i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(strMeta);  // i3ClassMeta::FindClassMetaByName( m_szClassMetaName);
		if( pMeta == nullptr)
		{
			char message[MAX_STRING_COUNT] = "";
			i3::snprintf( message, MAX_STRING_COUNT, 
							"[%s] 해당 무기의 class meta를 찾을수 없거나 등록되지 않았습니다. 다시 확인해 주십시오.\n", 
							strMeta);
//			I3PRINTLOG(I3LOG_FATAL, message); 
		}
	}

// issue : BUG_PB #8765
// 읽어들인 데이터 타입 유효성 검사. 2011-09-20. ugeun.ji 
#if defined( I3_DEBUG )
	CheckRegDataType();
#endif

#if defined( FIREDELAY_COST)
	// 60fps에 맞춰진 정확한 시간을 계산하기 위해 오차값을 더한다.
	REAL32 rRemain;

	if( m_rFireDelay > 0.f)
	{
		m_rFireDelay = m_rFireDelay + 0.00001f;
		rRemain = i3Math::fmod( m_rFireDelay, 0.016f);

		if( rRemain > 0.f)
		{
			rRemain = 0.016f - rRemain;
			m_rFireDelay = m_rFireDelay + rRemain;
		}
	}

	if( m_rFireDelay2 > 0.f)
	{
		m_rFireDelay2 = m_rFireDelay2 + 0.00001f;
		rRemain = i3Math::fmod( m_rFireDelay2, 0.016f);

		if( rRemain > 0.f)
		{
			rRemain = 0.016f - rRemain;
			m_rFireDelay2 = m_rFireDelay2 + rRemain;
		}
	}
#endif
}

#if defined( I3_DEBUG )
void CWeaponInfo::CheckRegDataType()
{	
	I3ASSERT( WEAPON::FIRE_UNKNOWN <= m_TypeFire && m_TypeFire < WEAPON::FIRE_COUNT );
	I3ASSERT( WEAPON::TRIGGER_UNKNOWN <= m_TypeTrigger && m_TypeTrigger < WEAPON::TRIGGER_COUNT );
	I3ASSERT( WEAPON::DAMAGE_UNKNOWN <= m_TypeDamage && m_TypeDamage < WEAPON::DAMAGE_COUNT );
}
#endif


void CWeaponInfo::OnEncriptInfo( UINT8 u8Key)
{
}

bool	CWeaponInfo::IsLimitedByDesigSlot() const
{
	INT32 resVal = m_IsLimitedByDesigSlot;

	I3ASSERT(resVal == 0 || resVal == 1);

	return (resVal != 0);
}


T_ItemID CWeaponInfo::GetItemID() const
{
	return m_nItemID;
}


WEAPON::RES_ID CWeaponInfo::GetNumber() const
{
	return WEAPON::ItemID2ResID( GetItemID());
}

WEAPON::RES_ID CWeaponInfo::GetRealNumber() const
{
	return m_nRealNumber;
}

void CWeaponInfo::GetWeaponName(  i3::rc_wstring& strOut) const
{
	strOut = m_strWeaponName;
}

void CWeaponInfo::GetFireEffect( i3::rc_wstring& strOut, INT32 idx /*= 0*/) const
{
	strOut = m_strFireEffect[idx];
}

void CWeaponInfo::GetObjHitEffect( i3::rc_wstring& strOut ) const
{
	strOut = m_strObjHitEffect;
}

void CWeaponInfo::GetCharaHitEffect( i3::rc_wstring& strOut ) const
{
	strOut = m_strCharaHitEffect;
}

void CWeaponInfo::GetCharaHitEffect1PV( i3::rc_wstring& strOut ) const
{
	strOut = m_strCharaHitEffect1PV;
}

void CWeaponInfo::GetDecalEffectExt( i3::rc_wstring& strOut ) const
{
	strOut = m_strDecalEffectExt;
}

UINT16 CWeaponInfo::GetUnDecalEffFlag() const
{
	return m_UnDecalEffFlag;
}

UINT16 CWeaponInfo::GetUnBlowEffFlag() const
{
	return m_UnBlowEffFlag;
}

UINT16 CWeaponInfo::GetUnColorEffectFlag() const
{
	return m_UnColorEffectFlag;
}

void CWeaponInfo::GetIdleEffect(i3::rc_wstring& strOut) const
{
	GlobalFunc::DecryptValue(strOut, m_strIdleEffect, m_u8EncryptKey[VALUETYPE_STR]);
}

REAL32 CWeaponInfo::GetIdleEffectLoopTime() const
{
	return GlobalFunc::DecryptValue(&m_rIdleEffectLoopTime, m_u8EncryptKey[VALUETYPE_REAL]);
}

void CWeaponInfo::GetFireSound( i3::rc_wstring& strOut) const
{
	strOut = m_strFireSound;
}

void CWeaponInfo::GetSwapSound( i3::rc_wstring& strOut) const
{
	strOut = m_strSwapSound;
}

void CWeaponInfo::GetIdleSound(i3::rc_wstring & strOut) const
{
	strOut = m_strIdleSound;
}

void CWeaponInfo::GetLinkedToCharaAI( i3::rc_wstring& strOut) const
{
	strOut = m_strLinkedToCharaAI;
}

void CWeaponInfo::GetFireEffectExt( i3::rc_wstring& strOut) const
{
	strOut = m_strFireEffectExt;
}

void CWeaponInfo::GetPosExtFireEffectOffSet( VEC3D* vecOut )
{
	i3Vector::Copy( vecOut, &m_vPosExtFireEffectOffset);
}

void CWeaponInfo::GetFireSoundExt( i3::rc_wstring& strOut) const
{
	strOut = m_strFireSoundExt;
}

void CWeaponInfo::GetClassMetaName( i3::rc_wstring& strOut) const
{
	strOut = m_strClassMetaName;
}

WEAPON_SLOT_TYPE CWeaponInfo::GetTypeUsage() const
{
	return (WEAPON_SLOT_TYPE)WEAPON::ItemID2Slot(GetItemID());
}

WEAPON_CLASS_TYPE CWeaponInfo::GetTypeClass() const
{
	return WEAPON::ItemID2Class( GetItemID());
}

WEAPON::FIRE_TYPE CWeaponInfo::GetTypeFire() const
{
	return m_TypeFire;
}

WEAPON::TRIGGER_TYPE CWeaponInfo::GetTypeTrigger() const
{
	return m_TypeTrigger;
}

WEAPON::DAMAGE_TYPE CWeaponInfo::GetTypeDamage() const
{
	return m_TypeDamage;
}

INT32 CWeaponInfo::GetMaxDamage(void) const
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{
		return m_nMaxDamageForDino;
	}
	else
	{
		return m_nMaxDamage;
	}
}

INT32 CWeaponInfo::GetMaxDamageForDino(void) const
{
	return m_nMaxDamageForDino;
}

INT32 CWeaponInfo::GetCriticalDamageChance() const
{
	return m_nCriticalDamageChance;
}

REAL32 CWeaponInfo::GetCriticalDamageRate() const
{
	return m_fCriticalDamageRate;
}

REAL32 CWeaponInfo::GetWeaponScale(void) const
{
	return m_rScaleRate;
}

REAL32 CWeaponInfo::GetWeaponXRotate(void) const
{
	return m_rXAxis;
}

REAL32 CWeaponInfo::GetWeaponYRotate(void) const
{
	return m_rYAxis;
}

REAL32 CWeaponInfo::GetWeaponZRotate(void) const
{
	return m_rZAxis;
}

UINT32 CWeaponInfo::GetOriginalWeaponItemID(void) const
{
	return m_WeaponOriginalItemID;
}

REAL32 CWeaponInfo::GetWeaponBrightness(void) const
{
	return m_rBrightness;
}

REAL32 CWeaponInfo::GetWeight() const
{
	return m_rWeight;
}

REAL32 CWeaponInfo::GetRange() const
{
	return m_rRange;
}

REAL32 CWeaponInfo::GetFireDelay() const
{
	return m_rFireDelay;
}

REAL32 CWeaponInfo::GetFireDelay2() const
{
	return m_rFireDelay2;
}

REAL32 CWeaponInfo::GetMoveNormalFire() const
{
	return m_rMoveNormalFire;
}

REAL32 CWeaponInfo::GetMoveNormalSit() const
{
	return m_rMoveNormalSit;
}

REAL32 CWeaponInfo::GetMoveNormalStand() const
{
	return m_rMoveNormalStand;
}


void CWeaponInfo::GetUiPath( i3::rc_wstring& strOut) const
{
	strOut = m_strUiPath;
}

INT32 CWeaponInfo::GetUIShapeIndex() const
{
	return m_nUIShapeIndex;
}

REAL32 CWeaponInfo::GetValue() const
{
	return m_rValue;
}

void CWeaponInfo::GetDescription( i3::rc_wstring& strOut) const
{
	strOut = m_strDescription;
}

WEAPON_ATTRIBUTE CWeaponInfo::_getAttribute( void) const
{
	return (WEAPON_ATTRIBUTE) m_Attribute;
}

bool CWeaponInfo::isDescriptionImage() const
{
	return (_getAttribute() & WEAPON_ATTRIBUTE_USINGDESCRIPTION) == WEAPON_ATTRIBUTE_USINGDESCRIPTION;
}

REAL32 CWeaponInfo::GetHelmetPenetrate() const
{
	return m_rHelmetPenetrate;
}

REAL32 CWeaponInfo::GetHelmetPenetrate_Str() const
{
	return m_rHelmetPenetrate_Str;
}



void CWeaponInfo::GetQuestPostfix_Use( i3::rc_wstring& strOut) const
{
	strOut = m_strQuestPostfix_Use;
}

void CWeaponInfo::GetQuestPostfix_Do( i3::rc_wstring& strOut) const
{
	strOut = m_strQuestPostfix_Do;
}



bool CWeaponInfo::AttachedSubWeapon()
{
	return (_getAttribute() & WEAPON_ATTRIBUTE_ATTACHEDSUB) == WEAPON_ATTRIBUTE_ATTACHEDSUB;
}

/*virtual*/ INT32 CWeaponInfo::GetDamage() const
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{
		return m_nDamageForDino;
	}
	else
	{
		return m_nDamage;
	}
}

/*virtual*/ REAL32 CWeaponInfo::GetDamageOnlyUI() const
{
	return m_nDamageOnlyUI;
}

/*virtual*/ INT32 CWeaponInfo::GetDamageForDino() const
{
	return m_nDamageForDino;
}

T_ItemID CWeaponInfo::GetLeftHandWeaponItemID( void) const
{
	T_ItemID itemID = GetSubExtensionItemID(WEAPON::EXT_DUAL_LEFT);
	if( itemID == 0)
		itemID = GetSubExtensionItemID(WEAPON::EXT_DUAL_LEFT_DIF);

	return itemID;
}

void CWeaponInfo::SetCompatibleItemID(INT32 idx, T_ItemID nItemID, bool bUseExtShape)
{
	I3_BOUNDCHK( idx, MAX_WEAPON_EXTENSION);
	m_stCompatibleInfo[idx].SetCompatibleInfo(nItemID, bUseExtShape);

	//I3TRACE("%d - PairWeapon ItemID [%d] = %d\n", GetItemID(), idx, m_stCompatibleInfo[idx]._nItemID);
}

T_ItemID CWeaponInfo::GetCompatibleItemID(INT32 idx) const
{
	I3_BOUNDCHK( idx, MAX_WEAPON_EXTENSION);

	return m_stCompatibleInfo[idx]._nItemID;
}

INT32 CWeaponInfo::GetCompatibleWeaponIdx(T_ItemID nItemID) const
{
	for(int i = 0 ; i < MAX_WEAPON_EXTENSION ; ++i)
	{
		if( GetCompatibleItemID(i) == nItemID)
			return i;
	}

	return -1;
}

REAL32 CWeaponInfo::GetPhysixForceRate() const
{
	return m_rPhysixForceRate;
}


bool CWeaponInfo::isBombTypeClass() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();
	return ( (type == WEAPON_CLASS_THROWING_GRENADE) || (type == WEAPON_CLASS_MISSION) || (type == WEAPON_CLASS_GRENADESHELL) );
}

bool CWeaponInfo::isUseExplosionRange() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();
	return (type == WEAPON_CLASS_THROWING_GRENADE || type == WEAPON_CLASS_THROWING_CONTAIN || type == WEAPON_CLASS_THROWING_ATTACK || 
			type == WEAPON_CLASS_THROWING_HEAL || type == WEAPON_CLASS_MISSION || type == WEAPON_CLASS_GRENADESHELL);
}
bool CWeaponInfo::hasAction() const
{
	WEAPON::FIRE_TYPE type = GetTypeFire();
	return (type == WEAPON::FIRE_MELEE || type == WEAPON::FIRE_THROWING);
}

bool CWeaponInfo::isRange() const
{
	WEAPON_SLOT_TYPE type = GetTypeUsage();
	return (type == WEAPON_SLOT_PRIMARY || type == WEAPON_SLOT_SECONDARY);
}

bool CWeaponInfo::isMelee() const
{
	WEAPON_SLOT_TYPE type = GetTypeUsage();
	return (type == WEAPON_SLOT_MELEE);
}

bool CWeaponInfo::isThrowing() const
{
	WEAPON_SLOT_TYPE type = GetTypeUsage();
	return (type == WEAPON_SLOT_THROWING1);
}

bool CWeaponInfo::isItem() const
{
	WEAPON_SLOT_TYPE type = GetTypeUsage();
	return (type == WEAPON_SLOT_THROWING2);
}

bool CWeaponInfo::isThrowingClass() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();
	return (type == WEAPON_CLASS_THROWING_GRENADE) || (type == WEAPON_CLASS_GRENADESHELL);
}

bool CWeaponInfo::isMission() const
{
	WEAPON_SLOT_TYPE type = GetTypeUsage();
	return (type == WEAPON_SLOT_MISSION);
}

bool CWeaponInfo::isNotMission() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();
	return (WEAPON_CLASS_MISSION != type);
}

bool CWeaponInfo::isEnableHeadShot() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();

	bool effectEnable = false;
	// revision 52958 공룡 -> 인간 헤드샷을로 킬 시 UI 나오도록 수정.
	if ((WEAPON_CLASS_THROWING_GRENADE != type) && (WEAPON_CLASS_GRENADESHELL != type) && (WEAPON_CLASS_MISSION != type))
		effectEnable = true;

	return effectEnable;
}

bool CWeaponInfo::isEnableCrossHairEffect() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();
	return (type != WEAPON_CLASS_THROWING_GRENADE && type != WEAPON_CLASS_SNIPER && 
			type != WEAPON_CLASS_THROWING_CONTAIN && type != WEAPON_CLASS_THROWING_ATTACK && type != WEAPON_CLASS_THROWING_HEAL);
}
bool CWeaponInfo::isEnableDrop() const
{
	WEAPON_SLOT_TYPE type = GetTypeUsage();
	return (type == WEAPON_SLOT_PRIMARY || type == WEAPON_SLOT_SECONDARY);
}

bool CWeaponInfo::isNotNeedContactBox() const
{
	WEAPON_SLOT_TYPE type = GetTypeUsage();
	return (type == WEAPON_SLOT_MELEE || type == WEAPON_SLOT_THROWING1 || type == WEAPON_SLOT_THROWING2 || type == WEAPON_SLOT_MISSION);
}

bool CWeaponInfo::isNotPenetrate() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();
	return (type == WEAPON_CLASS_KNIFE || type == WEAPON_CLASS_KNUCKLE || type == WEAPON_CLASS_THROWING_GRENADE || 
			type == WEAPON_CLASS_MISSION || type == WEAPON_CLASS_THROWING_CONTAIN || type == WEAPON_CLASS_THROWING_ATTACK || type == WEAPON_CLASS_THROWING_HEAL);
}

bool CWeaponInfo::isExplosionPhysics() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();
	return (type == WEAPON_CLASS_THROWING_GRENADE || type == WEAPON_CLASS_MISSION);
}

bool CWeaponInfo::isValidUiPath() const
{
	i3::rc_wstring wstrPath;
	GetUiPath( wstrPath);
	return !wstrPath.empty();	
}

// 무조건 자연 칼라가 설정되는 무기
bool CWeaponInfo::isUnconditionalNeutralColorCrossHair() const
{ 
	WEAPON_CLASS_TYPE type = GetTypeClass();
	return (type == WEAPON_CLASS_THROWING_GRENADE || type == WEAPON_CLASS_THROWING_CONTAIN || type == WEAPON_CLASS_THROWING_ATTACK
			|| type == WEAPON_CLASS_THROWING_HEAL || type == WEAPON_CLASS_MISSION || type == WEAPON_CLASS_GRENADESHELL);
}

bool CWeaponInfo::isUnableSwapIfZero() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();
	return (type == WEAPON_CLASS_THROWING_GRENADE || type == WEAPON_CLASS_THROWING_CONTAIN || type == WEAPON_CLASS_THROWING_ATTACK
			|| type == WEAPON_CLASS_THROWING_HEAL || type == WEAPON_CLASS_MISSION);
}

bool CWeaponInfo::isGun() const
{
	WEAPON::FIRE_TYPE type = GetTypeFire();
	return (type == WEAPON::FIRE_RANGE);
}


bool CWeaponInfo::isCompoundBow() const
{
	return isSame( WEAPON_SUBCLASS_BOW, WEAPON::getItemIndex(WEAPON::SUB_BOW_COMPOUND_BLUE));
}


bool CWeaponInfo::isC4() const
{
	WEAPON_CLASS_TYPE type = GetTypeClass();
	if( type == WEAPON_CLASS_MISSION)
	{
		WEAPON::RES_ID num = GetNumber();
		return (num == WEAPON::getItemIndex(WEAPON::C4) || num == WEAPON::getItemIndex( WEAPON::SHUTTLEX));
	}
	return false;
}

bool CWeaponInfo::isSame(WEAPON_CLASS_TYPE type, WEAPON::RES_ID nNumber) const
{
	WEAPON_CLASS_TYPE ct = GetTypeClass();
	if( ct == type)
	{
		WEAPON::RES_ID num = GetNumber();
		return (num == nNumber);
	}
	return false;
}

bool CWeaponInfo::isOverFireDelay(REAL32 rDelay) const
{
	return (rDelay >= GetFireDelay());
}

bool CWeaponInfo::hasExtensionInfo() const
{
	return (GetSubExtensionItemID() > 0)?true:false;
}

bool CWeaponInfo::hasFireEffect() const
{
	i3::rc_wstring wstrTemp;
	GetFireEffect( wstrTemp);
	return !wstrTemp.empty();	
}

bool CWeaponInfo::hasFireEffectExt() const
{
	i3::rc_wstring wstrTemp;
	GetFireEffectExt( wstrTemp);
	return !wstrTemp.empty();	
}

bool CWeaponInfo::hasIdleEffect() const
{
	i3::rc_wstring wstrTemp;
	GetIdleEffect(wstrTemp);
	return !wstrTemp.empty();
}

WEAPON_INDICATOR_TYPE CWeaponInfo::GetIndicatorType(void) const
{
	return m_nIndicatorType;
}

REAL32 CWeaponInfo::GetIndicateDistance( void) const
{
	return m_rIndicateDistance;
}

REAL32 CWeaponInfo::GetJumpParalysis() const
{
	return m_rJumpParalysis;
}

REAL32 CWeaponInfo::GetWalkParalysis() const
{
	return m_rWalkParalysis;
}

REAL32 CWeaponInfo::GetRunParalysis() const
{
	return m_rRunParalysis;
}

REAL32 CWeaponInfo::GetCrouchParalysis() const
{
	return m_rCrouchParalysis;
}

/*virtual*/ WEAPON::ATTACK_TYPE CWeaponInfo::GetAttackType(WEAPON::FIRE_ORDER fireOrder, INT32 i32FireCount) const
{
	if(fireOrder == WEAPON::SECONDARY_FIRE)
	{
		for(INT32 idx = 0 ; idx < MAX_WEAPON_EXTENSION ; ++idx)
		{
			WEAPON::EXT_TYPE extType = getExtensionType( idx);

			if(extType == WEAPON::EXT_BAYONET || extType == WEAPON::EXT_DIRECT_MELEE)
			{
				return (WEAPON::ATTACK_TYPE)(WEAPON::ATTACK_TYPE_R1 + i32FireCount);
			}
		}

		return WEAPON::ATTACK_TYPE_R1; 
	}
	else
		return WEAPON::ATTACK_TYPE_L1; 
}


// KEYWORD : ROTATE_GRENADE
REAL32 CWeaponInfo::GetRotateAngle( INT32 nIndex ) const // [0]벽X , [1]벽Y , [2]바닥X , [3]바닥Y
{
	REAL32 Val = 0;

	I3TRACE("[## ROTATE_TEST ##] Index : %d\n", nIndex);
	I3ASSERT ( 0 <= nIndex && nIndex < 4 );

	switch ( m_RotateAngleTable [nIndex] )
	{
	case 0: Val = 0;			break;
	case 1:	Val = I3_PI4;		break;	// pef에 세팅된 값대로 회전시킨다
	case 2:	Val = I3_PI2;		break;
	case 3: Val = I3_PI4 * 3;	break;
	case 4: Val = I3_PI;		break;	// 모델링 축 4방향으로 작업됬던 사항 8방향으로 수정하여 회전값 지정 가능하도록 수정
	case 5: Val = -I3_PI4 * 3;	break;
	case 6: Val = -I3_PI2;		break;
	case 7: Val = -I3_PI4;		break;
	}
	
	return Val;
}


INT32 CWeaponInfo::GetPercentage( WEAPON::INFO_TYPE nWeaponInfoType) const
{
	// 계산식	
	INT32 nPercentage = 0;

	switch( nWeaponInfoType)
	{
	case WEAPON::INFO_DAMAGE:		
		{
			//	일부 투척은 데미지 없음
			if( GetTypeClass() == WEAPON_CLASS_THROWING_CONTAIN)	return -1;

			nPercentage = GetDamage();	
		}
		break;
	case WEAPON::INFO_HIT_RATE:		
		{
			if( WEAPON::FIRE_MELEE == GetTypeFire() ) 		return 100;		//	근접 무기는 100%
			else return -1;
		}
		break;
	case WEAPON::INFO_SPEED:			nPercentage = INT32(GetMoveNormalStand());	break;	//	이동속도
	
	default:		return -1;
	}
	
	// 범위 제한 0 ~ 100
	if( 100 < nPercentage ) nPercentage = 100;
	if( 0 > nPercentage ) nPercentage = 0;

	return nPercentage;
}

void CWeaponInfo::MakeBulletCountString(i3::stack_wstring& out_wstrBulletCount)
{
	this->FillBulletCountString(out_wstrBulletCount);
}

void CWeaponInfo::MakeWeaponRangeString(i3::stack_wstring& out_wstrRange)
{

	switch(GetTypeFire())
	{
	case WEAPON::FIRE_THROWING:	
		{
			out_wstrRange = L"-";	
		}
		break;	
	default:
		{
			i3::sprintf( out_wstrRange, L"%d", (INT32) GetRange());
		}
		break;
	}
}

void CWeaponInfo::FillBulletCountString(i3::stack_wstring& out_wstrBulletCount)
{
	out_wstrBulletCount = L"-";
}

/*virtual*/ void CWeaponInfo::GetDamageInfo(CHARA_DAMAGEINFO* pDamageInfo)
{
	I3ASSERT(pDamageInfo);

	pDamageInfo->_ui32FireWeaponID	= GetItemID();
	pDamageInfo->_DamageType		= GetTypeDamage();
	pDamageInfo->_rEffectiveTime	= GetEffectiveTime();
	pDamageInfo->_rExplosionRange	= GetExplosion_Range();
	pDamageInfo->_rExplosionSlope	= GetExplosion_EffectiveSlope();
}


UINT8 CWeaponInfo::GetWeaponFireState(T_ItemID itemID) const
{
	INT32 i32State = 0;

	if( GetItemID() != itemID )
	{
		i32State = GetCompatibleWeaponIdx( itemID ) + 1;
	}
	
	return (i32State <= 0) ? 0 : ((UINT8)i32State);
}

void CWeaponInfo::SetKillInfo(KILL_INFO* pKillInfo)
{ 
	I3ASSERT(pKillInfo);

	pKillInfo->m_WeaponType = GetTypeClass();
	pKillInfo->m_Weapon = GetNumber();
}

//	하나의 무기내에 여러개의 익스텐션을 가질수 있으므로 기존에 m_pWeaponInfo->getExtensionType() 호출을 자제하세요.
//	무기의 현재 활성화된 익스텐션을 알기위해선 WeaponInfo에 있는 함수보다 무기에 있는 getCurrentExtensionType() 함수 이용하세요. komet
/*virtual*/ WEAPON::EXT_TYPE CWeaponInfo::getExtensionType(INT32 idx) const
{
	I3_BOUNDCHK( idx, MAX_WEAPON_EXTENSION);
	if( m_pExtInfo[ idx] != nullptr)
		return m_pExtInfo[idx]->GetExtType( m_u8EncryptKey[VALUETYPE_INT]);

	return WEAPON::EXT_NONE;
}

T_ItemID CWeaponInfo::GetExtensionItemID( INT32 idx ) const
{	
	I3_BOUNDCHK( idx, MAX_WEAPON_EXTENSION);
	
	if( m_pExtInfo[ idx] != nullptr)
		return m_pExtInfo[idx]->GetItemID(m_u8EncryptKey[VALUETYPE_INT]);

	return 0;
}

bool CWeaponInfo::isUseExtShape(INT32 idx) const
{
	I3_BOUNDCHK( idx, MAX_WEAPON_EXTENSION);

	if( m_pExtInfo[ idx] != nullptr)
		return m_pExtInfo[idx]->IsUseExtShape();

	return false;
}

void CWeaponInfo::GetExtensionName( i3::rc_wstring& out) const
{
	out = m_strExtName;
}

void CWeaponInfo::GetExtensionFeatures( i3::rc_wstring& out) const
{
	out = m_strExtFeatures;
}

T_ItemID CWeaponInfo::GetSubExtensionItemID( WEAPON::EXT_TYPE ExtType ) const
{
	if( ExtType == WEAPON::EXT_NONE )
	{
		for( INT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
		{
			if( getExtensionType( i) != ExtType )
			{
				T_ItemID id = m_pExtInfo[i]->GetItemID( m_u8EncryptKey[VALUETYPE_INT]);
				if( id > 0)
					return id;
			}
		}
	}
	else
	{
		for( INT32 i = 0; i < MAX_WEAPON_EXTENSION; i++)
		{
			if( getExtensionType( i) == ExtType )
			{
				T_ItemID id = m_pExtInfo[i]->GetItemID( m_u8EncryptKey[VALUETYPE_INT]);
				if( id > 0)
					return id;
			}
		}
	}

	return 0;
}

REAL32 CWeaponInfo::GetExtRange( void) const
{
	INT32 i;
	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( getExtensionType( i) & WEAPON_EXT_MASK_ADDABLE_DAMAGE)
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtSubInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtSubInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtSubInfo*)m_pExtInfo[i])->GetExtRange(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 0.f;
}

REAL32 CWeaponInfo::GetExtDamage( void) const
{
	INT32 i;
	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( getExtensionType( i) & WEAPON_EXT_MASK_ADDABLE_DAMAGE)
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtSubInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtSubInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtSubInfo*)m_pExtInfo[i])->GetExtDamage(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 0.f;
}

REAL32 CWeaponInfo::GetExtAttackTime( void) const
{
	INT32 i;
	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( getExtensionType( i) & WEAPON_EXT_MASK_ADDABLE_DAMAGE)
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtSubInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtSubInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtSubInfo*)m_pExtInfo[i])->GetExtAttackTime(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 0.f;
}

REAL32 CWeaponInfo::GetExt_DPD_Rate( void) const
{
	INT32 i;
	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( getExtensionType( i) & WEAPON_EXT_MASK_ADDABLE_DAMAGE)
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtSubInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtSubInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtSubInfo*)m_pExtInfo[i])->GetExt_DPD_Rate(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 0.f;
}


REAL32 CWeaponInfo::GetExtRecoilHorzFactor( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtFireType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtRecoilHorzFactor(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 1.f;
}

INT32 CWeaponInfo::GetExtRecoilHorzCount( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtFireType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtRecoilHorzCount(m_u8EncryptKey[VALUETYPE_INT]);
			}
		}
	}

	return 0;
}

INT32 CWeaponInfo::GetExtOneHorzEnable( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtFireType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtOneHorzEnable(m_u8EncryptKey[VALUETYPE_INT]);
			}
		}
	}

	return 0;
}

INT32 CWeaponInfo::GetExtOneVertEnable(void) const
{
	INT32 i;

	for (i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if (isExtFireType(i))
		{
			//ASSERT_WI(i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtOneVertEnable(m_u8EncryptKey[VALUETYPE_INT]);
			}
		}
	}

	return 0;
}

INT32 CWeaponInfo::GetExtAutoPointShotEnable(void) const
{
	INT32 i;

	for (i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if (isExtFireType(i))
		{
			//ASSERT_WI(i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtAutoPointShotEnable(m_u8EncryptKey[VALUETYPE_INT]);
			}
		}
	}

	return 0;
}

REAL32 CWeaponInfo::GetExtRecoilVertFactor( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtFireType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtRecoilVertFactor(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 1.f;
}

INT32 CWeaponInfo::GetExtRecoilVertCount(void) const
{
	INT32 i;

	for (i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if (isExtFireType(i))
		{
			//ASSERT_WI(i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtRecoilVertCount(m_u8EncryptKey[VALUETYPE_INT]);
			}
		}
	}

	return 0;
}

REAL32 CWeaponInfo::GetExtDeviationFactor( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtFireType( i) )
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtDeviationFactor(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 1.f;
}

INT32 CWeaponInfo::GetExtAutoFireCount( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtFireType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtAutoFireCount(m_u8EncryptKey[VALUETYPE_INT]);
			}
		}
	}

	return 0;
}

REAL32 CWeaponInfo::GetExtFireDelay( void) const
{
	INT32 i;
	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtFireType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponExtGunInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponExtGunInfo*)m_pExtInfo[i])->GetExtFireDelay(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 0.f;
}

REAL32 CWeaponInfo::GetZoomFOV( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		WEAPON::EXT_TYPE type = getExtensionType( i);

		if( type == WEAPON::EXT_ZOOM || type == WEAPON::EXT_STEADY_AIM)
		{
			if (i3::kind_of<CWeaponZoomInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponZoomInfo*)m_pExtInfo[i])->GetZoomFov(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 0.f;
}

INT32 CWeaponInfo::GetScopeType( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtScopeType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponScopeInfo*)m_pExtInfo[i])->GetScopeType(m_u8EncryptKey[VALUETYPE_INT]);
			}
		}
	}

	return -1;
}

INT32 CWeaponInfo::GetScopeSubType(void) const
{
	INT32 i;

	for (i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if (isExtScopeType(i))
		{
			//ASSERT_WI(i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponScopeInfo*)m_pExtInfo[i])->GetSubScopeType();
			}
		}
	}

	return -1;
}

REAL32 CWeaponInfo::GetScopeFOV( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtScopeType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponScopeInfo*)m_pExtInfo[i])->GetScopeFOV(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 0.f;
}

REAL32 CWeaponInfo::GetScopeFOV2( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtScopeType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponScopeInfo*)m_pExtInfo[i])->GetScopeFOV2(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 0.f;
}

INT32 CWeaponInfo::GetMaxScopeLevel( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( isExtScopeType( i))
		{
			//ASSERT_WI( i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponScopeInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponScopeInfo*)m_pExtInfo[i])->GetMaxScopeLevel(m_u8EncryptKey[VALUETYPE_INT]);
			}
		}
	}

	return 0;
}

REAL32 CWeaponInfo::GetExtSilencerDmgRate( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( getExtensionType( i) == WEAPON::EXT_SILENCER)
		{
			//ASSERT_WI( i3::same_of<CWeaponSilencerInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponSilencerInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponSilencerInfo*)m_pExtInfo[i])->GetExtSilencerDmgRate(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 1.f;
}

REAL32 CWeaponInfo::GetReloadTimeDual( void) const
{
	INT32 i;

	for( i = 0; i < MAX_WEAPON_EXTENSION; i++)
	{
		if( getExtensionType( i) == WEAPON::EXT_DUAL_MAG)
		{
			//ASSERT_WI( i3::same_of<CWeaponDualMagInfo*>(m_pExtInfo[i]));
			if ( i3::kind_of<CWeaponDualMagInfo*>(m_pExtInfo[i]))
			{
				return ((CWeaponDualMagInfo*)m_pExtInfo[i])->GetReloadTimeDual(m_u8EncryptKey[VALUETYPE_REAL]);
			}
		}
	}

	return 0.f;
}

INT32 CWeaponInfo::GetWeaponSpecialHitHudIndex( void) const
{
	return m_nSpecialDamageHUDIndex;
}

INT32 CWeaponInfo::GetWeaponSpecialDieHudIndex(void) const
{
	return m_nSpecialDieHUDIndex;
}