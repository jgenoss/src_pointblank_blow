#include "pch.h"
#include "CharaInfo.h"

#include "MyRoomInfoContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"


namespace
{
	const char * sPartName[ EQUIP::ePART_COUNT] = {
		"ResDftPartJacket",		// ePART_VEST,		// 상체
		"ResDftPartPoket",		// ePART_PANT,		// 하체
		"ResDftPartGlove",		// ePART_GLOVE,		// 손
		"",						// ePART_CHARA,		// 캐릭터 얼굴
		"ResDftPartHead",		// ePART_HEADGEAR,	// 머리 장비품
		"ResDftPartFace",		// ePART_FACEGEAR,	// 얼굴 장비품
		"ResDftPartHolster",	// ePART_HOLSTER,	// 총집
		"ResDftPartBelt",		// ePART_BELT,		// 벨트
		"ResDftPartSkin",		// ePART_SKIN		// 스킨
		"ResDftPartBeret",		// ePART_BERET		// 베레모
	};
}

I3_CLASS_INSTANCE( CCharaGlobal);
I3_CLASS_INSTANCE( CCharaGlobal2);
I3_CLASS_INSTANCE( CCharaInfo);
I3_CLASS_INSTANCE( CCharaInfo2);
I3_CLASS_INSTANCE( CUITemplateInfo_Chara);

void CCharaGlobal::ReadInfo( i3RegKey * pKey)
{
	char szName[ 128];

	I3ASSERT( pKey != nullptr);

	const char * pszName = pKey->GetName();
	I3ASSERT( pszName != nullptr);

	i3::string_ncopy_nullpad( szName, pszName, sizeof( szName));
	INT32 len = i3::generic_string_size( szName);

	if( len > 0)
		GlobalFunc::EncryptValue( szName, len, ENC_SHIFT_CHARA_RESNAME);

	SetName( szName);

	//요거 중요한 넘입니다. 지우지 마세요( 케릭터 타입을 문자열이아닌 인덱스로 찾고자하는경우 쓰입니다.
	//내부의 데이터가 정렬이되는경우 Child의 인덱스로 찾기를 시도하는경우 오류가 생길수 있으므로 내부데이터를 하나둡니다.
	SetInvincibleTime( GetKeyData<REAL32>(pKey, "InvincibleTime") );
	SetInvincibleTime_4vs4( GetKeyData<REAL32>(pKey, "InvincibleTime_4vs4") );

	SetFallMinDamage( GetKeyData<INT32>(pKey, "FallMinDamage") );
	SetFallDamageMinHeight( GetKeyData<REAL32>(pKey, "FallDamageMinHeight") );
	SetFallDamageSlope( GetKeyData<REAL32>(pKey, "FallDamageSlope") );
	SetAimingAngleMax( GetKeyData<REAL32>(pKey, "AimingAngleMax") );
	SetAimingAngleMin( GetKeyData<REAL32>(pKey, "AimingAngleMin") );

	SetSpeedReductionTime( GetKeyData<REAL32>(pKey, "SpeedReductionTime") );
	SetSpeedReductionMaxRate( GetKeyData<REAL32>(pKey, "SpeedReductionMaxRate") );
}

void CCharaGlobal::OnEncryptInfo( UINT8 u8Key)
{
	m_u8EncryptKey = u8Key;

	{
#if  defined( MEM_ENCRYPTION_LV2)
		GlobalFunc::EncryptValue( &m_rInvincibleTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rInvincibleTime_4vs4, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_nFallMinDamage, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rFallDamageMinHeight, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rFallDamageSlope, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rAimingAngleMax, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rAimingAngleMin, m_u8EncryptKey);
#endif

		GlobalFunc::EncryptValue( &m_rSpeedReductionTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rSpeedReductionMaxRate, m_u8EncryptKey);
	}
}

void CCharaGlobal::SetInvincibleTime( REAL32 val )
{
	m_rInvincibleTime = val;
}

REAL32 CCharaGlobal::GetInvincibleTime() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rInvincibleTime, m_u8EncryptKey);
#else
	return m_rInvincibleTime;
#endif
}

REAL32 CCharaGlobal::GetInvincibleTime_4vs4() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rInvincibleTime_4vs4, m_u8EncryptKey);
#else
	return m_rInvincibleTime_4vs4;
#endif
}

void CCharaGlobal::SetInvincibleTime_4vs4( REAL32 val )
{
	m_rInvincibleTime_4vs4 = val;
}

REAL32 CCharaGlobal::GetAimingAngleMax() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rAimingAngleMax, m_u8EncryptKey);
#else
	return m_rAimingAngleMax;
#endif
}

void CCharaGlobal::SetAimingAngleMax( REAL32 val )
{
	m_rAimingAngleMax = val;
}

REAL32 CCharaGlobal::GetAimingAngleMin() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rAimingAngleMin, m_u8EncryptKey);
#else
	return m_rAimingAngleMin;
#endif
}

void CCharaGlobal::SetAimingAngleMin( REAL32 val )
{
	m_rAimingAngleMin = val;
}

REAL32 CCharaGlobal::GetSpeedReductionTime() const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rSpeedReductionTime, m_u8EncryptKey);
}

void CCharaGlobal::SetSpeedReductionTime( REAL32 val )
{
	m_rSpeedReductionTime = val;
}

REAL32 CCharaGlobal::GetSpeedReductionMaxRate() const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rSpeedReductionMaxRate, m_u8EncryptKey);
}

void CCharaGlobal::SetSpeedReductionMaxRate( REAL32 val )
{
	m_rSpeedReductionMaxRate = val;
}

INT32 CCharaGlobal::GetFallMinDamage() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (INT32*) &m_nFallMinDamage, m_u8EncryptKey);
#else
	return m_nFallMinDamage; 
#endif
}
void CCharaGlobal::SetFallMinDamage(INT32 val)
{
	m_nFallMinDamage = val; 
}
REAL32 CCharaGlobal::GetFallDamageMinHeight() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rFallDamageMinHeight, m_u8EncryptKey);
#else
	return m_rFallDamageMinHeight; 
#endif
}
void CCharaGlobal::SetFallDamageMinHeight(REAL32 val)
{
	m_rFallDamageMinHeight = val; 
}
REAL32 CCharaGlobal::GetFallDamageSlope() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rFallDamageSlope, m_u8EncryptKey);
#else
	return m_rFallDamageSlope;
#endif
}
void CCharaGlobal::SetFallDamageSlope(REAL32 val)
{
	m_rFallDamageSlope = val; 
}

void CCharaGlobal::ReadDamageByModeInfo( i3RegKey * pKey)
{
	m_rPartDamageRateInSuddenDeath[ CHARA::eDAMAGEPART_HEAD] = GetKeyData<REAL32>(pKey, "Head");
	m_rPartDamageRateInSuddenDeath[ CHARA::eDAMAGEPART_BODY] = GetKeyData<REAL32>(pKey, "Body");	
	m_rPartDamageRateInSuddenDeath[ CHARA::eDAMAGEPART_ARM] = GetKeyData<REAL32>(pKey, "Arm");	
	m_rPartDamageRateInSuddenDeath[ CHARA::eDAMAGEPART_HAND] = GetKeyData<REAL32>(pKey, "Hand");	
	m_rPartDamageRateInSuddenDeath[ CHARA::eDAMAGEPART_LEG] = GetKeyData<REAL32>(pKey, "Leg");	
	m_rPartDamageRateInSuddenDeath[ CHARA::eDAMAGEPART_FOOT] = GetKeyData<REAL32>(pKey, "Foot");	
	m_rPartDamageRateInSuddenDeath[ CHARA::eDAMAGEPART_NECK] = GetKeyData<REAL32>(pKey, "Neck");	
	m_rPartDamageRateInSuddenDeath[ CHARA::eDAMAGEPART_VITAL] = GetKeyData<REAL32>(pKey, "Vital");
	m_rPartDamageRateInSuddenDeath[ CHARA::eDAMAGEPART_OTHER] = GetKeyData<REAL32>(pKey, "Other");	
	m_rMinDamage = GetKeyData<REAL32>(pKey, "MinDamage");
}

