#include "pch.h"
#include "MedalSetBase.h"
#include "MedalDef_Client.h"

static bool	isFlag(UINT8 reward, INT32 t)
{
	bool bFlag = false;
	switch(t)
	{
	case 0:	bFlag = ((reward & 0x01) != 0);	break;
	case 1:	bFlag = ((reward & 0x02) != 0);	break;
	case 2:	bFlag = ((reward & 0x04) != 0);	break;
	case 3:	bFlag = ((reward & 0x08) != 0);	break;
	case 4:	bFlag = ((reward & 0x10) != 0);	break;
	case 5:	bFlag = ((reward & 0x20) != 0);	break;
	}
	return bFlag;
}

MedalSetBase::MedalSetBase()
{

}

MedalSetBase::~MedalSetBase()
{
	m_viewMedalList.clear();
	for each( MedalSetInfo* pSetInfo in m_MedalList )
	{
		if( pSetInfo )
		{
			for each( MEDAL_INFO* ptr in pSetInfo->medalSet )
			{
				I3_SAFE_DELETE( ptr);
			}

			I3_SAFE_DELETE( pSetInfo);
		}
	}

	m_MedalList.clear();
}

INT32 MedalSetBase::getMedalSetCount( void)
{
	size_t _count = m_MedalList.size();
	INT32 result = 0;
	for(UINT32 i=0; i<_count; i++)
	{
		MedalSetInfo * pSetInfo = m_MedalList[i];
		result += pSetInfo->medalSetCount;
	}
	return result;
}

bool	MedalSetBase::LoadMedalMQFDir(const char * path)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	BOOL bResult=true;

	char drive[_MAX_DRIVE];
	char dir[MAX_PATH];

	hSrch = FindFirstFile(path,&wfd);
	if (hSrch == INVALID_HANDLE_VALUE) {
		return false;
	}

	

	while (bResult) {
		_splitpath(path,drive,dir,nullptr,nullptr);
		strtok(wfd.cFileName, ".");
		char * ext = strtok(nullptr,"");

		if ((strcmp(wfd.cFileName,".")  && strcmp(wfd.cFileName,"..")) && ext != nullptr && strcmp(ext, "mqfex") == 0) {
			strncat(dir, wfd.cFileName, i3::generic_string_size(wfd.cFileName) );
			strncat(dir, ".", i3::generic_string_size("."));
			strncat(dir, ext, i3::generic_string_size(ext));
			CMedalSystemFile medalFile;

			medalFile.SetAutoMemoryDelete( true );
			if( !medalFile.LoadMQF( dir ) )
			{
				::FindClose(hSrch);
				I3PRINTLOG(I3LOG_NOTICE, "LoadMQF Fail");
				return false;
			}

			INT32 _totalCount = medalFile.GetMedalInfoCount();
			if(_totalCount > 0)
			{
				MedalSetInfo* pInfo = new MedalSetInfo(medalFile.GetMedalSetType(), (UINT16)medalFile.GetMedalUniqueIdx(), _totalCount, false);
				MEMDUMP_NEW( pInfo, sizeof(MedalSetInfo));

				for(INT32 i=0; i < _totalCount; i++ )
				{
					MQF_MEDAL_INFO * mqfInfo = medalFile.GetMedalInfo(i);
					MEDAL_INFO* pMedal = new MEDAL_INFO(mqfInfo);
					MEMDUMP_NEW( pMedal, sizeof(MEDAL_INFO));

					pInfo->medalSet.push_back(pMedal);
					m_viewMedalList.push_back(pMedal);
					m_MedalMap.insert( i3::unordered_map< UINT16, MEDAL_INFO * >::value_type( pMedal->m_ui16Idx, pMedal));
				}
				m_MedalList.push_back(pInfo);
			}
		}

		bResult=FindNextFile(hSrch,&wfd);
	}
	FindClose(hSrch);
	return true;
}

UINT16	MedalSetBase::getMedalIdx(INT32 idx)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return pMedal->m_ui16Idx;
}

bool	MedalSetBase::isActivated(INT32 idx)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return pMedal->m_bActiveMedal;
}

bool	MedalSetBase::isAcquired(INT32 idx)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return pMedal->m_bAcq;
}

bool	MedalSetBase::isNew(INT32 idx)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return pMedal->m_bNew;
}

bool	MedalSetBase::isNew(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		return pInfo->m_bNew;
	}
	return false;
}

void	MedalSetBase::setNew(INT32 idx, bool bEnable)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	pMedal->m_bNew = bEnable;
}

bool	MedalSetBase::hasNew()
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.begin();

	while( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		if(pInfo->m_bNew)
			return true;
		++it;
	}
	return false;
}

