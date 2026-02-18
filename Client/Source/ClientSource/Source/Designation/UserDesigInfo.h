#pragma once

//
// 매니저같은데 두어야 하는지 좀더 재고해볼 것...
//

class CUserDesigInfo
{
public:	
	CUserDesigInfo() {}
	
	bool	IsGainedDesig(INT32 DesigSetType) const;
	void	GetGainedDesig(UINT8 (&OutDesigSetTypes)[MAX_USER_TITLE]) const;

	void	SetGainedDesigWithFlag(UINT64 DesigTypeGainFlag);
	void	SetGainedDesig(INT32 DesigSetType, bool bGained);
		
	UINT8	GetEquipDesig(INT32 desigSlot) const;
	void	SetEquipDesig(INT32 desigSlot, UINT8 desigSetType);

	void	GetEquipDesig(UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE]) const;
	void	SetEquipDesig(const UINT8 (&desigSetTypes)[MAX_EQUIP_USER_TITLE]);
	
	INT32	GetLimitCount() const;
	void	SetLimitCount(INT32 count);

	INT32	FindDesigEquipSlot(UINT8 desigSetType) const;
	
private:
	UINT8	m_aGainedDesig[MAX_USER_TITLE] = { 0 };			// 호칭 Id당 획득여부 0:없음, 1:획득
	UINT8	m_aEquipDesig[MAX_EQUIP_USER_TITLE] = { 0 };	// 장비된 호칭 Id
	INT32	m_LimitCount = 0;						// 사용가능 호칭 장비 슬롯 갯수
};

/////

struct TempUserDesigInfo
{
	UINT8	uploadDesigSetType = 0;
	UINT8	gainedDesigSetType = 0;
	UINT8	uploadSlotIdx = 0;
	UINT8	releaseSlotIdx = 0;
};

void	Reset_TempUserDesigInfo(TempUserDesigInfo& out);