REAL32 CCharaGlobal::GetDamageRateByMode(CHARA::eDAMAGEPART eParam) const
{
	I3ASSERT(eParam < CHARA::eDAMAGEPART_COUNT);
	return m_rPartDamageRateInSuddenDeath[eParam];
}

void CCharaGlobal::CalcDamageByPart(REAL32& rDamage)
{
	if( MyRoomInfoContext::i()->getStage()->IsHeadShotOnly() )
	{
		if(rDamage < m_rMinDamage)
			rDamage = m_rMinDamage;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////



void CCharaGlobal2::ReadInfo( i3RegKey * pKey)
{
	char szName[ 128];

	I3ASSERT( pKey != nullptr);

	char * pszName = (char*)pKey->GetName();
	I3ASSERT( pszName != nullptr);

	i3::string_ncopy_nullpad( szName, pszName, sizeof( szName));
	INT32 len = i3::generic_string_size( szName);
	if( len > 0)
		GlobalFunc::EncryptValue( szName, len, ENC_SHIFT_CHARA_RESNAME);

	SetName( szName);

	//요거 중요한 넘입니다. 지우지 마세요( 케릭터 타입을 문자열이아닌 인덱스로 찾고자하는경우 쓰입니다.
	//내부의 데이터가 정렬이되는경우 Child의 인덱스로 찾기를 시도하는경우 오류가 생길수 있으므로 내부데이터를 하나둡니다.

	SetInvincibleTime( GetKeyData<REAL32>(pKey, "InvincibleTime") );
	SetInvincibleTime_4vs4( GetKeyData<REAL32>(pKey, "InvincibleTime_4vs4") );

	SetFallMinDamage( GetKeyData<INT32>(pKey, "FallMinDamage") );
	SetFallDamageMinHeight( GetKeyData<REAL32>(pKey, "FallDamageMinHeight") );
	SetFallDamageSlope( GetKeyData<REAL32>(pKey, "FallDamageSlope") );
	SetAimingAngleMax( GetKeyData<REAL32>(pKey, "AimingAngleMax") );
	SetAimingAngleMin( GetKeyData<REAL32>(pKey, "AimingAngleMin") );

	SetSpeedReductionTime( GetKeyData<REAL32>(pKey, "SpeedReductionTime") );
	SetSpeedReductionMaxRate( GetKeyData<REAL32>(pKey, "SpeedReductionMaxRate") );
}




void CCharaGlobal2::OnEncryptInfo( UINT8 u8Key)
{
	m_u8EncryptKey = u8Key;

	{
#if defined( MEM_ENCRYPTION_LV2)
		GlobalFunc::EncryptValue( &m_rInvincibleTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rInvincibleTime_4vs4, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_nFallMinDamage, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rFallDamageMinHeight, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rFallDamageSlope, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rAimingAngleMax, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rAimingAngleMin, m_u8EncryptKey);
#endif

		GlobalFunc::EncryptValue( &m_rSpeedReductionTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rSpeedReductionMaxRate, m_u8EncryptKey);
	}
}

INT32 CCharaGlobal2::GetFallMinDamage() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (INT32*) &m_nFallMinDamage, m_u8EncryptKey);
#else
	return m_nFallMinDamage; 
#endif
}
void CCharaGlobal2::SetFallMinDamage(INT32 val)
{
	m_nFallMinDamage = val; 
}
REAL32 CCharaGlobal2::GetFallDamageMinHeight() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rFallDamageMinHeight, m_u8EncryptKey);
#else
	return m_rFallDamageMinHeight; 
#endif
}
void CCharaGlobal2::SetFallDamageMinHeight(REAL32 val)
{
	m_rFallDamageMinHeight = val; 
}
REAL32 CCharaGlobal2::GetFallDamageSlope() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rFallDamageSlope, m_u8EncryptKey);
#else
	return m_rFallDamageSlope; 
#endif
}
void CCharaGlobal2::SetFallDamageSlope(REAL32 val)
{
	m_rFallDamageSlope = val; 
}

REAL32 CCharaGlobal2::GetInvincibleTime() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rInvincibleTime, m_u8EncryptKey);
#else
	return m_rInvincibleTime;
#endif
}

void CCharaGlobal2::SetInvincibleTime( REAL32 val )
{
	m_rInvincibleTime = val;
}

REAL32 CCharaGlobal2::GetAimingAngleMax() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rAimingAngleMax, m_u8EncryptKey);
#else
	return m_rAimingAngleMax;
#endif
}

void CCharaGlobal2::SetAimingAngleMax( REAL32 val )
{
	m_rAimingAngleMax = val;
}

REAL32 CCharaGlobal2::GetInvincibleTime_4vs4() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rInvincibleTime_4vs4, m_u8EncryptKey);
#else
	return m_rInvincibleTime_4vs4;
#endif
}

void CCharaGlobal2::SetInvincibleTime_4vs4( REAL32 val )
{
	m_rInvincibleTime_4vs4 = val;
}

REAL32 CCharaGlobal2::GetAimingAngleMin() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rAimingAngleMin, m_u8EncryptKey);
#else
	return m_rAimingAngleMin;
#endif
}

void CCharaGlobal2::SetAimingAngleMin( REAL32 val )
{
	m_rAimingAngleMin = val;
}

REAL32 CCharaGlobal2::GetSpeedReductionMaxRate() const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rSpeedReductionMaxRate, m_u8EncryptKey);
}

void CCharaGlobal2::SetSpeedReductionMaxRate( REAL32 val )
{
	m_rSpeedReductionMaxRate = val;
}

REAL32 CCharaGlobal2::GetSpeedReductionTime() const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rSpeedReductionTime, m_u8EncryptKey);
}

void CCharaGlobal2::SetSpeedReductionTime( REAL32 val )
{
	m_rSpeedReductionTime = val;
}

REAL32 CSpeedReduction::GetRate() const
{
	return m_rRate;
}

void CSpeedReduction::SetRate(REAL32 val) { m_rRate = val; }
REAL32 CSpeedReduction::GetTime() const { return m_rTime; }
void CSpeedReduction::SetTime(REAL32 val) { m_rTime = val; }