MEDAL_INFO* MedalSetBase::getMedal(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		return pInfo;
	}
	return nullptr;
}

void	MedalSetBase::setMedalInfo(MEDAL& pMedal)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( pMedal.m_ui16Idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		pInfo->setActive(true);
		pInfo->setBattleCount(pMedal.m_ui16Count, false);
		pInfo->setTotalMedalReward(pMedal.m_ui8GetReward);
	}
}

void	MedalSetBase::setMedalInfo(CUR_MEDAL_SET& pMedal)
{
	for(size_t i=0; i < m_MedalList.size(); i++)
	{
		MedalSetInfo * pInfo = m_MedalList[i];
		UINT16 setIndex = pMedal.m_ui16Idx;
		if(pInfo->setIndex == setIndex)
		{
			for(INT32 j=0; j < pInfo->medalSetCount; j++)
			{
				pInfo->medalSet[j]->setActive(true);
				pInfo->medalSet[j]->setBattleCount(pMedal.m_ui16Count[j], false);
				if(pInfo->setType == MEDAL_SET_TYPE_EVENT)
					pInfo->medalSet[j]->setAcquired(true);
				if(isFlag(pMedal.m_ui8GetReward, j))
					pInfo->medalSet[j]->setMedalReward(MEDAL_LEVEL_1, true);
			}
			break;
		}
	}
}

void	MedalSetBase::setMedalInfo(COM_MEDAL_SET& pMedal)
{
	for(size_t i=0; i < m_MedalList.size(); i++)
	{
		MedalSetInfo * pInfo = m_MedalList[i];
		UINT16 setIndex = pMedal.m_ui16Idx;
		if(pInfo->setIndex == setIndex)
		{
			for(INT32 j=0; j < pInfo->medalSetCount; j++)
			{
				pInfo->medalSet[j]->setBattleCount(pMedal.m_ui16Count[j], false);
				if(isFlag(pMedal.m_ui8GetReward, j))
					pInfo->medalSet[j]->setMedalReward(MEDAL_LEVEL_1, true);
			}
			break;
		}
	}
}

void	MedalSetBase::setBattleInfo(const MEDAL_STATE & pMedal)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( pMedal.m_ui16MedalIdx);
	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		//I3PRINTLOG(I3LOG_NOTICE,"Medal Complete Idx = %d\n", pMedal.m_ui16MedalIdx);
		//I3PRINTLOG(I3LOG_NOTICE,"Medal Complete Cnt = %d\n", pMedal.m_ui16Count);
		pInfo->setBattleCount(pMedal.m_ui16Count, true);
	}
}

void	MedalSetBase::resetMedalInfo(void)
{
	for(size_t i = 0; i < m_MedalList.size(); ++i)
	{
		MedalSetInfo * pInfo = m_MedalList[i];
		for(INT32 j = 0 ; j < pInfo->medalSetCount ; ++j)
		{
			pInfo->medalSet[j]->setActive(false);
		}
	}
}

INT32	MedalSetBase::getMedalImageIndex(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		return pInfo->m_ui16ImageIdx;
	}
	return -1;
}

INT32	MedalSetBase::getMedalImageIndex(INT32 idx)
{
	MEDAL_INFO * pInfo = m_viewMedalList[idx];
	return pInfo->m_ui16ImageIdx;
}


UINT16	MedalSetBase::getMedalProcessCnt(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		return pInfo->m_ui16BattleCount;
	}
	return 0;
}

INT32	MedalSetBase::getMedalProcessCnt(INT32 idx)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return pMedal->m_ui16BattleCount;
}

UINT8   MedalSetBase::getMaxLevel(INT32 idx)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	UINT16 max_count = 0;
	UINT8  level = 0;
	for(UINT8 i=0; i<MEDAL_LEVEL_MAX + 1; i++)
	{
		if( max_count <= pMedal->m_ui16Count[i] )
		{
			max_count = pMedal->m_ui16Count[i];
			level = i;
		}
	}
	return level;
}

UINT8	MedalSetBase::getCurrentLevel(INT32 idx)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	UINT8 level = 0;
	
	for(UINT8 i=0; i<MEDAL_LEVEL_MAX + 1; i++)
	{
		if( pMedal->m_ui16BattleCount >= pMedal->m_ui16Count[i] )
			level = i;
		else
			break;
	}
	return level;
}

UINT8	MedalSetBase::getCurrentLevel(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		UINT8 level = 0;

		for(UINT8 i=0; i<MEDAL_LEVEL_MAX + 1; i++)
		{
			if( pInfo->m_ui16BattleCount >= pInfo->m_ui16Count[i] )
				level = i;
			else
				break;
		}
		return level;
	}
	return 0xff;
}

