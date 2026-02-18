#include "pch.h"
#include "MedalManager.h"
#include "ClanMedalSet.h"

bool ClanMedalSet::LoadMedalMQF()
{

	return true;
}

void ClanMedalSet::setMedalReward(UINT16 idx, UINT8 lv)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		pInfo->setMedalReward(lv,true);
	}
}

INT32 ClanMedalSet::getMedalTypeByIndex(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);
	if ( it != m_MedalMap.end() )
	{
		return MEDAL_SET_TYPE_CLAN;
	}
	return -1;
}