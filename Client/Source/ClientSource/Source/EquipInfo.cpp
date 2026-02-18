#include "pch.h"
#include "EquipInfo.h"
#include "MyRoomInfoContext.h"


I3_CLASS_INSTANCE( CEquipInfo);//, i3NamedElement);
I3_CLASS_INSTANCE( CHeadInfo);//, CEquipInfo);
I3_CLASS_INSTANCE( CBeretInfo);//, CHeadInfo);
I3_CLASS_INSTANCE( CFacegearInfo);//, CHeadInfo);
I3_CLASS_INSTANCE( CBeltInfo);//, CEquipInfo);
I3_CLASS_INSTANCE( CGloveInfo);//, CEquipInfo);
I3_CLASS_INSTANCE( CHolsterInfo);//, CEquipInfo);
I3_CLASS_INSTANCE( CJacketInfo);//, CEquipInfo);
I3_CLASS_INSTANCE( CPoketInfo);//, CEquipInfo);
I3_CLASS_INSTANCE( CSkinInfo);
I3_CLASS_INSTANCE( CUITemplateInfo_Parts);

/////////////////////////////////////////////////////////////////////////////////////

namespace 
{
	enum { VERSION1, VERSION2, VERSION_MAX = 2} ;
}

struct tagEQUIPABILITYINFO {
	const char * _pszFieldName;
	UINT32		_dataType;
};