void CCharaInfo::ReadInfo( i3RegKey * pKey)
{
	INT32 i;

	CEquipInfo::ReadInfo( pKey);	//	중요

	SetHP( GetKeyData<INT32>(pKey, "HP") );

	for( i = 0; i < pKey->getChildCount(); ++i)
	{
		i3RegKey * pChildKey = ( i3RegKey*) pKey->getChild( i);
		I3ASSERT( pChildKey != nullptr);

		if( i3::generic_is_iequal( pChildKey->GetName(), "PartsList") )
		{
			for( INT32 j = 0; j < EQUIP::ePART_COUNT; j++)
			{
				m_iResParts[ j]		= GetKeyData<INT32>(pChildKey, sPartName[ j]);
			}
		}
		else if( i3::generic_is_iequal( pChildKey->GetName(), "Movement") )
		{
			SetAccelRateBackVsFront( GetKeyData<REAL32>(pChildKey, "AccelRateBackVsFront") );
			SetAccelerationTime( GetKeyData<REAL32>(pChildKey, "AccelerationTime") );

			SetJumpHeight( GetKeyData<REAL32>(pChildKey, "JumpHeight") );
			SetJumpTime( GetKeyData<REAL32>(pChildKey, "JumpTime") );
			SetJumpDelay( GetKeyData<REAL32>(pChildKey, "JumpDelay") );
			SetJumpPenaltyTime( GetKeyData<REAL32>(pChildKey, "JumpPenaltyTime") );
			SetJumpPenaltyRate( GetKeyData<REAL32>(pChildKey, "JumpPenaltyRate") );

			SetNewTempJumpHeight(GetKeyData<REAL32>(pChildKey, "NEW_TEMP_JumpHeight"));
			SetNewTempClunchJumpHeight(GetKeyData<REAL32>(pChildKey, "NEW_TEMP_ClunchJumpHeight"));

			SetMoveSpeed(MST_RUN, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "FrontMoveSpeed") );
			SetMoveSpeed(MST_RUN, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "SideMoveSpeed") );
			SetMoveSpeed(MST_RUN, MDT_BACK, GetKeyData<REAL32>(pChildKey, "BackMoveSpeed") );
			SetMoveSpeed(MST_RUN, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "DiagonalMoveSpeed") );

			SetMoveSpeed(MST_WALK, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "WalkFrontMove") );
			SetMoveSpeed(MST_WALK, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "WalkSideMove") );
			SetMoveSpeed(MST_WALK, MDT_BACK, GetKeyData<REAL32>(pChildKey, "WalkBackMove") );
			SetMoveSpeed(MST_WALK, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "WalkDiagonalMove") );

			SetMoveSpeed(MST_JUMP_RUN, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "JumpRunFrontMoveSpeed"));
			SetMoveSpeed(MST_JUMP_RUN, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "JumpRunSideMoveSpeed"));
			SetMoveSpeed(MST_JUMP_RUN, MDT_BACK, GetKeyData<REAL32>(pChildKey, "JumpRunBackMoveSpeed"));
			SetMoveSpeed(MST_JUMP_RUN, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "JumpRunDiagonalMoveSpeed"));

			SetMoveSpeed(MST_JUMP_WALK, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "JumpWalkFrontMoveSpeed"));
			SetMoveSpeed(MST_JUMP_WALK, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "JumpWalkSideMoveSpeed"));
			SetMoveSpeed(MST_JUMP_WALK, MDT_BACK, GetKeyData<REAL32>(pChildKey, "JumpWalkBackMoveSpeed"));
			SetMoveSpeed(MST_JUMP_WALK, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "JumpWalkDiagonalMoveSpeed"));

			SetClunchJumpHeight( GetKeyData<REAL32>(pChildKey, "ClunchJumpHeight") );
			SetClunchJumpCheckTime( GetKeyData<REAL32>(pChildKey, "ClunchJumpCheckTime") );
		}
		else if( i3::generic_is_iequal( pChildKey->GetName(), "SpeedReduction") )
		{
			m_rSpeedReduction[CHPS_BODY].SetRate( GetKeyData<REAL32>(pChildKey, "SpeedReductionBody") );
			m_rSpeedReduction[CHPS_BODY].SetTime( GetKeyData<REAL32>(pChildKey, "SpeedReductionBodyTime") );

			m_rSpeedReduction[CHPS_ARM].SetRate( GetKeyData<REAL32>(pChildKey, "SpeedReductionArm") );
			m_rSpeedReduction[CHPS_ARM].SetTime( GetKeyData<REAL32>(pChildKey, "SpeedReductionArmTime") );

			m_rSpeedReduction[CHPS_HAND].SetRate( GetKeyData<REAL32>(pChildKey, "SpeedReductionHand") );
			m_rSpeedReduction[CHPS_HAND].SetTime( GetKeyData<REAL32>(pChildKey, "SpeedReductionHandTime") );

			m_rSpeedReduction[CHPS_LEG].SetRate( GetKeyData<REAL32>(pChildKey, "SpeedReductionLeg") );
			m_rSpeedReduction[CHPS_LEG].SetTime( GetKeyData<REAL32>(pChildKey, "SpeedReductionLegTime") );

			m_rSpeedReduction[CHPS_FOOT].SetRate( GetKeyData<REAL32>(pChildKey, "SpeedReductionFoot") );
			m_rSpeedReduction[CHPS_FOOT].SetTime( GetKeyData<REAL32>(pChildKey, "SpeedReductionFootTime") );
		}
		else if( i3::generic_is_iequal( pChildKey->GetName(), "DamageRate") )
		{
			ReadDamageRateInfo( pChildKey);
		}
		else if( i3::generic_is_iequal( pChildKey->GetName(), "Movement_ladder"))
		{
			m_rLadderWalkBackSpeed		= GetKeyData<REAL32>(pChildKey, "WalkBackSpeed");
			m_rLadderWalkFrontSpeed		= GetKeyData<REAL32>(pChildKey, "WalkFrontSpeed");
			m_rLadderWalkSideSpeed		= GetKeyData<REAL32>(pChildKey, "WalkSideSpeed");
			m_rLadderWalkDiagonalSpeed	= GetKeyData<REAL32>(pChildKey, "WalkDiagonalSpeed");
			m_rLadderRunBackSpeed		= GetKeyData<REAL32>(pChildKey, "RunBackSpeed");
			m_rLadderRunFrontSpeed		= GetKeyData<REAL32>(pChildKey, "RunFrontSpeed");
			m_rLadderRunSideSpeed		= GetKeyData<REAL32>(pChildKey, "RunSideSpeed");
			m_rLadderRunDiagonalSpeed	= GetKeyData<REAL32>(pChildKey, "RunDiagonalSpeed");	
		}
		else if (i3::generic_is_iequal(pChildKey->GetName(), "Movement_Space"))
		{
			SetMoveSpeed(MST_RUN, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "FrontMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_RUN, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "SideMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_RUN, MDT_BACK, GetKeyData<REAL32>(pChildKey, "BackMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_RUN, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "DiagonalMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_WALK, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "WalkFrontMove"), MMT_SPACE);
			SetMoveSpeed(MST_WALK, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "WalkSideMove"), MMT_SPACE);
			SetMoveSpeed(MST_WALK, MDT_BACK, GetKeyData<REAL32>(pChildKey, "WalkBackMove"), MMT_SPACE);
			SetMoveSpeed(MST_WALK, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "WalkDiagonalMove"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_RUN, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "JumpRunFrontMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_RUN, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "JumpRunSideMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_RUN, MDT_BACK, GetKeyData<REAL32>(pChildKey, "JumpRunBackMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_RUN, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "JumpRunDiagonalMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_WALK, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "JumpWalkFrontMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_WALK, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "JumpWalkSideMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_WALK, MDT_BACK, GetKeyData<REAL32>(pChildKey, "JumpWalkBackMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_WALK, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "JumpWalkDiagonalMoveSpeed"), MMT_SPACE);
		}
	}

	SetWeaponSwap( GetKeyData<REAL32>(pKey, "WeaponSwap") );
	SetScopeWaveVelocity( GetKeyData<REAL32>(pKey, "ScopeWaveVelocity") );
	SetScopeWaveRange( GetKeyData<REAL32>(pKey, "ScopeWaveRange") );
	SetReloadSpeed( GetKeyData<REAL32>(pKey, "ReloadSpeed") );
	SetShoulderSpeed( GetKeyData<REAL32>(pKey, "ShoulderSpeed") );

	SetCamIKHeight( GetKeyData<VEC4D>(pKey, "CamIKHeight") );					//자세에따른 카메라의 허리높이
	SetFPSCamPos( GetKeyData<VEC3D>(pKey, "FPSCamPos") );

	SetNPCAttackDelay( GetKeyData<REAL32>(pKey, "_NPCAttackDelay") );
	SetNPCInvincibleTime( GetKeyData<REAL32>(pKey, "_NPCInvincibleTime") );
	SetNPCRespawnTime( GetKeyData<REAL32>(pKey, "_NPCRespawnTime") );
	SetNPCSearchRandomValue( GetKeyData<REAL32>(pKey, "_NPCSearchRandomValue") );

	SetCharaSexType(GetKeyData<INT32>(pKey, "_SexType"));

	m_RepresentID = static_cast<UINT32>(GetKeyData<INT32>( pKey, "RepresentID"));

	m_TeamBlue_ItemID = static_cast<T_ItemID>(GetKeyData<INT32>(pKey, "FixedTeam_Blue"));
	m_TeamRed_ItemID = static_cast<T_ItemID>(GetKeyData<INT32>(pKey, "FixedTeam_Red"));

#if defined( AI_MOVE_TO_WEAPON)
	FIND_REG_DATA( pKey, "_AnimName",	m_szAnimName);
#endif
}

