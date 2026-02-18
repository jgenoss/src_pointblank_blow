#include "pch.h"
#include "MedalDef_Client.h"
#include "NormalMedalSet.h"

namespace
{
	i3::string getNormalQuestFilePath(void)
	{
		switch (LOCALE_NUMBER)
		{
		case LOCALE_CODE_KOREA: return NORMAL_MEDAL_PATH_KOREA;
		case LOCALE_CODE_THAILAND: return NORMAL_MEDAL_PATH_THAILAND;
		case LOCALE_CODE_INDONESIA: return NORMAL_MEDAL_PATH_INDONESIA;
		case LOCALE_CODE_RUSSIA: return NORMAL_MEDAL_PATH_RUSSIA;
		case LOCALE_CODE_TURKEY: return NORMAL_MEDAL_PATH_TURKEY;
		case LOCALE_CODE_CHINA: return NORMAL_MEDAL_PATH_CHINA;
		case LOCALE_CODE_TAIWAN: return NORMAL_MEDAL_PATH_TAIWAN;
		case LOCALE_CODE_JAPAN: return NORMAL_MEDAL_PATH_JAPAN;
		case LOCALE_CODE_BRAZIL: return NORMAL_MEDAL_PATH_BRAZIL;
		case LOCALE_CODE_LATIN_AMERICA: return NORMAL_MEDAL_PATH_LATIN_AMERICA;
		case LOCALE_CODE_NORTHAMERICA: return NORMAL_MEDAL_PATH_NORTHAMERICA;
		case LOCALE_CODE_ITALY: return NORMAL_MEDAL_PATH_ITALY;
		case LOCALE_CODE_MIDDLE_EAST: return NORMAL_MEDAL_PATH_MIDDLE_EAST;
		case LOCALE_CODE_PHILIPPINES: return NORMAL_MEDAL_PATH_PHILIPPINES;
		case LOCALE_CODE_MATCH: return NORMAL_MEDAL_PATH_MATCH;
		case LOCALE_CODE_SINGMAL: return NORMAL_MEDAL_PATH_SINGMAL;
		case LOCALE_CODE_VIETNAM: return NORMAL_MEDAL_PATH_VIETNAM;
		case LOCALE_CODE_KOREA_DAUM: return NORMAL_MEDAL_PATH_KOREA_DAUM;
		case LOCALE_CODE_KOREA_PICAON: return NORMAL_MEDAL_PATH_KOREA_PICAON;
		default: return NORMAL_MEDAL_PATH_NONE;
		}
	}
}

bool	NormalMedalSet::LoadMedalMQF()
{
	CMedalSystemFile medalFile;

	medalFile.SetAutoMemoryDelete(true);
	if (!medalFile.LoadMQF(getNormalQuestFilePath().c_str()))
	{
		// 해당 국가 파일 없으면 기본 파일 로드. 
		if (!medalFile.LoadMQF(NORMAL_MEDAL_PATH_NONE))
		{
			I3PRINTLOG(I3LOG_NOTICE, "LoadMQF Fail");
			return false;
		}
	}

	INT32 _totalCount = medalFile.GetMedalInfoCount();
	m_MedalList.reserve( _totalCount );
	m_viewMedalList.reserve( _totalCount );

	for( int i = 0; i < _totalCount; ++i )
	{
		MQF_MEDAL_INFO * mqfInfo = medalFile.GetMedalInfo(i);
		MEDAL_INFO* pMedal = new MEDAL_INFO(mqfInfo);
		MEMDUMP_NEW( pMedal, sizeof(MEDAL_INFO));

		MedalSetInfo* pInfo = new MedalSetInfo(MEDAL_SET_TYPE_NORMAL, 0, 1, false);
		MEMDUMP_NEW( pInfo, sizeof(MedalSetInfo));

		pInfo->medalSet.push_back(pMedal);

		m_MedalList.push_back(pInfo);
		m_viewMedalList.push_back(pMedal);
		m_MedalMap.insert( i3::unordered_map< UINT16, MEDAL_INFO * >::value_type( pMedal->m_ui16Idx, pMedal));
	}

	return true;
}


bool	NormalMedalSet::LoadMedalMQFinPack()
{
	CMedalSystemFile medalFile;

	medalFile.SetAutoMemoryDelete(true);
	if (!medalFile.LoadMQF(getNormalQuestFilePath().c_str()))
	{
		// 해당 국가 파일 없으면 기본 파일 로드. 
		if (!medalFile.LoadMQF(NORMAL_MEDAL_PATH_NONE))
		{
			I3PRINTLOG(I3LOG_NOTICE, "LoadMQF Fail Pack");
			return false;
		}
	}

	INT32 _totalCount = medalFile.GetMedalInfoCount();
	m_MedalList.reserve( _totalCount );
	m_viewMedalList.reserve( _totalCount );

	for( int i = 0; i < _totalCount; ++i )
	{
		MQF_MEDAL_INFO * mqfInfo = medalFile.GetMedalInfo(i);
		MEDAL_INFO* pMedal = new MEDAL_INFO(mqfInfo);
		MEMDUMP_NEW( pMedal, sizeof(MEDAL_INFO));

		MedalSetInfo* pInfo = new MedalSetInfo(MEDAL_SET_TYPE_NORMAL, 0, 1, false);
		MEMDUMP_NEW( pInfo, sizeof(MedalSetInfo));

		pInfo->medalSet.push_back(pMedal);

		m_MedalList.push_back(pInfo);
		m_viewMedalList.push_back(pMedal);
		m_MedalMap.insert( i3::unordered_map< UINT16, MEDAL_INFO * >::value_type( pMedal->m_ui16Idx, pMedal));
	}

	return true;
}

void NormalMedalSet::setMedalReward(UINT16 idx, UINT8 lv)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		pInfo->setMedalReward(lv,true);
	}
}

INT32 NormalMedalSet::getMedalTypeByIndex(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);
	if ( it != m_MedalMap.end() )
	{
		return MEDAL_SET_TYPE_NORMAL;
	}
	return -1;
}