tagEQUIPABILITYINFO sEquipAbilityProperty[VERSION_MAX][ EQUIP::eABILITY_COUNT] = {

	// VERSION1
	{	
		{ "Damage",					I3REG_TYPE_REAL32 },	// eABILITY_DAMAGE
		{ "DamageRate",				I3REG_TYPE_REAL32 },	// eABILITY_DAMAGERATE
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_CEBONUS
		{ "Defense_Arm",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_ARM
		{ "Defense_Body",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_BODY
		{ "Defense_Leg",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_LEG
		{ "Defense_Hand",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_HAND,
		{ "Defense_Foot",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_FOOt,
		{ "DetectC5",				I3REG_TYPE_INT32 },		// eABILITY_DETECTC5
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_FALLDAMAGE
		{ "HelmetProtectionRate",	I3REG_TYPE_REAL32 },	// eABILITY_PROTECTION_RATE
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_INVINCIBLE_TIME
		{ "MoveSpeed_Front",		I3REG_TYPE_REAL32 },	// eABILITY_MOVESPEED_FRONT
		{ "MoveSpeed_Back",			I3REG_TYPE_REAL32 },	// eABILITY_MOVESPEED_BACK
		{ "MoveSpeed_Side",			I3REG_TYPE_REAL32 },	// eABILITY_MOVESPEED_SIDE
		{ "MoveSpeed_Diagonal",		I3REG_TYPE_REAL32 },	// eABILITY_MOVESPEED_DIAGONAL
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_RESPAWNTIME
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_STBONUS
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_MOVESOUND_VOL
		{ "WeaponAccuracy",			I3REG_TYPE_REAL32 },	// eABILITY_ACCURACY
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_MAXBULLET
		{ "WeaponPenetrate",		I3REG_TYPE_REAL32 },	// eABILITY_PENETRATE
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_RANGE
		{ "WeaponReloadTime",		I3REG_TYPE_REAL32 },	// eABILITY_RELOADTIME
		{ "WeaponSwapTime",			I3REG_TYPE_REAL32 },	// eABILITY_SWAPTIME
		{ "WeaponFireDelay",		I3REG_TYPE_REAL32 },	// eABILITY_FIREDELAY
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_THROWSPEED
		{ nullptr,						I3REG_TYPE_INT32 },		// eABILITY_THROWCOUNT
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_HPINCREASE
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_C4SPEEDKIT
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_JUMPHEIGHT
		{ "CrossHair_SpeedFactor",	I3REG_TYPE_REAL32 },	// eABILITY_CROSSHAIR_SPEEDFACTOR
		{ "RecoilHorzCount",		I3REG_TYPE_INT32 },		// eABILITY_RECOILHORZCOUNT
	},
	// VERSION2
	{	
		{ "Damage",					I3REG_TYPE_REAL32 },	// eABILITY_DAMAGE
		{ "DamageRate",				I3REG_TYPE_REAL32 },	// eABILITY_DAMAGERATE
		{ "CEBonus",				I3REG_TYPE_REAL32 },	// eABILITY_CEBONUS
		{ "Defense_Arm",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_ARM
		{ "Defense_Body",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_BODY
		{ "Defense_Leg",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_LEG
		{ "Defense_Hand",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_HAND,
		{ "Defense_Foot",			I3REG_TYPE_REAL32 },	// eABILITY_DEFENSE_FOOt,
		{ "DetectC5",				I3REG_TYPE_INT32 },		// eABILITY_DETECTC5
		{ "FallDamage",				I3REG_TYPE_REAL32 },	// eABILITY_FALLDAMAGE
		{ "HelmetProtectionRate",	I3REG_TYPE_REAL32 },	// eABILITY_PROTECTION_RATE
		{ "InvincibleTime",			I3REG_TYPE_REAL32 },	// eABILITY_INVINCIBLE_TIME
		{ "MoveSpeed_Front",		I3REG_TYPE_REAL32 },	// eABILITY_MOVESPEED_FRONT
		{ "MoveSpeed_Back",			I3REG_TYPE_REAL32 },	// eABILITY_MOVESPEED_BACK
		{ "MoveSpeed_Side",			I3REG_TYPE_REAL32 },	// eABILITY_MOVESPEED_SIDE
		{ "MoveSpeed_Diagonal",		I3REG_TYPE_REAL32 },	// eABILITY_MOVESPEED_DIAGONAL
		{ "RespawnTime",			I3REG_TYPE_REAL32 },	// eABILITY_RESPAWNTIME
		{ "STBonus",				I3REG_TYPE_REAL32 },	// eABILITY_STBONUS
		{ "SoundVolume",			I3REG_TYPE_REAL32 },	// eABILITY_MOVESOUND_VOL
		{ "WeaponAccuracy",			I3REG_TYPE_REAL32 },	// eABILITY_ACCURACY
		{ "WeaponMaxBullet",		I3REG_TYPE_REAL32 },	// eABILITY_MAXBULLET
		{ "WeaponPenetrate",		I3REG_TYPE_REAL32 },	// eABILITY_PENETRATE
		{ "WeaponRange",			I3REG_TYPE_REAL32 },	// eABILITY_RANGE
		{ "WeaponReloadTime",		I3REG_TYPE_REAL32 },	// eABILITY_RELOADTIME
		{ "WeaponSwapTime",			I3REG_TYPE_REAL32 },	// eABILITY_SWAPTIME
		{ "WeaponFireDelay",		I3REG_TYPE_REAL32 },	// eABILITY_FIREDELAY
		{ "WeaponThrowSpeed",		I3REG_TYPE_REAL32 },	// eABILITY_THROWSPEED
		{ "WeaponThrowCount",		I3REG_TYPE_INT32 },		// eABILITY_THROWCOUNT
		{ "HPIncrease",				I3REG_TYPE_REAL32 },	// eABILITY_HPINCREASE
		{ "C4SpeedKit",				I3REG_TYPE_REAL32 },	// eABILITY_C4SPEEDKIT
		{ "JumpHeight",				I3REG_TYPE_REAL32 },	// eABILITY_JUMPHEIGHT
		{ nullptr,						I3REG_TYPE_REAL32 },	// eABILITY_CROSSHAIR_SPEEDFACTOR
		{ nullptr,						I3REG_TYPE_INT32 },		// eABILITY_RECOILHORZCOUNT
	}
};

I3_CLASS_INSTANCE( CEquipTransform);//, i3ElementBase);

CEquipTransform::~CEquipTransform()
{
	i3::vector_map<i3::string, TRANSFORMINFO*>::iterator it = m_listByChara.begin();

	while( it != m_listByChara.end())
	{
		I3MEM_SAFE_FREE( it->second);
		it++;
	}

	m_listByChara.clear();
}

void CEquipTransform::ReadInfo( i3RegKey * pKey)
{
	VEC4D v;
	INT32 i;

	for( i = 0; i < pKey->GetDataCount(); i++)
	{
		i3RegData * pData = pKey->GetData( i);

		if( pData->hasName())
		{
			TRANSFORMINFO * pInfo = (TRANSFORMINFO*) i3MemAlloc( sizeof( TRANSFORMINFO));
			GET_REG_DATA( pData, &v);
			i3Vector::Set( &pInfo->m_Pos, getX( &v), getY( &v), getZ( &v));
			pInfo->m_Scale = getW( &v);
			
			m_listByChara.insert( i3::vector_map<i3::string, TRANSFORMINFO*>::value_type( pData->GetName(), pInfo));
		}
	}
}

CEquipTransform::TRANSFORMINFO * CEquipTransform::_FindInfo( const char * pszName)
{
	i3::vector_map<i3::string, TRANSFORMINFO*>::iterator it;
	it = m_listByChara.find( pszName);

	if( it != m_listByChara.end())
		return (TRANSFORMINFO*)it->second;

	return nullptr;
}

bool CEquipTransform::GetPositionScale( const char * pszName, VEC3D & outVec, REAL32 & outScale)
{
	TRANSFORMINFO * pInfo = _FindInfo( pszName);

	if( pInfo != nullptr)
	{
		i3Vector::Copy( &outVec, &pInfo->m_Pos);
		outScale = pInfo->m_Scale;
		return true;
	}
	return false;
}


CEquipInfo::CEquipInfo(void)
{
	i3mem::Fill(m_ShareResItemID, 0xFFFFFFFF, sizeof(m_ShareResItemID));
}

CEquipInfo::~CEquipInfo(void)
{
	I3_SAFE_RELEASE( m_pTransformInfo);
	I3_SAFE_RELEASE( m_pTransformInfo_Red);
}

void CEquipInfo::ReadInfo( i3RegKey * pKey)
{
	INT32 i;

	I3ASSERT( pKey != nullptr);

	i3::rc_string KeyNameEnc = pKey->GetNameString();
	GlobalFunc::EncryptValue( KeyNameEnc, ENC_SHIFT_EQUIP_RESNAME);
	SetName( KeyNameEnc);

	//요거 중요한 넘입니다. 지우지 마세요( 케릭터 타입을 문자열이아닌 인덱스로 찾고자하는경우 쓰입니다.
	//내부의 데이터가 정렬이되는경우 Child의 인덱스로 찾기를 시도하는경우 오류가 생길수 있으므로 내부데이터를 하나둡니다.
	FIND_REG_DATA( pKey, "_Type",			&m_nType);
	FIND_REG_DATA( pKey, "_ResPath",		m_strResPath );
	FIND_REG_DATA( pKey, "DiffuseTexture",	m_strDiffuseTexture );
	FIND_REG_DATA( pKey, "NormalTexture",	m_strNormalTexture );
	FIND_REG_DATA( pKey, "SpecularTexture",	m_strSpecularTexture );
	FIND_REG_DATA( pKey, "ResLocalPath",	m_strResLocalPath );
	
	FIND_REG_DATA( pKey, "ITEMID",				(INT32*) &m_ITEMID);

	{
		char szTemp[ 256] = {0,};
		for( i = 0; i < MAX_SHARE_RES; i++)
		{
			i3::snprintf( szTemp, sizeof( szTemp), "ShareResID%d", i);
			FIND_REG_DATA( pKey, szTemp, (INT32*) &m_ShareResItemID[ i]);
		}
	}

	INT32 nTemp = 0;
	FIND_REG_DATA( pKey, "IsDefaultEquip", &nTemp);
	m_bIsDefaultEquipment = (nTemp>0);

	nTemp = 0;
	FIND_REG_DATA(pKey, "IsLimitedByDesigSlot", &nTemp);
	m_IsLimitedByDesigSlot = (nTemp > 0) ? 1 : 0;

	////////////////////////// Read Common Info //////////////////////////////

	i3RegKey * pChildKey = Config::FindKey( "Common", pKey);	

	if( pChildKey != nullptr)
		ReadCommonInfo( pChildKey);

	////////////////////////// Read Verson Info //////////////////////////////
	bool bV2Info = g_pEnvSet->IsV2Version();
	if( bV2Info )	pChildKey = Config::FindKey( "V2_Info", pKey);
	else			pChildKey = Config::FindKey( "V1_Info", pKey);

	if( pChildKey != nullptr )
		ReadVersionInfo(pChildKey, bV2Info);
	else
		ReadVersionInfo(pKey, bV2Info);
}

void CEquipInfo::ReadCommonInfo( i3RegKey * pKey)
{
	i3RegKey * pChildKey = Config::FindKey( "_Transform", pKey);

	if( pChildKey != nullptr)
	{
		m_pTransformInfo = CEquipTransform::new_object();
		m_pTransformInfo->ReadInfo( pChildKey);

		m_pTransformInfo_Red = m_pTransformInfo;
		I3_MUST_ADDREF(m_pTransformInfo_Red);
	}
	else
	{
		pChildKey = Config::FindKey( "_Transform_Red", pKey);
		if( pChildKey != nullptr)
		{
			m_pTransformInfo_Red = CEquipTransform::new_object();
			m_pTransformInfo_Red->ReadInfo( pChildKey);
		}

		pChildKey = Config::FindKey( "_Transform_Blue", pKey);
		if( pChildKey != nullptr)
		{
			m_pTransformInfo = CEquipTransform::new_object();
			m_pTransformInfo->ReadInfo( pChildKey);
		}
	}
}

void CEquipInfo::ReadVersionInfo( i3RegKey * pKey, bool isV2Info)
{
	i3::rc_wstring		resPostName;

	FIND_REG_DATA( pKey, "ResPostName",			resPostName );
	if( !resPostName.empty() ) 
	{
		m_strResName_Red = m_strResPostName = resPostName;
	}
	else
	{
		FIND_REG_DATA( pKey, "ResName_Blue",	m_strResPostName );
		FIND_REG_DATA( pKey, "ResName_Red",		m_strResName_Red );
	}

	// Res Name & Description
	FIND_REG_DATA( pKey, "_Name",			m_strName );
	FIND_REG_DATA( pKey, "Description",		m_strDescription );

	// ShapeIndex 설정.
	{
		// 기본 ShapeIndex
		FIND_REG_DATA( pKey, "UIShapeIndex",	&m_UIShapeIndex);

		// 버전 별 분리되는 ShapeIndex가 있는 경우 해당 값으로 적용한다.
		if( isV2Info )
			FIND_REG_DATA( pKey, "UIShapeIndexV2",	&m_UIShapeIndex);
		else
			FIND_REG_DATA( pKey, "UIShapeIndexV1",	&m_UIShapeIndex);
	}

	FIND_REG_DATA( pKey, "CombinationType", &m_i32CombinationType );

	// _Ability 
	i3RegKey * pChildKey = Config::FindKey(LS_STR("_Ability"), pKey);
	if( pChildKey != nullptr)
	{
		tagEQUIPABILITYINFO * pEquipTag = nullptr;

		if( isV2Info )	pEquipTag = sEquipAbilityProperty[VERSION2];
		else			pEquipTag = sEquipAbilityProperty[VERSION1];

		for( INT32 j = 0; j < EQUIP::eABILITY_COUNT; j++)
		{
			if( pEquipTag[ j]._pszFieldName != nullptr)
			{
				if( pEquipTag[ j]._dataType == I3REG_TYPE_INT32)
				{
					INT32 iTemp = 0;
					FIND_REG_DATA( pChildKey, pEquipTag[ j]._pszFieldName, &iTemp );
					m_Ability[ j] = (REAL32) iTemp;
				}
				else
				{
					GlobalFunc::VAR_FIND_REG_DATA( pChildKey, pEquipTag[ j]._pszFieldName, m_Ability[ j] );
				}
			}
		}
	}
}


/*virtual*/ void CEquipInfo::OnEncryptInfo( UINT8 u8Key)
{
	m_u8EncryptKey = u8Key;

	{
		GlobalFunc::EncryptValue( &m_nType, m_u8EncryptKey);

#if defined( MEM_ENCRYPTION_LV2)
		GlobalFunc::EncryptValue( &m_UIShapeIndex, m_u8EncryptKey);
#endif

		for( INT32 i = 0; i < MAX_SHARE_RES; i++)
		{
			GlobalFunc::EncryptValue( &m_ShareResItemID[i], m_u8EncryptKey);
		}

		////////////////////////////////////////////////////////////////////////
		GlobalFunc::EncryptValue( (INT32*)&m_ITEMID, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_i32CombinationType, m_u8EncryptKey);
		////////////////////////////////////////////////////////////////////////
	
		GlobalFunc::EncryptValue( m_strName, m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_strDescription, m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_strResPath, m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_strDiffuseTexture, m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_strNormalTexture, m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_strSpecularTexture, m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_strResLocalPath, m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_strResPostName, m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_strResName_Red, m_u8EncryptKey);
	}
}

void CEquipInfo::getName(  i3::rc_wstring& out) const
{
	GlobalFunc::DecryptValue( out, m_strName, m_u8EncryptKey);
}

void CEquipInfo::getResPath(  i3::rc_wstring& out) const
{
	GlobalFunc::DecryptValue( out, m_strResPath, m_u8EncryptKey);
}

void CEquipInfo::getDescription(  i3::rc_wstring& out) const
{
	GlobalFunc::DecryptValue( out, m_strDescription, m_u8EncryptKey);
}

void CEquipInfo::getDiffuseTexture( i3::rc_wstring& out) const
{
	GlobalFunc::DecryptValue( out, m_strDiffuseTexture, m_u8EncryptKey);
}

void CEquipInfo::getNormalTexture( i3::rc_wstring& out) const
{
	GlobalFunc::DecryptValue( out, m_strNormalTexture, m_u8EncryptKey);
}

void CEquipInfo::getSpecularTexture( i3::rc_wstring& out) const
{
	GlobalFunc::DecryptValue( out, m_strSpecularTexture, m_u8EncryptKey);
}

INT32	CEquipInfo::GetUIShapeIndex() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (INT32*) &m_UIShapeIndex, m_u8EncryptKey);
#else
	return m_UIShapeIndex;
#endif
}

bool	CEquipInfo::IsLimitedByDesigSlot() const
{
	INT32 resVal = m_IsLimitedByDesigSlot;
	
	I3ASSERT(resVal == 0 || resVal == 1);
	
	return (resVal != 0);
}

void	CEquipInfo::GetResLocalPath( i3::rc_wstring& out) const
{
	GlobalFunc::DecryptValue( out, m_strResLocalPath, m_u8EncryptKey);
}

void	CEquipInfo::GetResPostName( i3::rc_wstring& out, TEAM_TYPE team, INT32 res_subtype,  bool bFemale ) const
{
	if( team == TEAM_RED)
		GlobalFunc::DecryptValue( out, m_strResName_Red, m_u8EncryptKey);
	else
		GlobalFunc::DecryptValue( out, m_strResPostName, m_u8EncryptKey);
}


T_ItemID	CEquipInfo::GetITEMID(void)const
{
	return (T_ItemID) GlobalFunc::DecryptValue( (INT32*) &m_ITEMID, m_u8EncryptKey);
}

EQUIP::RES_ID CEquipInfo::GetResID( void) const
{
	return EQUIP::ItemID2ResID( GetITEMID());
}

bool CEquipInfo::IsSharedUniq( void) const
{
	for( UINT32 i = 0; i < MAX_SHARE_RES; i++)
		if( GetShareResItemID( i) != 0xFFFFFFFF)
			return true;

	return false;
}

T_ItemID	CEquipInfo::GetShareResItemID( UINT32 idx) const
{
	I3ASSERT_RETURN( idx < MAX_SHARE_RES, 0);

	return (T_ItemID) GlobalFunc::DecryptValue( (INT32*) &m_ShareResItemID[ idx], m_u8EncryptKey);
}

INT32 CEquipInfo::GetCombinationType(void) const
{
	return (T_ItemID) GlobalFunc::DecryptValue( (INT32*) &m_i32CombinationType, m_u8EncryptKey);
}

//////////////////////////////////////////////////////////////////////////////////////////
//										Ability											//
REAL32 CEquipInfo::GetAbilityValue( EQUIP::eABILITY ability) const
{
	I3ASSERT( ability < EQUIP::eABILITY_COUNT);
	return m_Ability[ ability];
}

////////////////////////////////////////////////////////////////////////////////////////////

CEquipTransform * CEquipInfo::GetTransform( TEAM_TYPE team) const
{
	if( team == TEAM_RED)
	{
		if( m_pTransformInfo_Red != nullptr)	return m_pTransformInfo_Red;
	}

	return m_pTransformInfo;
}

void CHeadInfo::ReadInfo( i3RegKey * pKey)
{
	INT32 i;

	CEquipInfo::ReadInfo( pKey);	//	중요

	for( i = 0; i < pKey->getChildCount(); ++i)
	{
		i3RegKey * pAbilityKey = ( i3RegKey*) pKey->getChild( i);
		I3ASSERT( pAbilityKey != nullptr);

		if( i3::generic_is_iequal( pAbilityKey->GetName(), LS_STR("_SubAbility")) )
		{
			FIND_REG_DATA( pAbilityKey, LS_STR("ProtectionTime"), &m_rProtectionTime);
			FIND_REG_DATA( pAbilityKey, LS_STR("DefenseForDino"), &m_rDefenseRateForDino);
		}
	}

	m_AvatarHeadType = AVATAR::HEAD_TYPE_NONE;
	FIND_REG_DATA( pKey, "HeadType", (INT32*) &m_AvatarHeadType);
	if( m_AvatarHeadType != AVATAR::HEAD_TYPE_NONE)
	{
		m_AvatarHeadType_Red = m_AvatarHeadType;
	}
	else
	{
		m_AvatarHeadType_Red = AVATAR::HEAD_TYPE_NONE;
		FIND_REG_DATA( pKey, "HeadType_Red", (INT32*) &m_AvatarHeadType_Red);
		FIND_REG_DATA( pKey, "HeadType_Blue", (INT32*) &m_AvatarHeadType);
	}

	INT32 Temp = -1;
	FIND_REG_DATA( pKey, "FacegearShow", (INT32*) &Temp);
	if( Temp != -1)
	{
		m_bFacegearShow = m_bFacegearShow_Red = (Temp==0)?false:true;
	}
	else
	{
		Temp = -1;
		FIND_REG_DATA( pKey, "FacegearShow_Red", (INT32*) &Temp);
		m_bFacegearShow_Red = (Temp==0)?false:true;

		Temp = -1;
		FIND_REG_DATA( pKey, "FacegearShow_Blue", (INT32*) &Temp);
		m_bFacegearShow = (Temp==0)?false:true;
	}
}

/*virtual*/ void CHeadInfo::OnEncryptInfo( UINT8 u8Key)
{
	CEquipInfo::OnEncryptInfo( u8Key);

#if defined( MEM_ENCRYPTION_LV2)
	GlobalFunc::EncryptValue( &m_rDefenseRateForDino, u8Key);
	GlobalFunc::EncryptValue( &m_rProtectionTime, u8Key);
#endif
	GlobalFunc::EncryptValue( (INT32*) &m_AvatarHeadType, u8Key);
	GlobalFunc::EncryptValue( (INT32*) &m_AvatarHeadType_Red, u8Key);
}

REAL32 CHeadInfo::getDefenseRate( void) const
{
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
	{
#if defined( MEM_ENCRYPTION_LV2)
		return GlobalFunc::DecryptValue( (REAL32*) &m_rDefenseRateForDino, m_u8EncryptKey);
#else
		return m_rDefenseRateForDino;
#endif
	}
	else
	{
		return GetAbilityValue( EQUIP::eABILITY_PROTECTION_RATE);
	}
}

REAL32 CHeadInfo::getProtectionTime( void) const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rProtectionTime, m_u8EncryptKey);
#else
	return m_rProtectionTime;
#endif
}

AVATAR::HEAD_TYPE CHeadInfo::getAvatarHeadType( TEAM_TYPE team) const
{
	if( team == TEAM_RED)
	{
		return (AVATAR::HEAD_TYPE) GlobalFunc::DecryptValue( (INT32*) &m_AvatarHeadType_Red, m_u8EncryptKey);
	}

	return (AVATAR::HEAD_TYPE) GlobalFunc::DecryptValue( (INT32*) &m_AvatarHeadType, m_u8EncryptKey);
}

bool CHeadInfo::getFacegearShow( TEAM_TYPE team) const
{
	if( team == TEAM_RED)
		return m_bFacegearShow_Red;

	return m_bFacegearShow;
}

////////////////////////////////////////////////////////////////////////////////////////////
////									CBeretInfo										////
////////////////////////////////////////////////////////////////////////////////////////////

/*virtual*/ void CBeretInfo::ReadVersionInfo( i3RegKey * pKey, bool isV2Info/* = false */)
{
	CHeadInfo::ReadVersionInfo( pKey);	//	중요

	i3::string str_res_name[3] = 
	{
		"ResPostName",
		"ResName_Blue",
		"ResName_Red"
	};

	i3::string str_add_keyword[EQUIP::eBERET_COUNT] =
	{
		"",
		"_AddTarget",
		"_AddReinforce",
		"_AddSuper"
	};

	for(INT32 idx = 0 ; idx < EQUIP::eBERET_COUNT; ++idx)
	{
		// 디폴트로 기본 아이템으로 설정
		m_strMixResPostName[idx] = m_strMixResPostName[0];
		m_strMixResName_Red[idx] = m_strMixResName_Red[0];

		i3::rc_wstring str_post_res_name;
		i3::string str_post_keyword = str_res_name[0] + str_add_keyword[idx];

		FIND_REG_DATA( pKey, str_post_keyword.c_str(),	str_post_res_name );
	
		if( !str_post_res_name.empty() ) 
		{
			m_strMixResPostName[idx] = m_strMixResName_Red[idx] = str_post_res_name;
		}
		else
		{
			i3::string str_blue_keyword = str_res_name[1] + str_add_keyword[idx];
			i3::string str_red_keyword = str_res_name[2] + str_add_keyword[idx];

			FIND_REG_DATA( pKey, str_blue_keyword.c_str(),	m_strMixResPostName[idx] );
			FIND_REG_DATA( pKey, str_red_keyword.c_str(),	m_strMixResName_Red[idx] );
		}
	}
}

/*virtual*/ void CBeretInfo::OnEncryptInfo( UINT8 u8Key)
{
	CHeadInfo::OnEncryptInfo( u8Key);

	for(INT32 idx = 0 ; idx < EQUIP::eBERET_COUNT; ++idx)
	{
		GlobalFunc::EncryptValue( m_strMixResPostName[idx], m_u8EncryptKey);
		GlobalFunc::EncryptValue( m_strMixResName_Red[idx], m_u8EncryptKey);
	}
}

/*virtual*/ void CBeretInfo::GetResPostName( i3::rc_wstring& out, TEAM_TYPE team, INT32 res_subtype,  bool bFemale ) const
{
	if( team == TEAM_RED)
		GlobalFunc::DecryptValue( out, m_strMixResName_Red[res_subtype], m_u8EncryptKey);
	else
		GlobalFunc::DecryptValue( out, m_strMixResPostName[res_subtype], m_u8EncryptKey);
}

////////////////////////////////////////////////////////////////////////////////////////////

void CJacketInfo::ReadInfo( i3RegKey * pKey)
{
	INT32 nTemp;

	CEquipInfo::ReadInfo( pKey);

	nTemp = -1;
	FIND_REG_DATA( pKey, "HeadgearShow", &nTemp);
	if( nTemp != -1)
	{
		m_bHeadgearShow = m_bHeadgearShow_Red = (nTemp == 1);
	}
	else
	{
		nTemp = 1;
		FIND_REG_DATA( pKey, "HeadgearShow_Red", &nTemp);
		m_bHeadgearShow_Red = (nTemp == 1); 

		nTemp = 1;
		FIND_REG_DATA( pKey, "HeadgearShow_Blue", &nTemp);
		m_bHeadgearShow = (nTemp == 1);
	}

	m_AvatarHeadType = AVATAR::HEAD_TYPE_NONE;
	FIND_REG_DATA( pKey, "HeadType", (INT32*) &m_AvatarHeadType);
	if( m_AvatarHeadType != AVATAR::HEAD_TYPE_NONE)
	{
		m_AvatarHeadType_Red = m_AvatarHeadType;
	}
	else
	{
		FIND_REG_DATA( pKey, "HeadType_Red", (INT32*) &m_AvatarHeadType_Red);
		FIND_REG_DATA( pKey, "HeadType_Blue", (INT32*) &m_AvatarHeadType);
	}
}

bool CJacketInfo::getHeadgearShow( TEAM_TYPE team) const
{
	if( team == TEAM_RED)
		return m_bHeadgearShow_Red;
	return m_bHeadgearShow;
}

AVATAR::HEAD_TYPE CJacketInfo::getAvatarHeadType( TEAM_TYPE team) const
{
	if( team == TEAM_RED)
		return m_AvatarHeadType_Red;
	return m_AvatarHeadType;
}

void CSkinInfo::ReadInfo( i3RegKey * pKey)
{
//	char szTemp[ MAX_PATH];
	CEquipInfo::ReadInfo( pKey);

	FIND_REG_DATA( pKey, "HeadType", (INT32*)&m_AvatarHeadType);
	FIND_REG_DATA( pKey, "ResName_Man_Red", m_strResName_Man_Red);
	FIND_REG_DATA( pKey, "ResName_Man_Blue", m_strResName_Man_Blue);
	FIND_REG_DATA( pKey, "ResName_Woman_Red", m_strResName_Woman_Red);
	FIND_REG_DATA( pKey, "ResName_Woman_Blue", m_strResName_Woman_Blue);

	m_bCommonStyle = (m_strResName_Man_Red.length() > 0) && (m_strResName_Woman_Red.length() > 0);

	for( INT32 i = 0; i < pKey->getChildCount(); ++i)
	{
		i3RegKey * pEquip = ( i3RegKey*) pKey->getChild( i);
		I3ASSERT( pEquip != nullptr);

		if( i3::generic_is_iequal( pEquip->GetName(), "_Equipment") )
		{
			FIND_REG_DATA( pEquip, "Headgear", (INT32*)&m_HeadgearItemID);
			FIND_REG_DATA( pEquip, "Facegear", (INT32*)&m_FacegearItemID);
			FIND_REG_DATA( pEquip, "Belt", (INT32*)&m_BeltItemID);
			FIND_REG_DATA( pEquip, "Holster", (INT32*)&m_HolsterItemID);
			break;
		}
	}
}

void CSkinInfo::ReadCommonInfo( i3RegKey * pKey)
{
	i3RegKey * pChildKey = Config::FindKey( "_Equipment", pKey);

	if( pChildKey != nullptr )
	{
		FIND_REG_DATA( pChildKey, "Headgear",	(INT32*)&m_HeadgearItemID);
		FIND_REG_DATA( pChildKey, "Facegear",	(INT32*)&m_FacegearItemID);
		FIND_REG_DATA( pChildKey, "Belt",		(INT32*)&m_BeltItemID);
		FIND_REG_DATA( pChildKey, "Holster",	(INT32*)&m_HolsterItemID);
	}
}

void CSkinInfo::OnEncryptInfo( UINT8 u8Key)
{
	CEquipInfo::OnEncryptInfo( u8Key);

	GlobalFunc::EncryptValue( (INT32*)&m_AvatarHeadType, m_u8EncryptKey);
	GlobalFunc::EncryptValue( (INT32*)&m_HeadgearItemID, m_u8EncryptKey);
	GlobalFunc::EncryptValue( (INT32*)&m_FacegearItemID, m_u8EncryptKey);
	GlobalFunc::EncryptValue( (INT32*)&m_BeltItemID, m_u8EncryptKey);
	GlobalFunc::EncryptValue( (INT32*)&m_HolsterItemID, m_u8EncryptKey);
}

AVATAR::HEAD_TYPE CSkinInfo::GetHeadType( void) const
{
	return static_cast<AVATAR::HEAD_TYPE>(GlobalFunc::DecryptValue( (INT32*)&m_AvatarHeadType, m_u8EncryptKey));
}

T_ItemID CSkinInfo::GetHeadgearItemID( void) const
{
	return GlobalFunc::DecryptValue( (INT32*)&m_HeadgearItemID, m_u8EncryptKey);
}

T_ItemID CSkinInfo::GetFacegearItemID( void) const
{
	return GlobalFunc::DecryptValue( (INT32*)&m_FacegearItemID, m_u8EncryptKey);
}

T_ItemID CSkinInfo::GetBeltItemID( void) const
{
	return GlobalFunc::DecryptValue( (INT32*)&m_BeltItemID, m_u8EncryptKey);
}

T_ItemID CSkinInfo::GetHolsterItemID( void) const
{
	return GlobalFunc::DecryptValue( (INT32*)&m_HolsterItemID, m_u8EncryptKey);
}

void CSkinInfo::GetResPostName( i3::rc_wstring& out, TEAM_TYPE team, INT32 res_subtype,  bool bFemale) const
{
	if( m_bCommonStyle)
	{
		if( team == TEAM_RED)
		{
			if( bFemale)	GlobalFunc::DecryptValue( out, m_strResName_Woman_Red, m_u8EncryptKey);
			else			GlobalFunc::DecryptValue( out, m_strResName_Man_Red, m_u8EncryptKey);
		}
		else
		{
			if( bFemale)	GlobalFunc::DecryptValue( out, m_strResName_Woman_Blue, m_u8EncryptKey);
			else			GlobalFunc::DecryptValue( out, m_strResName_Man_Blue, m_u8EncryptKey);
		}
	}
	else
	{
		CEquipInfo::GetResPostName( out, team, res_subtype);
	}
}

void CUITemplateInfo_Parts::ReadInfo(i3RegKey* key)
{
	FIND_REG_DATA(key, "Type", &m_type);

	FIND_REG_DATA(key, "Name", m_name);

	if( g_pEnvSet->IsV2Version() )
	{
		FIND_REG_DATA(key, "Name_V2", m_name);
		FIND_REG_DATA(key, "LoopLoad_V1", &m_LoopLoad);
	}
	else
	{
		FIND_REG_DATA(key, "Name_V1", m_name);
		FIND_REG_DATA(key, "LoopLoad_V1", &m_LoopLoad);
	}
}

INT32 CUITemplateInfo_Parts::GetType() const
{
	return m_type;
}

INT32 CUITemplateInfo_Parts::GetLoopLoad() const
{
	return m_LoopLoad;
}

const i3::rc_wstring& CUITemplateInfo_Parts::GetName() const
{
	return m_name;
}