/*virtual*/ void CCharaInfo::OnEncryptInfo( UINT8 u8Key)
{
	CEquipInfo::OnEncryptInfo( u8Key);

	{
		GlobalFunc::EncryptValue( &m_RepresentID, m_u8EncryptKey);

#if defined( MEM_ENCRYPTION_LV1)
		GlobalFunc::EncryptValue( &m_vCamIKHeight, sizeof( m_vCamIKHeight), m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_vFPSCamPos, sizeof( m_vFPSCamPos), m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rNPCAttackDelay, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rNPCInvincibleTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rNPCRespawnTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rNPCSearchRandomValue, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rJumpHeight, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rJumpTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rJumpDelay, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rJumpPenaltyTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rJumpPenaltyRate, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rClunchJumpHeight, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rClunchJumpCheckTime, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rLadderWalkBackSpeed,		m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rLadderWalkFrontSpeed,		m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rLadderWalkSideSpeed,		m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rLadderWalkDiagonalSpeed,	m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rLadderRunBackSpeed,		m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rLadderRunFrontSpeed,		m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rLadderRunSideSpeed,		m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rLadderRunDiagonalSpeed,	m_u8EncryptKey);
#endif

#if defined( MEM_ENCRYPTION_LV2)
		GlobalFunc::EncryptValue( &m_rAccelRateBackVsFront, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rAccelerationTime, m_u8EncryptKey);

		GlobalFunc::EncryptValue( &m_rWeaponSwap, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rScopeWaveVelocity, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rScopeWaveRange, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rReloadSpeed, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rShoulderSpeed, m_u8EncryptKey);

		for( INT32 i = 0; i < CHARA::eDAMAGEPART_COUNT; i++)
		{
			GlobalFunc::EncryptValue( &m_rPartDamageRate[ i],  m_u8EncryptKey);
		}
#endif

#if defined( MEM_ENCRYPTION_LV3)
		GlobalFunc::EncryptValue( &m_nHP, m_u8EncryptKey);
		for(INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
		{
			GlobalFunc::EncryptValue( &m_iResParts[i], m_u8EncryptKey);
		}
		for (INT32 x = 0; x < MMT_COUNT; x++)
		{
			for (INT32 i = 0; i < MST_COUNT; i++)
			{
				for (INT32 j = 0; j < MDT_COUNT; j++)
				{
					GlobalFunc::EncryptValue(&m_arMoveSpeed[x][i][j], m_u8EncryptKey);
				}
			}
		}
#endif
	}
}


void CCharaInfo2::ReadInfo( i3RegKey * pKey)
{
	SetHP(GetKeyData<INT32>(pKey, "HP"));

	SetITEMID( GetKeyData<INT32>(pKey, "ITEMID"));

	SetJumpHeight(GetKeyData<REAL32>(pKey, "JumpHeight"));
	SetJumpTime(GetKeyData<REAL32>(pKey, "JumpTime"));
	SetJumpDelay(GetKeyData<REAL32>(pKey, "JumpDelay"));
	SetJumpPenaltyTime(GetKeyData<REAL32>(pKey, "JumpPenaltyTime"));
	SetJumpPenaltyRate(GetKeyData<REAL32>(pKey, "JumpPenaltyRate"));

	SetWeaponSwap(GetKeyData<REAL32>(pKey, "WeaponSwap"));
	SetScopeWaveVelocity(GetKeyData<REAL32>(pKey, "ScopeWaveVelocity"));
	SetScopeWaveRange(GetKeyData<REAL32>(pKey, "ScopeWaveRange"));
	SetReloadSpeed(GetKeyData<REAL32>(pKey, "ReloadSpeed"));
	SetShoulderSpeed(GetKeyData<REAL32>(pKey, "ShoulderSpeed"));

	SetMoveSpeed(MST_RUN,MDT_FRONT,GetKeyData<REAL32>(pKey, "FrontMoveSpeed"));
	SetMoveSpeed(MST_RUN,MDT_SIDE,GetKeyData<REAL32>(pKey, "SideMoveSpeed"));
	SetMoveSpeed(MST_RUN,MDT_BACK,GetKeyData<REAL32>(pKey, "BackMoveSpeed"));
	SetMoveSpeed(MST_RUN,MDT_DIAGONAL,GetKeyData<REAL32>(pKey, "DiagonalMoveSpeed"));

	SetMoveSpeed(MST_WALK,MDT_FRONT,GetKeyData<REAL32>(pKey, "WalkFrontMove"));
	SetMoveSpeed(MST_WALK,MDT_SIDE,GetKeyData<REAL32>(pKey, "WalkSideMove"));
	SetMoveSpeed(MST_WALK,MDT_BACK,GetKeyData<REAL32>(pKey, "WalkBackMove"));
	SetMoveSpeed(MST_WALK,MDT_DIAGONAL,GetKeyData<REAL32>(pKey, "WalkDiagonalMove"));

	SetMoveSpeed(MST_JUMP_RUN, MDT_FRONT, GetKeyData<REAL32>(pKey, "JumpRunFrontMoveSpeed"));
	SetMoveSpeed(MST_JUMP_RUN, MDT_SIDE, GetKeyData<REAL32>(pKey, "JumpRunSideMoveSpeed"));
	SetMoveSpeed(MST_JUMP_RUN, MDT_BACK, GetKeyData<REAL32>(pKey, "JumpRunBackMoveSpeed"));
	SetMoveSpeed(MST_JUMP_RUN, MDT_DIAGONAL, GetKeyData<REAL32>(pKey, "JumpRunDiagonalMoveSpeed"));
	SetMoveSpeed(MST_JUMP_WALK, MDT_FRONT, GetKeyData<REAL32>(pKey, "JumpWalkFrontMoveSpeed"));
	SetMoveSpeed(MST_JUMP_WALK, MDT_SIDE, GetKeyData<REAL32>(pKey, "JumpWalkSideMoveSpeed"));
	SetMoveSpeed(MST_JUMP_WALK, MDT_BACK, GetKeyData<REAL32>(pKey, "JumpWalkBackMoveSpeed"));
	SetMoveSpeed(MST_JUMP_WALK, MDT_DIAGONAL, GetKeyData<REAL32>(pKey, "JumpWalkDiagonalMoveSpeed"));

	SetAccelRateBackVsFront(GetKeyData<REAL32>(pKey, "AccelRateBackVsFront"));
	SetAccelerationTime(GetKeyData<REAL32>(pKey, "AccelerationTime"));

	//자세에따른 카메라의 허리높이
	SetCamIKHeight(GetKeyData<VEC4D>(pKey, "CamIKHeight"));
	SetFPSCamPos(GetKeyData<VEC3D>(pKey, "FPSCamPos"));

	SetNPCAttackDelay(GetKeyData<REAL32>(pKey, "_NPCAttackDelay"));
	SetNPCInvincibleTime(GetKeyData<REAL32>(pKey, "_NPCInvincibleTime"));
	SetNPCRespawnTime(GetKeyData<REAL32>(pKey, "_NPCRespawnTime"));
	SetNPCSearchRandomValue(GetKeyData<REAL32>(pKey, "_NPCSearchRandomValue"));

	SetClunchJumpHeight(GetKeyData<REAL32>(pKey, "ClunchJumpHeight"));
	SetClunchJumpCheckTime(GetKeyData<REAL32>(pKey, "ClunchJumpCheckTime"));

	m_rSpeedReduction[CHPS_BODY].SetRate(GetKeyData<REAL32>(pKey, "SpeedReductionBody"));
	m_rSpeedReduction[CHPS_BODY].SetTime(GetKeyData<REAL32>(pKey, "SpeedReductionBodyTime"));

	m_rSpeedReduction[CHPS_ARM].SetRate(GetKeyData<REAL32>(pKey, "SpeedReductionArm"));
	m_rSpeedReduction[CHPS_ARM].SetTime(GetKeyData<REAL32>(pKey, "SpeedReductionArmTime"));

	m_rSpeedReduction[CHPS_HAND].SetRate(GetKeyData<REAL32>(pKey, "SpeedReductionHand"));
	m_rSpeedReduction[CHPS_HAND].SetTime(GetKeyData<REAL32>(pKey, "SpeedReductionHandTime"));

	m_rSpeedReduction[CHPS_LEG].SetRate(GetKeyData<REAL32>(pKey, "SpeedReductionLeg"));
	m_rSpeedReduction[CHPS_LEG].SetTime(GetKeyData<REAL32>(pKey, "SpeedReductionLegTime"));

	m_rSpeedReduction[CHPS_FOOT].SetRate(GetKeyData<REAL32>(pKey, "SpeedReductionFoot"));
	m_rSpeedReduction[CHPS_FOOT].SetTime(GetKeyData<REAL32>(pKey, "SpeedReductionFootTime"));

	

	i3RegKey * pDamageRateKey = (i3RegKey*) pKey->getChild( 0);
	if( pDamageRateKey != nullptr && i3::generic_is_iequal( pDamageRateKey->GetName(), "DamageRate") )
	{
		ReadDamageRateInfo( pDamageRateKey);
	}

	for (INT32 i=0; i<pKey->getChildCount(); i++)
	{
		i3RegKey* pChildKey = (i3RegKey*)pKey->getChild(i);
		I3ASSERT(pChildKey != nullptr);

		if( i3::generic_is_iequal( pChildKey->GetName(), "PartsList") )
		{
			for( INT32 j = 0; j < EQUIP::ePART_COUNT; j++)
			{
				m_iResParts[ j]	= GetKeyData<INT32>(pChildKey, sPartName[ j]);
			}
		}
		else if( i3::generic_is_iequal(pChildKey->GetName(), "Movement_ladder"))
		{
			m_rLadderWalkBackSpeed		= GetKeyData<REAL32>(pChildKey, "WalkBackSpeed");
			m_rLadderWalkFrontSpeed		= GetKeyData<REAL32>(pChildKey, "WalkFrontSpeed");
			m_rLadderWalkSideSpeed		= GetKeyData<REAL32>(pChildKey, "WalkSideSpeed");
			m_rLadderWalkDiagonalSpeed	= GetKeyData<REAL32>(pChildKey, "WalkDiagonalSpeed");
			m_rLadderRunBackSpeed		= GetKeyData<REAL32>(pChildKey, "RunBackSpeed");
			m_rLadderRunFrontSpeed		= GetKeyData<REAL32>(pChildKey, "RunFrontSpeed");
			m_rLadderRunSideSpeed		= GetKeyData<REAL32>(pChildKey, "RunSideSpeed");
			m_rLadderRunDiagonalSpeed	= GetKeyData<REAL32>(pChildKey, "RunDiagonalSpeed");

			break;
		}
		else if (i3::generic_is_iequal(pChildKey->GetName(), "Movement_Space") )
		{
			SetMoveSpeed(MST_RUN, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "FrontMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_RUN, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "SideMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_RUN, MDT_BACK, GetKeyData<REAL32>(pChildKey, "BackMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_RUN, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "DiagonalMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_WALK, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "WalkFrontMove"), MMT_SPACE);
			SetMoveSpeed(MST_WALK, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "WalkSideMove"), MMT_SPACE);
			SetMoveSpeed(MST_WALK, MDT_BACK, GetKeyData<REAL32>(pChildKey, "WalkBackMove"), MMT_SPACE);
			SetMoveSpeed(MST_WALK, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "WalkDiagonalMove"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_RUN, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "JumpRunFrontMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_RUN, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "JumpRunSideMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_RUN, MDT_BACK, GetKeyData<REAL32>(pChildKey, "JumpRunBackMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_RUN, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "JumpRunDiagonalMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_WALK, MDT_FRONT, GetKeyData<REAL32>(pChildKey, "JumpWalkFrontMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_WALK, MDT_SIDE, GetKeyData<REAL32>(pChildKey, "JumpWalkSideMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_WALK, MDT_BACK, GetKeyData<REAL32>(pChildKey, "JumpWalkBackMoveSpeed"), MMT_SPACE);
			SetMoveSpeed(MST_JUMP_WALK, MDT_DIAGONAL, GetKeyData<REAL32>(pChildKey, "JumpWalkDiagonalMoveSpeed"), MMT_SPACE);
		}
	}
}

void CCharaInfo2::OnEncryptInfo(UINT8 u8Key)
{
	m_u8EncryptKey = u8Key;

	{
#if defined( MEM_ENCRYPTION_LV1)
		GlobalFunc::EncryptValue(&m_vCamIKHeight, sizeof(m_vCamIKHeight), m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_vFPSCamPos, sizeof(m_vFPSCamPos), m_u8EncryptKey);

		GlobalFunc::EncryptValue(&m_rNPCAttackDelay, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rNPCInvincibleTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rNPCRespawnTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rNPCSearchRandomValue, m_u8EncryptKey);

		GlobalFunc::EncryptValue(&m_rClunchJumpHeight, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rClunchJumpCheckTime, m_u8EncryptKey);

		GlobalFunc::EncryptValue(&m_rLadderWalkBackSpeed, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rLadderWalkFrontSpeed, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rLadderWalkSideSpeed, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rLadderWalkDiagonalSpeed, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rLadderRunBackSpeed, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rLadderRunFrontSpeed, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rLadderRunSideSpeed, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rLadderRunDiagonalSpeed, m_u8EncryptKey);
#endif

#if defined( MEM_ENCRYPTION_LV2)
		GlobalFunc::EncryptValue(&m_rAccelRateBackVsFront, m_u8EncryptKey);

		GlobalFunc::EncryptValue(&m_rWeaponSwap, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rScopeWaveVelocity, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rScopeWaveRange, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rReloadSpeed, m_u8EncryptKey);
		GlobalFunc::EncryptValue(&m_rShoulderSpeed, m_u8EncryptKey);

#endif

#if defined( MEM_ENCRYPTION_LV3)
		GlobalFunc::EncryptValue(&m_nHP, m_u8EncryptKey);
		for (INT32 x = 0; x < MMT_COUNT; x++)
		{
			for (INT32 i = 0; i < MST_COUNT; i++)
			{
				for (INT32 j = 0; j < MDT_COUNT; j++)
				{
					GlobalFunc::EncryptValue(&m_arMoveSpeed[x][i][j], m_u8EncryptKey);
				}
			}
		}
		
		for(INT32 i = 0; i < EQUIP::ePART_COUNT; i++)
		{
			GlobalFunc::EncryptValue( &m_iResParts[i], m_u8EncryptKey);
		}
#endif	
		
		GlobalFunc::EncryptValue( &m_rAccelerationTime, m_u8EncryptKey);

		for( INT32 i = 0; i < CHARA::eDAMAGEPART_COUNT; i++)
		{
			GlobalFunc::EncryptValue( &m_rPartDamageRate[ i], m_u8EncryptKey);
		}

		////////////////////////////////////////////////////////////////////////

		GlobalFunc::EncryptValue( &m_rJumpHeight, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rJumpTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rJumpDelay, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rJumpPenaltyTime, m_u8EncryptKey);
		GlobalFunc::EncryptValue( &m_rJumpPenaltyRate, m_u8EncryptKey);
	}
}

const CSpeedReduction* CCharaInfo::GetSpeedReduction(CHARA_HIT_PART_SIMPLE ePart) const
{
	if( ePart > CHPS_UNDKNOWN &&  ePart < CHPS_COUNT )
		return &m_rSpeedReduction[ePart];

	return nullptr;
}

void CCharaInfo::ReadDamageRateInfo( i3RegKey * pKey)
{
	I3ASSERT( pKey != nullptr);

	SetPartDamageRate( CHARA::eDAMAGEPART_HEAD, GetKeyData<REAL32>(pKey, "Head"));
	SetPartDamageRate( CHARA::eDAMAGEPART_BODY, GetKeyData<REAL32>(pKey, "Body"));
	SetPartDamageRate( CHARA::eDAMAGEPART_ARM, GetKeyData<REAL32>(pKey, "Arm"));
	SetPartDamageRate( CHARA::eDAMAGEPART_HAND, GetKeyData<REAL32>(pKey, "Hand"));
	SetPartDamageRate( CHARA::eDAMAGEPART_LEG, GetKeyData<REAL32>(pKey, "Leg"));
	SetPartDamageRate( CHARA::eDAMAGEPART_FOOT, GetKeyData<REAL32>(pKey, "Foot"));
	SetPartDamageRate( CHARA::eDAMAGEPART_NECK, GetKeyData<REAL32>(pKey, "Neck"));
	SetPartDamageRate( CHARA::eDAMAGEPART_VITAL, GetKeyData<REAL32>(pKey, "Vital"));
	SetPartDamageRate( CHARA::eDAMAGEPART_OTHER, GetKeyData<REAL32>(pKey, "Other"));
}

REAL32 CCharaInfo::GetJumpDelay() const
{
	return m_rJumpDelay;
}
void CCharaInfo::SetJumpDelay(REAL32 val)
{
	m_rJumpDelay = val; 
}
REAL32 CCharaInfo::GetAccelerationTime() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rAccelerationTime, (UINT8)m_u8EncryptKey);
#else
	return m_rAccelerationTime;
#endif
}
void CCharaInfo::SetAccelerationTime(REAL32 val)
{
	m_rAccelerationTime = val; 
}

