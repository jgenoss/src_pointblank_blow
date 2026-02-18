#include "pch.h"
#include "DinoInfo.h"

I3_CLASS_INSTANCE( CDinoInfo);

CDinoInfo::CDinoInfo( void)
{
	i3mem::Fill(m_UIShapeIndex_CCSKill, -1, sizeof(m_UIShapeIndex_CCSKill));
	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		m_rDinoHP_Ratio[i] = 1.0f;
}

void CDinoInfo::ReadInfo( i3RegKey * pKey)
{
	CCharaInfo::ReadInfo( pKey);

	SetDinoWeaponID(0, GetKeyData<INT32>(pKey, "DinoWeaponID0") );
	SetDinoWeaponID(1, GetKeyData<INT32>(pKey, "DinoWeaponID1") );
	SetDinoWeaponID(2, GetKeyData<INT32>(pKey, "DinoWeaponID2") );

	SetDinoHP_Ratio(0, GetKeyData<REAL32>(pKey, "HP_Ratio_2") );
	SetDinoHP_Ratio(1, GetKeyData<REAL32>(pKey, "HP_Ratio_3") );
	SetDinoHP_Ratio(2, GetKeyData<REAL32>(pKey, "HP_Ratio_4") );
	SetDinoHP_Ratio(3, GetKeyData<REAL32>(pKey, "HP_Ratio_5") );
	SetDinoHP_Ratio(4, GetKeyData<REAL32>(pKey, "HP_Ratio_6") );
	SetDinoHP_Ratio(5, GetKeyData<REAL32>(pKey, "HP_Ratio_7") );
	SetDinoHP_Ratio(6, GetKeyData<REAL32>(pKey, "HP_Ratio_8") );
	SetDinoHP_Ratio(7, GetKeyData<REAL32>(pKey, "HP_Ratio_9") );
	SetDinoHP_Ratio(8, GetKeyData<REAL32>(pKey, "HP_Ratio_10") );
	SetDinoHP_Ratio(9, GetKeyData<REAL32>(pKey, "HP_Ratio_11") );
	SetDinoHP_Ratio(10, GetKeyData<REAL32>(pKey, "HP_Ratio_12") );
	SetDinoHP_Ratio(11, GetKeyData<REAL32>(pKey, "HP_Ratio_13") );
	SetDinoHP_Ratio(12, GetKeyData<REAL32>(pKey, "HP_Ratio_14") );
	SetDinoHP_Ratio(13, GetKeyData<REAL32>(pKey, "HP_Ratio_15") );
	SetDinoHP_Ratio(14, GetKeyData<REAL32>(pKey, "HP_Ratio_16") );

	SetDinoCCRadius( GetKeyData<REAL32>(pKey, "DinoCCRadius") );
	SetDinoCCTall( GetKeyData<REAL32>(pKey, "DinoCCTall") );
	SetDinoCCStepOffset( GetKeyData<REAL32>(pKey, "DinoCCStepOffset") );

	SetFootStepTrembleAmp( GetKeyData<REAL32>(pKey, "ShakeFootStep") );

	char szCCSkill[MAX_STRING_COUNT] = {0,};
	for( UINT8 cnt = 0; cnt < CC_SKILL_MAX; ++cnt)
	{
		sprintf_s( szCCSkill, MAX_STRING_COUNT, "UIShapeIndex_CCSkill%d", (cnt + 1));
		FIND_REG_DATA( pKey, szCCSkill, &m_UIShapeIndex_CCSKill[cnt]);

		sprintf_s( szCCSkill, MAX_STRING_COUNT, "CCSkillDesc_%d", (cnt + 1));
		FIND_REG_DATA(pKey, szCCSkill, m_astrCCSkillDesc[cnt]);
	}
}

