#include "pch.h"			
#include "CommonDediCli.h"

char* g_pDinoName[DINO_KIND_COUNT] = {
	"RaptorRed",
	"RaptorGreen",
	"RaptorBlue",
	"Sting",
	"Acid",
	"AcidBomb",
	"MutantTrex",
	"Tank",
	"AcidItem",
	"RaptorReinforce",
	"StingReinforce",
	"AcidReinforce"
};

INT8 g_i8MaxSkillLevel[USER_SKILL_COUNT] = {
	MAX_LEVEL_USERSKILL,
	MAX_LEVEL_USERSKILL,
	MAX_LEVEL_USERSKILL,
	MAX_LEVEL_USERSKILL,

	MAX_LEVEL_USERSKILL,
	MAX_LEVEL_USERSKILL,
	MAX_LEVEL_USERSKILL,
	MAX_LEVEL_USERSKILL,
	MAX_LEVEL_USERSKILL,
	MAX_LEVEL_USERSKILL,
	MAX_LEVEL_USERSKILL,
	2,
	1
};

REAL32 D3DXPlaneDotCoord( D3DXPLANE* pP, VEC3D* pV)
{
	return pP->a*pV->x + pP->b*pV->y + pP->c*pV->z + pP->d;
}

void CDomiStageCli::Destroy()
{
	I3_SAFE_DELETE_ARRAY( m_NaviMeshTri );
	m_i32NaviMeshCount = 0;
}

INT32 CDomiStageCli::LoadSAM( char* szFileName )
{
	struct ST_SAM_HEADER
	{
		char	_szID[2];
		char	_szVerConvert[32];
		short	_i16Reserved1;
		short	_i16Reserved2;
	};

	ST_SAM_HEADER Header;
	FILE* fp = fopen( szFileName, "rb" );

	if (fp == nullptr)
		return -1;

	int alreadyReadCount = 0;
	alreadyReadCount = fread( &Header, sizeof( ST_SAM_HEADER ), 1, fp );
	I3ASSERT(alreadyReadCount == 1);

	if ( Header._szID[0] != 'N' || Header._szID[1] != 'M' )
	{
		fclose( fp );
		return -1;
	}
		
	alreadyReadCount = fread( &m_i32NaviMeshCount, sizeof( INT32 ), 1, fp );
	I3ASSERT(alreadyReadCount == 1);

	m_NaviMeshTri = new ST_NM_NODE_SAM [m_i32NaviMeshCount];
	alreadyReadCount = fread( m_NaviMeshTri, sizeof( ST_NM_NODE_SAM ), m_i32NaviMeshCount, fp );
	I3ASSERT(alreadyReadCount == m_i32NaviMeshCount);
	fclose( fp );

	return 1;
}