REAL32 CCharaInfo::GetMoveSpeed( EMoveSpeedType eMST, EMoveDirectionType eMDT, EMoveModeType eMMT ) const
{
	I3ASSERT( eMST < MST_COUNT);
	I3ASSERT( eMDT < MDT_COUNT);
	return GlobalFunc::DecryptValue( (REAL32*)&m_arMoveSpeed[eMMT][eMST][eMDT], m_u8EncryptKey);
}

void CCharaInfo::SetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, REAL32 Value, EMoveModeType eMMT )
{
	m_arMoveSpeed[eMMT][eMST][eMDT] = Value;
}

T_ItemID CCharaInfo::GetRepresentID( void) const
{
	return (T_ItemID) GlobalFunc::DecryptValue( (UINT32*) &m_RepresentID, m_u8EncryptKey);
}

INT32 CCharaInfo::GetHP() const
{
#if defined( MEM_ENCRYPTION_LV3)
	return GlobalFunc::DecryptValue( (INT32*)&m_nHP, m_u8EncryptKey);
#else

	return m_nHP;
#endif
}

void CCharaInfo::SetHP( INT32 val )
{
	m_nHP = val;
}

T_ItemID	CCharaInfo::GetPartsItemID(EQUIP::ePART parts) const
{
	I3ASSERT_RETURN(parts != EQUIP::ePART_UNKNOWN, 0);
#if defined(MEM_ENCRYPTION_LV3)
	return GlobalFunc::DecryptValue(  (INT32*)&m_iResParts[parts], m_u8EncryptKey);
#else
	return m_iResParts[parts];
#endif
}

