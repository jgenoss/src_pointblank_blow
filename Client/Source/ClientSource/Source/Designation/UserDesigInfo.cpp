#include "pch.h"
#include "UserDesigInfo.h"

enum
{
	ENC_SEED_GAINED	= 6,
	ENC_SEED_EQUIP	= 7,
	ENC_SEED_LIMIT	= 8,
};

void	CUserDesigInfo::GetGainedDesig(UINT8 (&OutDesigSetTypes)[MAX_USER_TITLE]) const
{
	::memcpy(OutDesigSetTypes, m_aGainedDesig, sizeof(m_aGainedDesig));
	GlobalFunc::DecryptValue(OutDesigSetTypes, sizeof(m_aGainedDesig), ENC_SEED_GAINED);
}

bool	CUserDesigInfo::IsGainedDesig(INT32 DesigSetType) const
{
	if ( DesigSetType < MAX_USER_TITLE )
	{
		UINT8	GainedTitle[MAX_USER_TITLE];
		GetGainedDesig(GainedTitle);
		return (GainedTitle[DesigSetType] == 1);		// 의도적으로 != 0 대신, == 1 로 사용..
	}
	return 0;	
}

void	CUserDesigInfo::SetGainedDesigWithFlag(UINT64 DesigTypeGainFlag)
{
	bool	GainedDesigArray[MAX_USER_TITLE] = {0, };

	for( UINT32 i = 0 ; i < MAX_USER_TITLE ; i++ )
	{
		GainedDesigArray[i] = (1 == ( DesigTypeGainFlag & 0x1 ) );
		DesigTypeGainFlag = DesigTypeGainFlag >> 1;
	}

	GlobalFunc::EncryptValue(GainedDesigArray, sizeof(GainedDesigArray), ENC_SEED_GAINED);
	::memcpy(m_aGainedDesig, GainedDesigArray, sizeof(GainedDesigArray));
}

void	CUserDesigInfo::SetGainedDesig(INT32 DesigSetType, bool bGained)
{
	if ( DesigSetType < 0 || DesigSetType >= MAX_USER_TITLE )
		return;

	UINT8	GainedDesig[MAX_USER_TITLE] = { 0, };
	GetGainedDesig(GainedDesig);

	GainedDesig[DesigSetType] = (bGained == 1);		// 의도적으로 != 0 대신, == 1 로 사용..

	GlobalFunc::EncryptValue(GainedDesig, sizeof(GainedDesig), ENC_SEED_GAINED);
	::memcpy(m_aGainedDesig, GainedDesig, sizeof(GainedDesig));
}

UINT8	CUserDesigInfo::GetEquipDesig(INT32 desigSlot) const
{
	if ( desigSlot < MAX_EQUIP_USER_TITLE )
	{
		UINT8	EquipTitle[MAX_EQUIP_USER_TITLE];
		GetEquipDesig(EquipTitle);
		return EquipTitle[desigSlot];
	}
	return 0;
}

void	CUserDesigInfo::SetEquipDesig(INT32 desigSlot, UINT8 desigSetType)
{
	if ( desigSlot < 0 || desigSlot >= MAX_EQUIP_USER_TITLE )
		return;

	UINT8	EquipTitle[MAX_EQUIP_USER_TITLE] = { 0, };
	GetEquipDesig(EquipTitle);

	bool check_dupicate = false;

	if( desigSetType != 0x00 ) //0x00은 빈 공간으로 변경할때 오는 값.
	{
		for( INT32 check_index = 0 ; check_index < MAX_EQUIP_USER_TITLE ; ++check_index )
		{
			if( EquipTitle[check_index] == desigSetType ) 
			{
				check_dupicate = true;	break;
			}
		}
	}

	if(!check_dupicate)
		EquipTitle[desigSlot] = desigSetType;
	
	GlobalFunc::EncryptValue(EquipTitle, sizeof(EquipTitle), ENC_SEED_EQUIP);
	::memcpy(m_aEquipDesig, EquipTitle, sizeof(EquipTitle));
}


void	CUserDesigInfo::GetEquipDesig(UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE]) const
{
	::memcpy(desigSetTypes, m_aEquipDesig, sizeof(m_aEquipDesig));
	GlobalFunc::DecryptValue(desigSetTypes, sizeof(m_aEquipDesig), ENC_SEED_EQUIP);
}

void	CUserDesigInfo::SetEquipDesig(const UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE])
{
	UINT8 _desigTypes[MAX_EQUIP_USER_TITLE];
	::memcpy(_desigTypes, desigSetTypes, sizeof(_desigTypes));
	GlobalFunc::EncryptValue(_desigTypes, sizeof(_desigTypes), ENC_SEED_EQUIP);
	::memcpy(m_aEquipDesig, _desigTypes, sizeof(m_aEquipDesig));	
}

INT32	CUserDesigInfo::GetLimitCount() const
{
	return GlobalFunc::DecryptValue((INT32 *)&m_LimitCount, ENC_SEED_LIMIT);	
}

void	CUserDesigInfo::SetLimitCount(INT32 count)
{
	GlobalFunc::EncryptValue(&count, ENC_SEED_LIMIT);
	m_LimitCount = count;
}

INT32	CUserDesigInfo::FindDesigEquipSlot(UINT8 desigSetType) const
{
	INT32 LimitCount = GetLimitCount();
	UINT8 EquipDesig[MAX_EQUIP_USER_TITLE];
	GetEquipDesig(EquipDesig);

	for ( INT32 i = 0 ; i < LimitCount ; ++i )
	{
		if ( EquipDesig[i] == desigSetType )
			return i;
	}
	return -1;	
}

void	Reset_TempUserDesigInfo(TempUserDesigInfo& out)
{
	out.uploadDesigSetType = 0;
	out.gainedDesigSetType = 0;
	out.uploadSlotIdx = 0;
	out.releaseSlotIdx = 0;
}
