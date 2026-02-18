#if !defined( __EVENT_MEDAL_SET_H__)
#define __EVENT_MEDAL_SET_H__

#include "MedalSetBase.h"

class EventMedalSet : public MedalSetBase
{
public:
	EventMedalSet()	{}
	virtual ~EventMedalSet() {}

public:
	virtual void	BindMedal(MQF_MEDAL_INFO* pData) override {}
	virtual void	BindMedal(MQF_DAILY_MEDAL_SET_INFO* pData) override {}
	virtual bool	LoadMedalMQF() override;
	virtual INT32	getMedalTypeByIndex(UINT16) override;
	virtual void	setMedalReward(UINT16 idx, UINT8 lv) override;
};

#endif