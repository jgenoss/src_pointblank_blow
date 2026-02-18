#include "pch.h"
#include "TitleInfo.h"

I3_CLASS_INSTANCE( CTitleInfo); //, i3ElementBase);

enum
{
	ENC_SEED_GAINED	= 3,
	ENC_SEED_EQUIP	= 4,
	ENC_SEED_LIMIT	= 5,
};

CTitleInfo::CTitleInfo()
{
	i3mem::FillZero(m_GainedUserTitle, sizeof(m_GainedUserTitle));
	i3mem::FillZero(m_EquipUserTitle, sizeof(m_EquipUserTitle));
	m_LimitUserTitleCount	= 0;
}

UINT8 CTitleInfo::GetGainedTitle( INT32 idx ) const
{
	if ( idx < MAX_USER_TITLE )
	{
		UINT8	GainedTitle[MAX_USER_TITLE];
		GetGainedTitle(GainedTitle);
		return GainedTitle[idx];
	}
	return 0;
}

void CTitleInfo::GetGainedTitle(UINT8 OutTitle[MAX_USER_TITLE]) const
{
	i3mem::Copy(OutTitle, m_GainedUserTitle, sizeof(m_GainedUserTitle));
	GlobalFunc::DecryptValue(OutTitle, sizeof(m_GainedUserTitle), ENC_SEED_GAINED);
}

void CTitleInfo::SetGainedTitle( INT32 idx, UINT8 val )
{
	if ( idx < 0 || idx >= MAX_USER_TITLE )
		return;

	UINT8	GainedTitle[MAX_USER_TITLE] = {};
	GetGainedTitle(GainedTitle);

	GainedTitle[idx] = val;

	GlobalFunc::EncryptValue(GainedTitle, sizeof(GainedTitle), ENC_SEED_GAINED);
	i3mem::Copy(m_GainedUserTitle, GainedTitle, sizeof(GainedTitle));
}

void CTitleInfo::SetGainedTitle( UINT64 GainedTitle )
{
	UINT8	GainedTitleArray[MAX_USER_TITLE] = {};

	for( UINT32 i = 0 ; i < MAX_USER_TITLE ; i++ )
	{
		GainedTitleArray[i] = (1 == ( GainedTitle & 0x1 ));
		GainedTitle = GainedTitle >> 1;
	}

	GlobalFunc::EncryptValue(GainedTitleArray, sizeof(GainedTitleArray), ENC_SEED_GAINED);
	i3mem::Copy(m_GainedUserTitle, GainedTitleArray, sizeof(GainedTitleArray));
}

UINT8 CTitleInfo::GetEquipTitle( INT32 idx ) const
{
	if ( idx < MAX_EQUIP_USER_TITLE )
	{
		UINT8	EquipTitle[MAX_EQUIP_USER_TITLE];
		GetEquipTitle(EquipTitle);
		return EquipTitle[idx];
	}
	return 0;
}

void CTitleInfo::GetEquipTitle( UINT8 OutTitle[MAX_EQUIP_USER_TITLE] ) const
{
	i3mem::Copy(OutTitle, m_EquipUserTitle, sizeof(m_EquipUserTitle));
	GlobalFunc::DecryptValue(OutTitle, sizeof(m_EquipUserTitle), ENC_SEED_EQUIP);
}

void CTitleInfo::SetEquipTitle( INT32 idx, UINT8 title )
{
	if ( idx < 0 || idx >= MAX_EQUIP_USER_TITLE )
		return;

	UINT8	EquipTitle[MAX_EQUIP_USER_TITLE] = {};
	GetEquipTitle(EquipTitle);

	EquipTitle[idx] = title;

	GlobalFunc::EncryptValue(EquipTitle, sizeof(EquipTitle), ENC_SEED_EQUIP);
	i3mem::Copy(m_EquipUserTitle, EquipTitle, sizeof(EquipTitle));

}

void CTitleInfo::SetEquipTitle( const UINT8 title[MAX_EQUIP_USER_TITLE] )
{
	UINT8 _title[MAX_EQUIP_USER_TITLE];
	i3mem::Copy(_title, title, sizeof(_title));
	GlobalFunc::EncryptValue(_title, sizeof(_title), ENC_SEED_EQUIP);
	i3mem::Copy(m_EquipUserTitle, _title, sizeof(m_EquipUserTitle));
}

INT32 CTitleInfo::GetLimitCount() const
{
	return GlobalFunc::DecryptValue((INT32 *)&m_LimitUserTitleCount, ENC_SEED_LIMIT);
}

void CTitleInfo::SetLimitCount( INT32 count )
{
	GlobalFunc::EncryptValue(&count, ENC_SEED_LIMIT);
	m_LimitUserTitleCount = count;
}

INT32 CTitleInfo::HaveTitle( UINT8 title ) const
{
	INT32 LimitCount = GetLimitCount();
	UINT8 EquipTitle[MAX_EQUIP_USER_TITLE];
	GetEquipTitle(EquipTitle);
	for ( INT32 i = 0 ; i < LimitCount ; ++i )
	{
		if ( EquipTitle[i] == title )
			return i;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE( CTitleNetTempInfo);//, i3ElementBase);

CTitleNetTempInfo::CTitleNetTempInfo()
{
	m_GainedIdx				= 0;
	m_UploadSlotIdx			= 0;
	m_UploadDesignationIdx	= 0;
	m_ReleaseSlotIdx		= 0;
}