void CDinoInfo::OnEncryptInfo( UINT8 u8Key)
{
	CCharaInfo::OnEncryptInfo( u8Key);

#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::EncryptValue( &m_rRexFootStepTrembleAmp, m_u8EncryptKey);
#endif

#if defined( MEM_ENCRYPTION_LV2)
	for( UINT8 cnt = 0; cnt < CC_SKILL_MAX; ++cnt)
	{
		GlobalFunc::EncryptValue( &m_UIShapeIndex_CCSKill[cnt], m_u8EncryptKey);

		GlobalFunc::EncryptValue( m_astrCCSkillDesc[cnt], m_u8EncryptKey);
	}
#endif

	GlobalFunc::EncryptValue( &m_rDinoCCStepOffset, m_u8EncryptKey);
	GlobalFunc::EncryptValue( &m_rDinoCCRadius, m_u8EncryptKey);
	GlobalFunc::EncryptValue( &m_rDinoCCTall, m_u8EncryptKey);

	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		GlobalFunc::EncryptValue( &m_rDinoHP_Ratio[ i], m_u8EncryptKey);

	for(INT32 i = 0 ; i < DINO_WEAPON_ARRAY_COUNT ; ++i)
		GlobalFunc::EncryptValue(&m_ui32DinoWeaponID[i], m_u8EncryptKey);
}

T_ItemID	CDinoInfo::GetDinoWeaponID(INT32 idx) const
{
	I3_BOUNDCHK(idx, DINO_WEAPON_ARRAY_COUNT);

	return (T_ItemID) GlobalFunc::DecryptValue((UINT32*)&m_ui32DinoWeaponID[ idx], m_u8EncryptKey);
}

void	CDinoInfo::SetDinoWeaponID(INT32 idx, T_ItemID itemID)
{
	I3_BOUNDCHK(idx, DINO_WEAPON_ARRAY_COUNT);
	m_ui32DinoWeaponID[idx] = itemID;
}

REAL32		CDinoInfo::GetDinoHP_Ratio(INT32 idx) const
{
	I3_BOUNDCHK(idx, SLOT_MAX_COUNT);

	return GlobalFunc::DecryptValue( (REAL32*)&m_rDinoHP_Ratio[idx], m_u8EncryptKey);
}

void		CDinoInfo::SetDinoHP_Ratio(INT32 idx, REAL32 val)
{
	I3_BOUNDCHK(idx, SLOT_MAX_COUNT);
	m_rDinoHP_Ratio[idx] = val;
}

REAL32		CDinoInfo::GetDinoCCRadius() const
{
	return m_rDinoCCRadius;
}

REAL32		CDinoInfo::GetDinoCCTall() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rDinoCCTall, m_u8EncryptKey);
#else
	return m_rDinoCCTall;
#endif
}

REAL32		CDinoInfo::GetDinoCCStepOffset() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rDinoCCStepOffset, m_u8EncryptKey);
#else
	return m_rDinoCCStepOffset;
#endif
}

REAL32		CDinoInfo::getFootStepTrembleAmp(void) const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( (REAL32*) &m_rRexFootStepTrembleAmp, m_u8EncryptKey);
#else
	return m_rRexFootStepTrembleAmp;
#endif
}

INT32		CDinoInfo::getShapeIndexCCSkill( UINT8 idx) const
{
	I3ASSERT( idx < CC_SKILL_MAX);

#if defined( MEM_ENCRYPTION) && defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue( (INT32*) &m_UIShapeIndex_CCSKill[idx], m_u8EncryptKey);
#else
	return m_UIShapeIndex_CCSKill[idx];
#endif
}

void		CDinoInfo::getCCSkillDescription( i3::rc_wstring& out, UINT8 idx) const
{
	I3ASSERT( idx < CC_SKILL_MAX);

#if defined( MEM_ENCRYPTION) && defined( MEM_ENCRYPTION_LV2)
	GlobalFunc::DecryptValue( out, m_astrCCSkillDesc[idx], m_u8EncryptKey);
#else
	//i3::string_ncopy_nullpad( pszOut, m_szCCSkillDesc[idx], sizeof( m_szCCSkillDesc[idx]));
	out = m_astrCCSkillDesc[idx];
#endif
}

void		CDinoInfo::SetFootStepTrembleAmp(REAL32 val)
{
	m_rRexFootStepTrembleAmp = val;
}