INT32 CDomiGlobalCli::LoadCondition( char* szPath )
{
	i3IniParser Parser;
	char szTmp[64];
	INT32 i32Tmp;

	char szName[MAX_PATH];
	_snprintf( szName, MAX_PATH-1, "%s\\Skill_SP_Table.ini", szPath );
	if ( !Parser.OpenFromFile( szName ) )								return -1;
	if ( !Parser.ReadSection( _T("UseSupply") ) )						return -2;

	if ( !Parser.GetValue( "HPSupply_Use_SP", &i32Tmp ) )				return -3;
	m_ConditionSvrCli._HPSupply._ui16SpendSP_Use = (UINT16)i32Tmp;
	if ( !Parser.GetValue( "AmmoSupply_Use_SP", &i32Tmp ) )				return -4;
	m_ConditionSvrCli._AmmoSupply._ui16SpendSP_Use = (UINT16)i32Tmp;

	if ( !Parser.ReadSection( _T("InstallObject") ) )					return -5;
	for( INT32 i = 0; i < MAX_LEVEL_SENTRYGUN; i++ )
	{
		_snprintf( szTmp, 64-1, "SentryGunLv%d_Install_SP", i + 1 );
		if ( !Parser.GetValue( szTmp, &i32Tmp ) )						return -6;
		m_ConditionSvrCli._Sentry[i]._ui16SpendSP_Install = (UINT16)i32Tmp;

		_snprintf( szTmp, 64-1, "SentryGunLv%d_Fix_SP", i + 1 );
		if ( !Parser.GetValue( szTmp, &i32Tmp ) )						return -7;
		m_ConditionSvrCli._Sentry[i]._ui16SpendSP_Fix = (UINT16)i32Tmp;

		_snprintf( szTmp, 64-1, "SentryGunLv%d_HP", i + 1 );
		if ( !Parser.GetValue( szTmp, &i32Tmp ) )						return -8;
		m_ConditionSvrCli._Sentry[i]._i16MaxHP = (INT16)i32Tmp;
	}
	for( INT32 i = 0; i < MAX_LEVEL_DUMMY; i++ )
	{
		_snprintf( szTmp, 64-1, "DummyLv%d_Install_SP", i + 1 );
		if ( !Parser.GetValue( szTmp, &i32Tmp ) )						return -9;
		m_ConditionSvrCli._Dummy[i]._ui16SpendSP_Install = (UINT16)i32Tmp;

		_snprintf( szTmp, 64-1, "DummyLv%d_Fix_SP", i + 1 );
		if ( !Parser.GetValue( szTmp, &i32Tmp ) )						return -10;
		m_ConditionSvrCli._Dummy[i]._ui16SpendSP_Fix = (UINT16)i32Tmp;

		_snprintf( szTmp, 64-1, "DummyLv%d_HP", i + 1 );
		if ( !Parser.GetValue( szTmp, &i32Tmp ) )						return -11;
		m_ConditionSvrCli._Dummy[i]._i16MaxHP = (INT16)i32Tmp;

		_snprintf( szTmp, 64-1, "DummyLv%d_DamageExplosion", i + 1 );
		if ( !Parser.GetValue( szTmp, &i32Tmp ) )						return -12;
		m_ConditionSvrCli._Dummy[i]._i16DamageExplosion = (INT16)i32Tmp;
	}

	if ( !Parser.ReadSection( _T("SP") ) )								return -13;
	for( INT32 i = 0; i < MAX_LEVEL_DOMI_RANGE; i++ )
	{
		_snprintf( szTmp, 64-1, "MaxSP%d", i + 1 );
		if ( !Parser.GetValue( szTmp, &i32Tmp ) )						return -14;
		m_ConditionSvrCli._SPInfo[i]._ui16MaxSP = (UINT16)i32Tmp;

		_snprintf( szTmp, 64-1, "BasicSP%d", i + 1 );
		if ( !Parser.GetValue( szTmp, &i32Tmp ) )						return -15;
		m_ConditionSvrCli._SPInfo[i]._ui16BasicSP = (UINT16)i32Tmp;
	}

	if ( !Parser.ReadSection( _T("UserSkill") ) )						return -16;

	char *pszSkillName[USER_SKILL_COUNT] = {
		"HP",
		"Damage",
		"MoveSpeed",
		"Magazine",
		"WeaponEffectRatio",
		"ReloadSpeed",
		"GrenadeDamage",
		"Aggro",
		"GetSP",
		"MaxSP",
		"SpendSP",
		"GetDamage",
		"MeatBomb"
	};

	bool bUseIncreaseRatio[USER_SKILL_COUNT] = {
		true,
		true,
		true,
		true,
		false,
		true,
		true,
		true,
		true,
		true,
		true,
		true,
		false
	};

	for( INT32 i = 0; i < USER_SKILL_COUNT; i++ )
	{
		m_ConditionSvrCli._i8UserSkillMaxLevel[i] = g_i8MaxSkillLevel[i];

		for( INT8 j = 0; j < g_i8MaxSkillLevel[i]; j++ )
		{
			_snprintf( szTmp, 64-1, "%sLv%d_SpendSP", pszSkillName[i], j + 1 );
			if ( !Parser.GetValue( szTmp, &i32Tmp ) )					return -17;
			m_ConditionSvrCli._UserSkill[i][j]._ReqSPRound._i16SP = (INT16)i32Tmp;

			_snprintf( szTmp, 64-1, "%sLv%d_MinRound", pszSkillName[i], j + 1 );
			if ( !Parser.GetValue( szTmp, &i32Tmp ) )					return -18;
			m_ConditionSvrCli._UserSkill[i][j]._ReqSPRound._i16Round = (INT16)i32Tmp;

			_snprintf( szTmp, 64-1, "%sLv%d_IncreaseRatio", pszSkillName[i], j + 1 );
			if ( !Parser.GetValue( szTmp, &i32Tmp ) )					return -19;

			if ( bUseIncreaseRatio[i] )
			{
				m_ConditionSvrCli._UserSkill[i][j]._r32IncreaseRatio = i32Tmp/100.f + 1.0000001f;
			}
			else
			{
				m_ConditionSvrCli._UserSkill[i][j]._i32Value = i32Tmp;
			}
		}
	}

	Parser.Close();

	return 1;
}

void CDomiStageSetCli::Init()
{

}