T_ItemID	CCharaInfo2::GetPartsItemID(EQUIP::ePART parts)
{
#if defined(MEM_ENCRYPTION_LV3)
	return GlobalFunc::DecryptValue(  (INT32*)&m_iResParts[parts], m_u8EncryptKey);
#else
	return m_iResParts[parts];
#endif
}


REAL32 CCharaInfo::GetJumpHeight() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rJumpHeight, m_u8EncryptKey);
#else
	return m_rJumpHeight;
#endif
}

void CCharaInfo::SetJumpHeight( REAL32 val )
{
	m_rJumpHeight = val;
}
REAL32 CCharaInfo::GetJumpTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rJumpTime, m_u8EncryptKey);
#else
	return m_rJumpTime;
#endif
}

void CCharaInfo::SetNewTempJumpHeight(REAL32 val)
{
	if (!(val > 0.0f))
		val = 4.0f;

	m_rNewTempJumpHeight = val;
}
REAL32 CCharaInfo::GetNewTempJumpHeight() const
{
	return m_rNewTempJumpHeight;
}

void CCharaInfo::SetNewTempClunchJumpHeight(REAL32 val)
{
	if (!(val > 0.0f))
		val = 3.0f;

	m_rNewTempClunchJumpHeight = val;
}
REAL32 CCharaInfo::GetNewTempClunchJumpHeight() const
{
	return m_rNewTempClunchJumpHeight;
}

void CCharaInfo::SetJumpTime(REAL32 val)
{
	m_rJumpTime = val;
}

REAL32 CCharaInfo::GetJumpPenaltyTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rJumpPenaltyTime, m_u8EncryptKey);
#else
	return m_rJumpPenaltyTime;
#endif
}
void CCharaInfo::SetJumpPenaltyTime(REAL32 val)
{
	m_rJumpPenaltyTime = val;
}
REAL32 CCharaInfo::GetJumpPenaltyRate() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rJumpPenaltyRate, m_u8EncryptKey);
#else
	return m_rJumpPenaltyRate;
#endif
}
void CCharaInfo::SetJumpPenaltyRate(REAL32 val)
{
	m_rJumpPenaltyRate = val;
}
REAL32 CCharaInfo::GetWeaponSwap() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rWeaponSwap, m_u8EncryptKey);
#else
	return m_rWeaponSwap;
#endif
}
void CCharaInfo::SetWeaponSwap(REAL32 val)
{
	m_rWeaponSwap = val;
}
REAL32 CCharaInfo::GetScopeWaveVelocity() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rScopeWaveVelocity, m_u8EncryptKey);
#else
	return m_rScopeWaveVelocity;
#endif
}
void CCharaInfo::SetScopeWaveVelocity(REAL32 val)
{
	m_rScopeWaveVelocity = val;
}
REAL32 CCharaInfo::GetScopeWaveRange() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rScopeWaveRange, m_u8EncryptKey);
#else
	return m_rScopeWaveRange;
#endif
}
void CCharaInfo::SetScopeWaveRange(REAL32 val)
{
	m_rScopeWaveRange = val;
}
REAL32 CCharaInfo::GetReloadSpeed() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rReloadSpeed, m_u8EncryptKey);
#else
	return m_rReloadSpeed;
#endif
}
void CCharaInfo::SetReloadSpeed(REAL32 val)
{
	m_rReloadSpeed = val;
}
REAL32 CCharaInfo::GetShoulderSpeed() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rShoulderSpeed, m_u8EncryptKey);
#else
	return m_rShoulderSpeed;
#endif
}
void CCharaInfo::SetShoulderSpeed(REAL32 val)
{
	m_rShoulderSpeed = val;
}
REAL32 CCharaInfo::GetAccelRateBackVsFront() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rAccelRateBackVsFront, m_u8EncryptKey);
#else
	return m_rAccelRateBackVsFront;
#endif
}
void CCharaInfo::SetAccelRateBackVsFront(REAL32 val)
{
	m_rAccelRateBackVsFront = val;
}
void CCharaInfo::GetCamIKHeight( VEC4D * pOut) const
{	
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::DecryptValue( pOut, (VEC4D*) &m_vCamIKHeight, m_u8EncryptKey);
#else
	i3Vector::Copy( pOut, (VEC4D*) &m_vCamIKHeight);
#endif
}
void CCharaInfo::SetCamIKHeight(const VEC4D & val)
{
	m_vCamIKHeight = val;
}
void CCharaInfo::GetFPSCamPos( VEC3D * pOut) const
{
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::DecryptValue( pOut, (VEC3D*) &m_vFPSCamPos, m_u8EncryptKey);
#else
	i3Vector::Copy( pOut, (VEC3D*) &m_vFPSCamPos);
#endif
}
void CCharaInfo::SetFPSCamPos(const VEC3D & val)
{
	m_vFPSCamPos = val;
}

REAL32 CCharaInfo::GetPartDamageRate( CHARA::eDAMAGEPART damagePart) const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rPartDamageRate[ damagePart], m_u8EncryptKey);
#else
	return m_rPartDamageRate[ damagePart];
#endif
}

void CCharaInfo::SetPartDamageRate( CHARA::eDAMAGEPART damagePart, REAL32 val)
{
	m_rPartDamageRate[ damagePart] = val;
}

CHARA::eSEX_TYPE CCharaInfo::GetCharaSexType() const
{
	return (CHARA::eSEX_TYPE) m_nCharaSexType;
}

CHARA::eSEX_TYPE CCharaInfo::GetCharaSexTypePHYSIX() const
{
	int cal_type;
	cal_type = m_nCharaSexType;
	if (cal_type >= CHARA::eSEX_TYPE_SKINY_MALE) cal_type = (cal_type % 2);

	return (CHARA::eSEX_TYPE)cal_type;
}

