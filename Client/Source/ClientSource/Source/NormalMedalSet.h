#if !defined( __NORMAL_MEDAL_SET_H__)
#define __NORMAL_MEDAL_SET_H__

#include "MedalSetBase.h"

class NormalMedalSet : public MedalSetBase
{
public:
	NormalMedalSet()			{}
	virtual ~NormalMedalSet()	{}

public:
	virtual void	BindMedal(MQF_MEDAL_INFO* pData) override {}
	virtual void	BindMedal(MQF_DAILY_MEDAL_SET_INFO* pData) override {}
	virtual bool	LoadMedalMQF() override;
	virtual INT32	getMedalSetCount() override { return m_MedalList.size();	};
	virtual INT32	getMedalTypeByIndex(UINT16) override;
	virtual void	setMedalReward(UINT16 idx, UINT8 lv) override;
	bool	LoadMedalMQFinPack();

};

#endif