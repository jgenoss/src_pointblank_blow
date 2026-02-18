#pragma once

// È£Äª
class	CTitleInfo	: public i3ElementBase
{
	I3_CLASS_DEFINE( CTitleInfo, i3ElementBase );
public:
	CTitleInfo();

	UINT8	GetGainedTitle(INT32 idx) const;
	void	GetGainedTitle(UINT8 OutTitle[MAX_USER_TITLE]) const;
	void	SetGainedTitle(INT32 idx, UINT8 val);
	void	SetGainedTitle(UINT64 GainedTitle);

	UINT8	GetEquipTitle(INT32 idx) const;
	void	GetEquipTitle(UINT8 OutTitle[MAX_EQUIP_USER_TITLE]) const;
	void	SetEquipTitle(INT32 idx, UINT8 title);
	void	SetEquipTitle(const UINT8 title[MAX_EQUIP_USER_TITLE]);

	INT32	GetLimitCount() const;
	void	SetLimitCount(INT32 count);

	INT32	HaveTitle(UINT8 title) const;

private:
	UINT8	m_GainedUserTitle[MAX_USER_TITLE];			// È£Äª Id´ç È¹µæ¿©ºÎ 0:¾øÀ½, 1:È¹µæ
	UINT8	m_EquipUserTitle[MAX_EQUIP_USER_TITLE];		// ÀåºñµÈ È£Äª Id
	INT32	m_LimitUserTitleCount;						// »ç¿ë°¡´É È£Äª Àåºñ ½½·Ô °¹¼ö
};


class	CTitleNetTempInfo	: public i3ElementBase
{
	I3_CLASS_DEFINE( CTitleNetTempInfo, i3ElementBase );
public:
	CTitleNetTempInfo();

public:
	UINT8	GetGainedIdx() const			{ return m_GainedIdx; }
	UINT8	GetReleaseSlotIdx() const		{ return m_ReleaseSlotIdx; }
	UINT8 	GetUploadSlotIdx() const		{ return m_UploadSlotIdx; }
	UINT8 	GetUploadDesignationIdx() const	{ return m_UploadDesignationIdx; }

	void	SetGainedIdx(UINT8 val)				{ m_GainedIdx = val; }
	void	SetReleaseSlotIdx(UINT8 val)		{ m_ReleaseSlotIdx = val; }
	void 	SetUploadSlotIdx(UINT8 val)			{ m_UploadSlotIdx = val; }
	void 	SetUploadDesignationIdx(UINT8 val)	{ m_UploadDesignationIdx = val; }

private:
	UINT8	m_GainedIdx;
	UINT8	m_UploadSlotIdx;
	UINT8	m_UploadDesignationIdx;
	UINT8	m_ReleaseSlotIdx;

};