CHARA::eSEX_TYPE CCharaInfo::GetCharaSexTypeDISPLAY() const
{
	int cal_type;
	cal_type = m_nCharaSexType;
	if (cal_type >= CHARA::eSEX_TYPE_SKINY_MALE)
	{
		cal_type = (cal_type - CHARA::eSEX_TYPE_SKINY_MALE);
		cal_type %= 2;
	}
		
	return (CHARA::eSEX_TYPE)cal_type;
}

REAL32 CCharaInfo::GetNPCAttackDelay() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rNPCAttackDelay, m_u8EncryptKey);
#else
	return m_rNPCAttackDelay;
#endif
}
void CCharaInfo::SetNPCAttackDelay(REAL32 val)
{
	m_rNPCAttackDelay = val;
}

REAL32 CCharaInfo::GetNPCInvincibleTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rNPCInvincibleTime, m_u8EncryptKey);
#else
	return m_rNPCInvincibleTime;
#endif
}
void CCharaInfo::SetNPCInvincibleTime(REAL32 val)
{
	m_rNPCInvincibleTime = val;
}
REAL32 CCharaInfo::GetNPCRespawnTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rNPCRespawnTime, m_u8EncryptKey);
#else
	return m_rNPCRespawnTime;
#endif
}
void CCharaInfo::SetNPCRespawnTime(REAL32 val)
{
	m_rNPCRespawnTime = val;
}
REAL32 CCharaInfo::GetNPCSearchRandomValue() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rNPCSearchRandomValue, m_u8EncryptKey);
#else
	return m_rNPCSearchRandomValue;
#endif
}
void CCharaInfo::SetNPCSearchRandomValue(REAL32 val)
{
	m_rNPCSearchRandomValue = val;
}
REAL32 CCharaInfo::GetClunchJumpHeight() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rClunchJumpHeight, m_u8EncryptKey);
#else
	return m_rClunchJumpHeight;
#endif
}
void CCharaInfo::SetClunchJumpHeight(REAL32 val)
{
	m_rClunchJumpHeight = val;
}
REAL32 CCharaInfo::GetClunchJumpCheckTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rClunchJumpCheckTime, m_u8EncryptKey);
#else
	return m_rClunchJumpCheckTime;
#endif
}
void CCharaInfo::SetClunchJumpCheckTime(REAL32 val)
{
	m_rClunchJumpCheckTime = val;
}

REAL32 CCharaInfo::GetLadderWalkBackSpeed()	const	
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rLadderWalkBackSpeed, m_u8EncryptKey);
#else
	return m_rLadderWalkBackSpeed;
#endif
}

REAL32 CCharaInfo::GetLadderWalkFrontSpeed() const	
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rLadderWalkFrontSpeed, m_u8EncryptKey);
#else
	return m_rLadderWalkFrontSpeed;
#endif	
}

REAL32 CCharaInfo::GetLadderWalkSideSpeed()	const	
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rLadderWalkSideSpeed, m_u8EncryptKey);
#else
	return m_rLadderWalkSideSpeed;
#endif	
}

REAL32 CCharaInfo::GetLadderWalkDiagonalSpeed()	const 
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rLadderWalkDiagonalSpeed, m_u8EncryptKey);
#else
	return m_rLadderWalkDiagonalSpeed;
#endif
}

REAL32 CCharaInfo::GetLadderRunBackSpeed() const		
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rLadderRunBackSpeed, m_u8EncryptKey);
#else
	return m_rLadderRunBackSpeed;
#endif			
}

REAL32 CCharaInfo::GetLadderRunFrontSpeed() const		
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rLadderRunFrontSpeed, m_u8EncryptKey);
#else
	return m_rLadderRunFrontSpeed;
#endif	
}

REAL32 CCharaInfo::GetLadderRunSideSpeed() const		
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rLadderRunSideSpeed, m_u8EncryptKey);
#else
	return m_rLadderRunSideSpeed;
#endif		
}

REAL32 CCharaInfo::GetLadderRunDiagonalSpeed() const	
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rLadderRunDiagonalSpeed, m_u8EncryptKey);
#else
	return m_rLadderRunDiagonalSpeed;
#endif	
}




void CCharaInfo2::ReadDamageRateInfo( i3RegKey * pKey)
{
	I3ASSERT( pKey != nullptr);
	SetPartDamageRate( CHARA::eDAMAGEPART_HEAD, GetKeyData<REAL32>(pKey, "Head"));
	SetPartDamageRate( CHARA::eDAMAGEPART_BODY, GetKeyData<REAL32>(pKey, "Body"));
	SetPartDamageRate( CHARA::eDAMAGEPART_ARM, GetKeyData<REAL32>(pKey, "Arm"));
	SetPartDamageRate( CHARA::eDAMAGEPART_HAND, GetKeyData<REAL32>(pKey, "Hand"));
	SetPartDamageRate( CHARA::eDAMAGEPART_LEG, GetKeyData<REAL32>(pKey, "Leg"));
	SetPartDamageRate( CHARA::eDAMAGEPART_FOOT, GetKeyData<REAL32>(pKey, "Foot"));
	SetPartDamageRate( CHARA::eDAMAGEPART_NECK, GetKeyData<REAL32>(pKey, "Neck"));
	SetPartDamageRate( CHARA::eDAMAGEPART_VITAL, GetKeyData<REAL32>(pKey, "Vital"));
	SetPartDamageRate( CHARA::eDAMAGEPART_OTHER, GetKeyData<REAL32>(pKey, "Other"));
}

REAL32 CCharaInfo2::GetAccelerationTime() const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rAccelerationTime, m_u8EncryptKey);
}
void CCharaInfo2::SetAccelerationTime(REAL32 val)
{
	m_rAccelerationTime = val; 
}
REAL32 CCharaInfo2::GetJumpDelay() const
{
	return m_rJumpDelay;
}
void CCharaInfo2::SetJumpDelay(REAL32 val)
{
	m_rJumpDelay = val; 
}

const CSpeedReduction* CCharaInfo2::GetSpeedReduction(CHARA_HIT_PART_SIMPLE ePart) const
{
	if( ePart > CHPS_UNDKNOWN &&  ePart < CHPS_COUNT )
	{
		return &m_rSpeedReduction[ePart];
	}

	return nullptr;
}
REAL32 CCharaInfo2::GetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, EMoveModeType eMMT) const
{
	I3ASSERT(eMST < MST_COUNT);
	I3ASSERT(eMDT < MDT_COUNT);
	return GlobalFunc::DecryptValue((REAL32*)&m_arMoveSpeed[eMMT][eMST][eMDT], m_u8EncryptKey);
}
void CCharaInfo2::SetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, REAL32 Value, EMoveModeType eMMT)
{
	m_arMoveSpeed[eMMT][eMST][eMDT] = Value;
}
REAL32 CCharaInfo2::GetJumpHeight() const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rJumpHeight, m_u8EncryptKey);
}
void CCharaInfo2::SetJumpHeight(REAL32 val)
{
	m_rJumpHeight = val; 
}
INT32 CCharaInfo2::GetHP() const
{
#if defined( MEM_ENCRYPTION_LV3)
	return GlobalFunc::DecryptValue( (INT32*) &m_nHP, m_u8EncryptKey);
#else
	return m_nHP; 
#endif
}
void CCharaInfo2::SetHP(INT32 val)
{
	m_nHP = val; 
}
REAL32 CCharaInfo2::GetJumpTime() const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rJumpTime, m_u8EncryptKey);
}
void CCharaInfo2::SetJumpTime(REAL32 val)
{
	m_rJumpTime = val; 
}
REAL32 CCharaInfo2::GetJumpPenaltyTime() const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rJumpPenaltyTime, m_u8EncryptKey);
}
void CCharaInfo2::SetJumpPenaltyTime(REAL32 val)
{
	m_rJumpPenaltyTime = val; 
}

REAL32 CCharaInfo2::GetJumpPenaltyRate() const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rJumpPenaltyRate, m_u8EncryptKey);
}
void CCharaInfo2::SetJumpPenaltyRate(REAL32 val)
{
	m_rJumpPenaltyRate = val; 
}
REAL32 CCharaInfo2::GetWeaponSwap() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rWeaponSwap, m_u8EncryptKey);
#else
	return m_rWeaponSwap; 