INT32 CDomiStageSetCli::LoadINI( char* szName )
{
	i3IniParser Parser;
	INT32 i32Tmp;
	if ( !Parser.OpenFromFile( szName ) )							return -1;
	if ( !Parser.ReadSection( _T("Common") ) )						return -2;

	char szTmp[64];

	if ( !Parser.GetValue( "SentryGun_Count", &i32Tmp ) )			return -3;
	m_i8SentryGunMax = (INT8)i32Tmp;
	for( INT32 i = 0; i < i32Tmp; i++ )
	{
		_snprintf( szTmp, 64-1, "SentryGun%02d_X", i );
		if ( !Parser.GetValue( szTmp, &m_SentryGun[i]._v3Pos.x ) )	return -4;
		_snprintf( szTmp, 64-1, "SentryGun%02d_Y", i );
		if ( !Parser.GetValue( szTmp, &m_SentryGun[i]._v3Pos.y ) )	return -5;
		_snprintf( szTmp, 64-1, "SentryGun%02d_Z", i );
		if ( !Parser.GetValue( szTmp, &m_SentryGun[i]._v3Pos.z ) )	return -6;
	}
	if (!Parser.GetValue("Dummy_Count", &i32Tmp))					return -7;
	m_i8DummyMax = (INT8)i32Tmp;
	for( INT32 i = 0; i < i32Tmp; i++ )
	{
		_snprintf( szTmp, 64-1, "Dummy%02d_X", i );
		if ( !Parser.GetValue( szTmp, &m_Dummy[i]._v3Pos.x ) )		return -8;
		_snprintf( szTmp, 64-1, "Dummy%02d_Y", i );
		if ( !Parser.GetValue( szTmp, &m_Dummy[i]._v3Pos.y ) )		return -9;
		_snprintf( szTmp, 64-1, "Dummy%02d_Z", i );
		if ( !Parser.GetValue( szTmp, &m_Dummy[i]._v3Pos.z ) )		return -10;
		_snprintf( szTmp, 64-1, "Dummy%02d_Angle", i );
		INT32 i32Angle;
		if ( !Parser.GetValue( szTmp, &i32Angle ) )					return -11;
		m_Dummy[i]._i16Angle = (INT16)i32Angle;
	}
	if ( !Parser.GetValue( "HPSupply_Count", &i32Tmp ) )			return -12;
	m_i8HPMax = (INT8)i32Tmp;
	for( INT32 i = 0; i < i32Tmp; i++ )
	{
		_snprintf( szTmp, 64-1, "HPSupply%02d_X", i );
		if ( !Parser.GetValue(szTmp, &m_HPSupply[i]._v3Pos.x ) )	return -13;
		_snprintf( szTmp, 64-1, "HPSupply%02d_Y", i );
		if ( !Parser.GetValue(szTmp, &m_HPSupply[i]._v3Pos.y ) )	return -14;
		_snprintf( szTmp, 64-1, "HPSupply%02d_Z", i );
		if ( !Parser.GetValue(szTmp, &m_HPSupply[i]._v3Pos.z ) )	return -15;
		_snprintf( szTmp, 64-1, "HPSupply%02d_Angle", i );
		INT32 i32Angle;
		if ( !Parser.GetValue( szTmp, &i32Angle ) )					return -16;
		m_HPSupply[i]._i16Angle = (INT16)i32Angle;
	}
	if ( !Parser.GetValue( "AmmoSupply_Count", &i32Tmp ) )			return -17;
	m_i8AmmoMax = (INT8)i32Tmp;
	for( INT32 i = 0; i < i32Tmp; i++ )
	{
		_snprintf( szTmp, 64-1, "AmmoSupply%02d_X", i );
		if ( !Parser.GetValue( szTmp, &m_AmmoSupply[i]._v3Pos.x ) )	return -18;
		_snprintf( szTmp, 64-1, "AmmoSupply%02d_Y", i );
		if ( !Parser.GetValue( szTmp, &m_AmmoSupply[i]._v3Pos.y ) )	return -19;
		_snprintf( szTmp, 64-1, "AmmoSupply%02d_Z", i );
		if ( !Parser.GetValue( szTmp, &m_AmmoSupply[i]._v3Pos.z ) )	return -20;
		_snprintf( szTmp, 64-1, "AmmoSupply%02d_Angle", i );
		INT32 i32Angle;
		if ( !Parser.GetValue( szTmp, &i32Angle ) )					return -21;
		m_AmmoSupply[i]._i16Angle = (INT16)i32Angle;
	}
	if ( !Parser.GetValue( "SkillShop_Count", &i32Tmp ) )			return -22;
	m_i8SkillShopMax = (INT8)i32Tmp;
	for( INT32 i = 0; i < i32Tmp; i++ )
	{
		_snprintf( szTmp, 64-1, "SkillShop%02d_X", i );
		if ( !Parser.GetValue( szTmp, &m_SkillShop[i]._v3Pos.x ) )	return -23;
		_snprintf( szTmp, 64-1, "SkillShop%02d_Y", i );
		if ( !Parser.GetValue( szTmp, &m_SkillShop[i]._v3Pos.y ) )	return -24;
		_snprintf( szTmp, 64-1, "SkillShop%02d_Z", i );
		if ( !Parser.GetValue( szTmp, &m_SkillShop[i]._v3Pos.z ) )	return -25;
		_snprintf( szTmp, 64-1, "SkillShop%02d_Angle", i );
		INT32 i32Angle;
		if ( !Parser.GetValue( szTmp, &i32Angle ) )					return -26;
		m_SkillShop[i]._i16Angle = (INT16)i32Angle;
	}

	Parser.Close();
	return 1;
}