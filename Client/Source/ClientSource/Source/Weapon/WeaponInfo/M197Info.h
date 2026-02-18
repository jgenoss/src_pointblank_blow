#if !defined( __M197_INFO_H)
#define __M197_INFO_H

#include "GunInfo.h"

class CM197Info : public CGunInfo
{
	I3_CLASS_DEFINE(CM197Info, CGunInfo);
private:
	REAL32 m_rContinueFireTime;
	REAL32 m_rChargeSpeedRatio;
	REAL32 m_rFirstDelayTime;
	REAL32 m_rSlope_DelayPerFire;
	REAL32 m_rFirePenaltyTime;
	REAL32 m_rLimitRho;
	REAL32 m_rLimitTheta;

public:

	CM197Info();
	virtual ~CM197Info();

	virtual void	ReadWeaponInfo( i3RegKey * pKey) override;
	virtual void	OnEncriptInfo( UINT8 u8Key) override;

	REAL32			GetContinueFireTime() const;
	REAL32			GetChargeSpeedRatio() const;
	REAL32			GetFirstDelayTime() const;
	REAL32			GetSlope_DelayPerFire() const;
	REAL32			GetFirePenaltyTime() const;
	REAL32			GetLimitRho() const;
	REAL32			GetLimitTheta() const;
};

#endif // __M197_INFO_H