#endif
}
void CCharaInfo2::SetWeaponSwap(REAL32 val)
{
	m_rWeaponSwap = val; 
}
REAL32 CCharaInfo2::GetScopeWaveRange() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rScopeWaveRange, m_u8EncryptKey);
#else
	return m_rScopeWaveRange; 
#endif
}
void CCharaInfo2::SetScopeWaveRange(REAL32 val)
{
	m_rScopeWaveRange = val; 
}
REAL32 CCharaInfo2::GetShoulderSpeed() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rShoulderSpeed, m_u8EncryptKey);
#else
	return m_rShoulderSpeed; 
#endif
}
void CCharaInfo2::SetShoulderSpeed(REAL32 val)
{
	m_rShoulderSpeed = val; 
}
void CCharaInfo2::GetCamIKHeight( VEC4D * pOut) const
{
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::DecryptValue( pOut, (VEC4D*) &m_vCamIKHeight, m_u8EncryptKey);
#else
	i3Vector::Copy( pOut, (VEC4D*) &m_vCamIKHeight);
#endif
}
void CCharaInfo2::SetCamIKHeight(const VEC4D & val)
{
	m_vCamIKHeight = val; 
}
REAL32 CCharaInfo2::GetReloadSpeed() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rReloadSpeed, m_u8EncryptKey);
#else
	return m_rReloadSpeed; 
#endif
}
void CCharaInfo2::SetReloadSpeed(REAL32 val)
{
	m_rReloadSpeed = val; 
}
REAL32 CCharaInfo2::GetAccelRateBackVsFront() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rAccelRateBackVsFront, m_u8EncryptKey);
#else
	return m_rAccelRateBackVsFront; 
#endif
}
void CCharaInfo2::SetAccelRateBackVsFront(REAL32 val)
{
	m_rAccelRateBackVsFront = val; 
}
REAL32 CCharaInfo2::GetScopeWaveVelocity() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rScopeWaveVelocity, m_u8EncryptKey);
#else
	return m_rScopeWaveVelocity; 
#endif
}
void CCharaInfo2::SetScopeWaveVelocity(REAL32 val)
{
	m_rScopeWaveVelocity = val; 
}
void CCharaInfo2::GetFPSCamPos( VEC3D * pOut) const
{
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::DecryptValue( pOut, (VEC3D*) &m_vFPSCamPos, m_u8EncryptKey);
#else
	i3Vector::Copy( pOut, (VEC3D*) &m_vFPSCamPos);
#endif
}
void CCharaInfo2::SetFPSCamPos(const VEC3D & val)
{
	m_vFPSCamPos = val; 
}

REAL32 CCharaInfo2::GetPartDamageRate( CHARA::eDAMAGEPART damagePart) const
{
	return GlobalFunc::DecryptValue( (REAL32*) &m_rPartDamageRate[ damagePart], m_u8EncryptKey);
}

void CCharaInfo2::SetPartDamageRate( CHARA::eDAMAGEPART damagePart, REAL32 val)
{
	m_rPartDamageRate[ damagePart] = val;
}


REAL32 CCharaInfo2::GetNPCAttackDelay() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rNPCAttackDelay, m_u8EncryptKey);
#else
	return m_rNPCAttackDelay; 
#endif
}
void CCharaInfo2::SetNPCAttackDelay(REAL32 val)
{
	m_rNPCAttackDelay = val; 
}
REAL32 CCharaInfo2::GetNPCRespawnTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rNPCRespawnTime, m_u8EncryptKey);
#else
	return m_rNPCRespawnTime; 
#endif
}
void CCharaInfo2::SetNPCRespawnTime(REAL32 val)
{
	m_rNPCRespawnTime = val; 
}
REAL32 CCharaInfo2::GetNPCInvincibleTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rNPCInvincibleTime, m_u8EncryptKey);
#else
	return m_rNPCInvincibleTime; 
#endif
}
void CCharaInfo2::SetNPCInvincibleTime(REAL32 val)
{
	m_rNPCInvincibleTime = val; 
}
REAL32 CCharaInfo2::GetClunchJumpHeight() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rClunchJumpHeight, m_u8EncryptKey);
#else
	return m_rClunchJumpHeight; 
#endif
}
void CCharaInfo2::SetClunchJumpHeight(REAL32 val)
{
	m_rClunchJumpHeight = val; 
}
REAL32 CCharaInfo2::GetNPCSearchRandomValue() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rNPCSearchRandomValue, m_u8EncryptKey);
#else
	return m_rNPCSearchRandomValue; 
#endif
}
void CCharaInfo2::SetNPCSearchRandomValue(REAL32 val)
{
	m_rNPCSearchRandomValue = val; 
}
REAL32 CCharaInfo2::GetClunchJumpCheckTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rClunchJumpCheckTime, m_u8EncryptKey);
#else
	return m_rClunchJumpCheckTime; 
#endif
}
void CCharaInfo2::SetClunchJumpCheckTime(REAL32 val)
{
	m_rClunchJumpCheckTime = val; 
}

void CCharaInfo2::SetITEMID( T_ItemID val)
{
	m_ITEMID = val;
}

T_ItemID CCharaInfo2::GetITEMID( void)
{
#if defined( MEM_ENCRYPTION_LV1)
	return (T_ItemID) GlobalFunc::DecryptValue( (INT32*)&m_ITEMID, m_u8EncryptKey);
#else
	return m_ITEMID; 
#endif
}

REAL32 CCharaInfo2::GetLadderWalkBackSpeed() const		
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rLadderWalkBackSpeed, m_u8EncryptKey);
#else
	return m_rLadderWalkBackSpeed; 
#endif		
}

REAL32 CCharaInfo2::GetLadderWalkFrontSpeed() const	
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rLadderWalkFrontSpeed, m_u8EncryptKey);
#else
	return m_rLadderWalkFrontSpeed; 	
#endif
}

REAL32 CCharaInfo2::GetLadderWalkSideSpeed() const	
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rLadderWalkSideSpeed, m_u8EncryptKey);
#else
	return m_rLadderWalkSideSpeed; 
#endif			
}

REAL32 CCharaInfo2::GetLadderWalkDiagonalSpeed() const	
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rLadderWalkDiagonalSpeed, m_u8EncryptKey);
#else
	return m_rLadderWalkDiagonalSpeed; 
#endif		
}

REAL32 CCharaInfo2::GetLadderRunBackSpeed() const			
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rLadderRunBackSpeed, m_u8EncryptKey);
#else
	return m_rLadderRunBackSpeed; 
#endif		
}

REAL32 CCharaInfo2::GetLadderRunFrontSpeed() const		
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rLadderRunFrontSpeed, m_u8EncryptKey);
#else
	return m_rLadderRunFrontSpeed; 
#endif			
}

REAL32 CCharaInfo2::GetLadderRunSideSpeed() const			
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rLadderRunSideSpeed, m_u8EncryptKey);
#else
	return m_rLadderRunSideSpeed; 
#endif		
}

REAL32 CCharaInfo2::GetLadderRunDiagonalSpeed() const		
{ 
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*)&m_rLadderRunDiagonalSpeed, m_u8EncryptKey);
#else
	return m_rLadderRunDiagonalSpeed; 
#endif	
}

void CUITemplateInfo_Chara::ReadInfo(i3RegKey* rk)
{
	FIND_REG_DATA(rk, "Type", &m_type);

	i3::rc_wstring wstrName;

	FIND_REG_DATA(rk, "Name", wstrName);

	if( g_pEnvSet->IsV2Version() )
	{
		FIND_REG_DATA(rk, "Name_V2", wstrName);
		FIND_REG_DATA(rk, "LoopLoad_V2", &m_LoopLoad);
	}
	else
	{
		FIND_REG_DATA(rk, "Name_V1", wstrName);
		FIND_REG_DATA(rk, "LoopLoad_V1", &m_LoopLoad);
	}

	i3::utf16_to_mb(wstrName, m_name);
}

INT32 CUITemplateInfo_Chara::GetType() const
{
	return m_type;
}

const i3::string& CUITemplateInfo_Chara::GetName() const
{
	return m_name;
}

INT32 CUITemplateInfo_Chara::GetLoopLoad() const
{
	return m_LoopLoad;
}