INT32	MedalSetBase::getLevelProcessCnt(MEDAL_INFO * pMedal, INT32 idx)
{
	return pMedal->m_ui16Count[idx];
}

INT32	MedalSetBase::getLevelProcessCnt(INT32 idx, UINT8 lv)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return pMedal->m_ui16Count[lv];
}

float	MedalSetBase::getLevelRate(INT32 idx, UINT8 lv)
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	INT32 battleCount	= pMedal->m_ui16BattleCount;
	INT32 currentCount	= 0;
	INT32 maxCount		= 0;
	
	currentCount = battleCount - getLevelProcessCnt(pMedal, lv);
	if(lv == MEDAL_LEVEL_MAX)
		maxCount = currentCount;
	else
		maxCount = getLevelProcessCnt(pMedal, lv+1) - getLevelProcessCnt(pMedal, lv);

	if(currentCount >= maxCount)
		currentCount = maxCount;
	if(currentCount < 0)
		currentCount = 0;

	if(currentCount == maxCount || maxCount == 0.0f)
		return 1.0f;
	else
		return (float)currentCount / (float)maxCount;
}

i3::rc_wstring	MedalSetBase::getMedalName(INT32 idx) const
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return GAME_QUEST_RCSTRING(pMedal->m_szNameKey);
}

const char *	MedalSetBase::getMedalDesc(INT32 idx)	// UI에서만 쓰이는 함수입니다.
{
	MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return pMedal->m_szDescription;
}

const char*		MedalSetBase::getMedalDesc(UINT16 idx)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		MEDAL_INFO * pInfo = it->second;
		return pInfo->m_szDescription;
	}
	return nullptr;
}

const MQF_REWARD_INFO*	MedalSetBase::getMedalReward(INT32 idx, UINT8 lv)
{
	const MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return &pMedal->m_eRewardInfo[lv];
}

const MQF_REWARD_INFO*	MedalSetBase::getMedalReward(UINT16 idx, UINT8 lv)
{
	i3::unordered_map< UINT16, MEDAL_INFO *>::iterator it = m_MedalMap.find( idx);

	if ( it != m_MedalMap.end() )
	{
		const MEDAL_INFO * pInfo = it->second;
		return &pInfo->m_eRewardInfo[lv];
	}
	return nullptr;
}

bool MedalSetBase::isGetReward(INT32 idx, UINT8 lv)
{
	const MEDAL_INFO * pMedal = m_viewMedalList[idx];
	return pMedal->m_bRewardAcq[lv];
}

namespace MedalSetProc
{
	bool file_find_in_pack( INT32 Level, char * pszPath, void * pFileInfo, void * pUserData)
	{
		MedalSetBase* medal_base = static_cast<MedalSetBase*>(pUserData);

		WIN32_FIND_DATA * pFile = (WIN32_FIND_DATA *) pFileInfo;

		i3::string file_path(pszPath);
		file_path.append( "\\" );
		file_path.append( pFile->cFileName );

		CMedalSystemFile medalFile;
		medalFile.SetAutoMemoryDelete( true );

		if( !medalFile.LoadMQFinPack( file_path.c_str() ) )
		{
			I3PRINTLOG(I3LOG_NOTICE, "LoadMQF Fail");
			return false;
		}

		INT32 _totalCount = medalFile.GetMedalInfoCount();
		if(_totalCount > 0)
		{
			MedalSetInfo* pInfo = new MedalSetInfo(medalFile.GetMedalSetType(), (UINT16)medalFile.GetMedalUniqueIdx(), _totalCount, false);
			MEMDUMP_NEW( pInfo, sizeof(MedalSetInfo));

			for(INT32 i=0; i < _totalCount; i++ )
			{
				MQF_MEDAL_INFO * mqfInfo = medalFile.GetMedalInfo(i);
				MEDAL_INFO* pMedal = new MEDAL_INFO(mqfInfo);
				MEMDUMP_NEW( pMedal, sizeof(MEDAL_INFO));

				pInfo->medalSet.push_back(pMedal);
				medal_base->m_viewMedalList.push_back(pMedal);
				medal_base->m_MedalMap.insert( i3::unordered_map< UINT16, MEDAL_INFO * >::value_type( pMedal->m_ui16Idx, pMedal));
			}
			medal_base->m_MedalList.push_back(pInfo);
		}

		return true;
	}
}

bool	MedalSetBase::LoadMedalMQFinPack(const char * path)
{
	i3System::LookForFiles( path, "*.mqfex", MedalSetProc::file_find_in_pack, this, 0); //0 대신 FFT_SUBDIR 이거면 하위까지 할 듯
	return true;
}