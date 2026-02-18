#include "pch.h"
#include "MedalManager.h"
#include "DailyMedalSet.h"
#include "MedalDef_Client.h"

bool DailyMedalSet::LoadMedalMQF()
{
	const char* szDailyMedalPath = "Quest/Daily/*.*";
	return LoadMedalMQFDir(szDailyMedalPath);
}

void DailyMedalSet::setMedalReward(UINT16 idx, UINT8 lv)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		pInfo->setMedalReward(1,true);
	}
}

INT32 DailyMedalSet::getMedalTypeByIndex(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);
	if ( it != m_MedalMap.end() )
	{
		return MEDAL_SET_TYPE_DAILY;
	}
	return -1;
}