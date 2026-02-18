#include "pch.h"
#include "MedalManager.h"
#include "GlobalVariables.h"

#include "MedalSetBase.h"
#include "ClanMedalSet.h"
#include "DailyMedalSet.h"
#include "EventMedalSet.h"
#include "NormalMedalSet.h"

CMedalManager::CMedalManager(void) : m_fAchievementRate(0.f), m_iCompletedMedalCount(0)
	, m_rRewardResultWaitTime(0.0f), m_bIsWaitResult(false), m_nSortType(0)
{
	MedalSetList[MEDAL_SET_TYPE_NORMAL] = new NormalMedalSet;
	MedalSetList[MEDAL_SET_TYPE_EVENT] = new EventMedalSet;
	MedalSetList[MEDAL_SET_TYPE_DAILY] = new DailyMedalSet;
	MedalSetList[MEDAL_SET_TYPE_CLAN] = new ClanMedalSet;
	
	MEMDUMP_NEW( MedalSetList[ MEDAL_SET_TYPE_NORMAL], sizeof(NormalMedalSet));
	MEMDUMP_NEW( MedalSetList[ MEDAL_SET_TYPE_EVENT], sizeof(EventMedalSet));
	MEMDUMP_NEW( MedalSetList[ MEDAL_SET_TYPE_DAILY], sizeof(DailyMedalSet));
	MEMDUMP_NEW( MedalSetList[ MEDAL_SET_TYPE_CLAN], sizeof(ClanMedalSet));

	for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; ++i)
	{
		notifyMedalList[i].m_ui16Idx = 0;
		notifyMedalList[i].m_ui8MedalType = 0;
		notifyMedalList[i].m_ui16Count = 0;
	}

	// Medal Reward Resqust Info
	m_vRewardResultList.clear();

	// Quick Medal Reword
	m_vQuickRewardList.clear();

	ClearCurrentRewardInfo();
}

CMedalManager::~CMedalManager(void)
{
	for(INT32 i=0; i<MEDAL_SET_TYPE_MAX; ++i)
	{
		I3_SAFE_DELETE(MedalSetList[i]);
	}
}

INT32 CMedalManager::getMedalTotalCount(INT32 _Type)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return -1;
	return MedalSetList[_Type]->getMedalTotalCount();
}

INT32 CMedalManager::getMedalSetCount(INT32 _Type)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return -1;
	return MedalSetList[_Type]->getMedalSetCount();
}

UINT16 CMedalManager::getMedalIdx(INT32 _Type, INT32 idx)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return 0;
	if( idx < 0) return 0;
	return MedalSetList[_Type]->getMedalIdx(idx);
} 

INT32 CMedalManager::getImageIdx(INT32 _Type, INT32 idx)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return -1;
	if( idx < 0) return -1;
	return MedalSetList[_Type]->getMedalImageIndex(idx);
}

INT32 CMedalManager::getImageIdx(UINT8 _Type,UINT16 idx)
{
	if( _Type >= MEDAL_SET_TYPE_MAX) return -1;
	return MedalSetList[_Type]->getMedalImageIndex(idx);
}

INT32 CMedalManager::getLevelProcessCnt(INT32 _Type, INT32 idx, UINT8 lv)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return -1;
	return MedalSetList[_Type]->getLevelProcessCnt(idx,lv);
}

INT32 CMedalManager::getMedalProcessCnt(INT32 _Type, INT32 idx)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return -1;
	if( idx < 0) return -1;
	return MedalSetList[_Type]->getMedalProcessCnt(idx);
}

UINT8 CMedalManager::getCurrentLevel(INT32 _Type, INT32 idx)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return 0;
	if( idx < 0) return 0;
	return MedalSetList[_Type]->getCurrentLevel(idx);
}

UINT8 CMedalManager::getMaxLevel(INT32 _Type, INT32 idx)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return 0;
	if( idx < 0) return 0;
	return MedalSetList[_Type]->getMaxLevel(idx);
}

float CMedalManager::getLevelRate(INT32 _Type, INT32 idx, UINT8 lv)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return 1.f;
	if( idx < 0) return 1.f;
	return MedalSetList[_Type]->getLevelRate(idx, lv);
}

bool CMedalManager::isAcquired(INT32 _Type,INT32 idx)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return false;
	if( idx < 0) return false;
	return MedalSetList[_Type]->isAcquired(idx);
}

bool CMedalManager::isActivated(INT32 _Type, INT32 idx)
{
	if( _Type < 0 || _Type >= MEDAL_SET_TYPE_MAX) return false;
	if( idx < 0) return false;
	return MedalSetList[_Type]->isActivated(idx);
}

bool CMedalManager::isChecked(INT32 _Type,INT32 idx)
{
	if( idx < 0 ) return false;
	for(INT32 i=0; i < MAX_NOTIFY_MADAL_COUNT; i++){
		if(notifyMedalList[i].m_ui8MedalType == (UINT8)_Type &&
			notifyMedalList[i].m_ui16Idx == MedalSetList[_Type]->getMedalIdx(idx))
		{
			return true;
		}
	}
	return false;
}

bool CMedalManager::isNew(INT32 _Type,INT32 idx)
{
	return MedalSetList[_Type]->isNew(idx);
}

void CMedalManager::setNew(INT32 _Type,INT32 idx, bool bEnable)
{
	MedalSetList[_Type]->setNew(idx, bEnable);
}

bool CMedalManager::hasNew(INT32 _Type)
{
	return MedalSetList[_Type]->hasNew();
}

char* CMedalManager::getMedalFileName(INT32 fileindex)
{
	return nullptr;
}

i3::rc_wstring CMedalManager::getMedalName(INT32 _Type, INT32 idx) const
{
	return MedalSetList[_Type]->getMedalName(idx);
}

const char* CMedalManager::getMedalDesc(INT32 _Type, INT32 idx)
{
	return MedalSetList[_Type]->getMedalDesc(idx);
}

const char* CMedalManager::getNotifyMedalDesc(UINT8 _Type, UINT16 idx)
{
	return MedalSetList[_Type]->getMedalDesc(idx);
}

const char* CMedalManager::getNotifyMedalCountDesc(UINT8 _Type, UINT16 idx)
{
	return nullptr;
}

const MQF_REWARD_INFO *	CMedalManager::getReward(INT32 _Type, INT32 idx, UINT8 lv)
{
	return MedalSetList[_Type]->getMedalReward(idx, lv);
}

bool CMedalManager::isGetReward(INT32 _Type, INT32 idx, UINT8 lv)
{
	return MedalSetList[_Type]->isGetReward(idx, lv);
}

bool CMedalManager::isReceivableReward( INT32 _Type, INT32 idx )
{
	for (UINT8 level = 0; level < MEDAL_LEVEL_MAX; ++level )
	{
		INT32 curr_count = getMedalProcessCnt(_Type, idx) - getLevelProcessCnt(_Type, idx, level);
		INT32 goal_count = getLevelProcessCnt(_Type, idx, level + 1) - getLevelProcessCnt(_Type, idx, level);

		if(curr_count >= goal_count) curr_count = goal_count;
		if(curr_count < 0) curr_count = 0;

		if ( !isGetReward(_Type, idx, level) && curr_count == goal_count)
			return true;
	}

	return false;
}

void CMedalManager::getMedalMessageFront(MEDAL_STATE *pData)
{
	MEDAL_STATE *cData = &(m_qMedalMessage.front());
	pData->m_ui8MedalType = cData->m_ui8MedalType;
	pData->m_ui16MedalIdx = cData->m_ui16MedalIdx;
	pData->m_ui16Count = cData->m_ui16Count;
	m_qMedalMessage.pop();
}


void CMedalManager::PushCompleteMedalInfo(const MEDAL_STATE & data)
{
	MedalCompleteInfo info;
	if( !_CheckCompleteMedal(data,info) )
		return;

	m_qCompleteMedal.push(info);
}

void CMedalManager::getCompleteMedalFront(MedalCompleteInfo *pData)
{
	MedalCompleteInfo *cData = &(m_qCompleteMedal.front());
	pData->SetMedalCompleteInfo(cData->_type,cData->_idx,cData->_level);
	m_qCompleteMedal.pop();
}

bool CMedalManager::IsPerfectClearMedal(MedalCompleteInfo *pData) const
{
	// 여기에 MEDAL_LEVEL_MAX 로 들어오는 경우는 마지막 레벨을 클리어 했을 경우 뿐임
	if ( pData->_level >= MEDAL_LEVEL_MAX )
		return true;
	else
	{
		// 4레벨이 마지막이지 않은 메달의 경우 다음 레벨의 필요 카운트가 0으로 세팅되므로 이렇게 판별..

		MedalSetBase * pMedalSet = MedalSetList[pData->_type];
		MEDAL_INFO * pMedal = pMedalSet->getMedal(pData->_idx);
		INT32 i32NextLevCnt	= pMedal->m_ui16Count[pData->_level + 1];

		if ( i32NextLevCnt == 0 )
		{
			return true;
		}
	}
	return false;
}

MedalViewInfo* CMedalManager::GetMedalViewIdx(INT32 idx) const
{
	MedalViewInfo* pMedalInfo = nullptr;

	for( UINT32 i = 0; i < m_viewList.size(); i++)
	{
		if( m_viewList[i]->medal_Idx == idx)
		{
			pMedalInfo = m_viewList[i];
			break;
		}
	}

	return pMedalInfo;
}

void	CMedalManager::MakeViewList(MEDALTAB eTab)					
{
	MEDAL_SET_TYPE tMedalType = MEDAL_SET_TYPE_NORMAL;

	if(eTab == TAB_EVENT )
		tMedalType = MEDAL_SET_TYPE_EVENT;
	else if(eTab == TAB_BASIC )
		tMedalType = MEDAL_SET_TYPE_NORMAL;
	else if(eTab == TAB_DAILY )
		tMedalType = MEDAL_SET_TYPE_DAILY;

	m_viewList.clear();
	
	INT32 _total = getMedalSetCount(tMedalType);

	for(INT32 i=0; i < _total; i++)
	{
		if(CMedalManager::i()->isActivated(tMedalType, i) == false)
			continue;

		m_viewInfo[i].medal_viewIdx = i;
		m_viewInfo[i].medal_Idx		= CMedalManager::i()->getMedalIdx(tMedalType, i);
		m_viewInfo[i].medal_tabIdx	= eTab;
		m_viewList.push_back(&m_viewInfo[i]);
	}
	std::sort(m_viewList.begin(), m_viewList.end(), _sort_medal);
}

// 보상을 받을 수 있는 완료된 퀘스트 리스트를 생성
void	CMedalManager::MakeRewardList()
{
	INT32 _RewardInfoCount = 0;
	m_fAchievementRate = 0.f;
	m_iCompletedMedalCount = 0;

	// GD팀 요청으로 모든 타입(일반, 일일, 이벤트) 메달 퀘스트의 보상 메달을 체크
	for (INT32 tMedalType = 0; tMedalType < 3; tMedalType++)
	{
		INT32 _total = getMedalSetCount(tMedalType);

		// 해당 타입에 등록된 모든 메달을 확인한다
		for(INT32 i = 0; i < _total; i++)
		{
			m_rewardInfo[_RewardInfoCount].medal_viewIdx	= i;
			m_rewardInfo[_RewardInfoCount].medal_Idx		= CMedalManager::i()->getMedalIdx(tMedalType, i);
			m_rewardInfo[_RewardInfoCount].medal_tabIdx		= tMedalType;

			if(CMedalManager::i()->isActivated(tMedalType, i) == false)
				continue;

			// 일반 메달 퀘스트는 4레벨까지 있음
			if (tMedalType == MEDAL_SET_TYPE_NORMAL)
			{
				for (UINT8 iMedalLevel = 0; iMedalLevel < MEDAL_LEVEL_MAX; iMedalLevel++)
				{
					INT32 begin_cnt = m_rewardInfo[_RewardInfoCount].getMedalProcessCnt() - m_rewardInfo[_RewardInfoCount].getLevelProcessCnt(iMedalLevel);
					INT32 dest_cnt = 0;

					// 레벨 4 테스트 필요 
					// 이 코드들은 나에게 똥을 안겨주었어....일단 돌지 않는 코드만 삭제 처리....(16.07.29.수빈)....옛날 동익씨 코드같은데 많이 실망 스럽....

					dest_cnt = m_rewardInfo[_RewardInfoCount].getLevelProcessCnt(iMedalLevel + 1) - m_rewardInfo[_RewardInfoCount].getLevelProcessCnt(iMedalLevel);

					if (begin_cnt >= dest_cnt)
						begin_cnt = dest_cnt;
					else if  (begin_cnt < 0)
						begin_cnt = 0;

					if (begin_cnt == dest_cnt)
					{
						// 완료된 메달은 카운트 후 리스트에 넣는다
						m_iCompletedMedalCount++;

						if (!m_rewardInfo[_RewardInfoCount].isGetReward(iMedalLevel))
						{
							m_rewardInfo[_RewardInfoCount].medal_lv = iMedalLevel + 1;
							m_rewardList.push_back(&m_rewardInfo[_RewardInfoCount]);
							_RewardInfoCount++;

							// 여기 코드가 필요한가?
							m_rewardInfo[_RewardInfoCount].medal_viewIdx	= i;
							m_rewardInfo[_RewardInfoCount].medal_Idx		= CMedalManager::i()->getMedalIdx(tMedalType, i);
							m_rewardInfo[_RewardInfoCount].medal_tabIdx		= tMedalType;
						}
					}
				}
			}
			else if(tMedalType == MEDAL_SET_TYPE_DAILY || tMedalType == MEDAL_SET_TYPE_EVENT)
			{
				// 이벤트, 일일 메달 퀘스트는 1레벨만 존재함
				if (!m_rewardInfo[_RewardInfoCount].isGetReward(0) && m_rewardInfo[_RewardInfoCount].getLevelProcessCnt(1) == m_rewardInfo[_RewardInfoCount].getMedalProcessCnt())
				{
					m_rewardInfo[_RewardInfoCount].medal_lv = 1;
					m_rewardList.push_back(&m_rewardInfo[_RewardInfoCount]);
					_RewardInfoCount++;
				}
			}
			else
				I3TRACE("Making Reward Medal List fail. Unknown Medal Type \n");
		}
	}

	// 하지만 메달 완료률은 일반 메달만 확인한다(일반 메달의 레벨 1개당 1개로 계산)  
	m_fAchievementRate = (REAL32)m_iCompletedMedalCount/((REAL32)getMedalSetCount(MEDAL_SET_TYPE_NORMAL) * 4.f) * 100.f;
}

bool	CMedalManager::_sort_medal(const MedalViewInfo* s1, const MedalViewInfo* s2)
{
	INT32 nSortType = CMedalManager::i()->GetSortType();
	if( nSortType == MEDAL_SORT_DEFAULT)						return (s1->medal_viewIdx < s2->medal_viewIdx);
	else if(nSortType == MEDAL_SORT_ACQUIRE)
	{
		if((!s1->isAcquired()) && (s2->isAcquired()))			return false;
		else if((s1->isAcquired()) && (!s2->isAcquired()))	return true;
		else
		{
			return (s1->medal_viewIdx < s2->medal_viewIdx);
		}
	}
	else if(nSortType == MEDAL_SORT_NON_ACQUIRE)
	{
		if((!s1->isAcquired()) && (s2->isAcquired()))		return true;
		else if((s1->isAcquired()) && (!s2->isAcquired()))	return false;
		else
		{
			return (s1->medal_viewIdx < s2->medal_viewIdx);
		}
	}
	else if(nSortType == MEDAL_SORT_NO_GET_REWARD)
	{

		if( !s1->isReceivableReward() && s2->isReceivableReward() )			return false;
		else if( s1->isReceivableReward() && !s2->isReceivableReward() )	return true;
		else 
		{
			return (s1->medal_viewIdx < s2->medal_viewIdx);
		}
	}
	else
		return false;
}

void CMedalManager::SetItemRestriction(T_ItemID ItemID)
{
	SHOP_ITEM_DATA* pItemBase = CShop::i()->GetItemData(ItemID);
	if ( pItemBase == nullptr )
	{
		return;
	}

	MedalViewInfo* pMedalInfo = CMedalManager::i()->GetMedalViewIdx(pItemBase->_ui16Condition);
	if( pMedalInfo == nullptr)
	{
		return;
	}

	if( pItemBase->_ui8RestrictionType == TYPE_ITEM_RESTRICTION_MEDAL )
	{
		pMedalInfo->setRestrictionItem(ItemID);
	}
}

void CMedalManager::ProcessCompleteMedalInfo(const MEDAL_STATE & data)
{
	// 빠른 메달 보상용 vector에 push
	PushQuickRewardMedal(data);

	// 퀘스트 이펙트 출력용 큐에 push
	PushCompleteMedalInfo(data);
	
	// 배틀 정보 갱신 이후에 클라에서 들고있는 메달 정보가 바뀌기 때문에 해당 함수 위와 아래의 순서 중요
	// 배틀 정보 갱신
	ReplaceBattleInfo(data);

	// 메세지 출력용 큐에 push
	// 기획팀 요청으로 메달 진행상황 플로팅 보드 사용하지 않습니다
	// PushMedalMessageInfo(data);
}

void CMedalManager::ReplaceBattleInfo(const MEDAL_STATE & data)
{
	INT32 _type = MEDAL_SET_TYPE_NORMAL;

	if( data.m_ui8MedalType != MEDAL_TYPE_NORMAL )
		_type = getMedalSetTypeByIndex(data.m_ui16MedalIdx);
	
	
	if (_type < 0 || _type >= MEDAL_SET_TYPE_MAX)
	{
		I3_BOUNDCHK(_type, MEDAL_SET_TYPE_MAX);
		return;
	}
	
	MedalSetList[_type]->setBattleInfo(data);
}

bool CMedalManager::IsCompleteMedalInfo(const MEDAL_STATE & data) const
{
	return MedalSetList[data.m_ui8MedalType]->isNew(data.m_ui16MedalIdx);
}

bool CMedalManager::LoadMedalData()
{
	for(size_t i=0; i<MEDAL_SET_TYPE_MAX; ++i)
	{
		if( MedalSetList[i]->LoadMedalMQF() == false )
		{
			I3PRINTLOG(I3LOG_NOTICE,"Load MQF Data fail...");
			return false;
		}
	}
	return true;
}

void CMedalManager::setNotifyMedal( UINT32 idx, NOTIFY_MEDAL *pMedal )
{
	notifyMedalList[idx].m_ui8MedalType	= pMedal->m_ui8MedalType;
	notifyMedalList[idx].m_ui16Idx		= pMedal->m_ui16Idx;
	notifyMedalList[idx].m_ui16Count	= pMedal->m_ui16Count;
}

INT32 CMedalManager::getMedalNormalTypeByIndex(UINT16 idx)
{
	return 0;
}

INT32 CMedalManager::getMedalSetTypeByIndex(UINT16 idx)
{
	
	INT32 _type = -1;
	for(INT32 i=MEDAL_SET_TYPE_EVENT; i<MEDAL_SET_TYPE_MAX; i++)
	{
		MedalSetBase * pBase = MedalSetList[i];	
		_type = pBase->getMedalTypeByIndex(idx);
		if( _type != -1)
			return _type;
	}
	return _type;
}

void CMedalManager::GetSortTypeStr( i3::rc_wstring& strOut, MEDAL_SORT_TYPE _type)
{
	switch(_type)
	{
	case MEDAL_SORT_DEFAULT :		strOut = GAME_RCSTRING("STR_UI_MEDAL_SORT_NORMAL");	break;
	case MEDAL_SORT_ACQUIRE :		strOut = GAME_RCSTRING("STR_UI_MEDAL_SORT_GAIN");		break;
	case MEDAL_SORT_NON_ACQUIRE :	strOut = GAME_RCSTRING("STR_UI_MEDAL_SORT_NOTGAIN");	break;
	case MEDAL_SORT_NO_GET_REWARD :	strOut = GAME_RCSTRING("STR_UI_MEDAL_SORT_REWARD");	break;
	default:						strOut.clear();
	}
}

/***************************************************************************************
****						Medal Reward Resqust Info								****
***************************************************************************************/

void CMedalManager::BuildMedalRewardPacket(MEDAL_REWARD * pOut, const MedalCompleteInfo * pInfo)
{
	if(pInfo->_type == MEDAL_SET_TYPE_NORMAL)
		pOut->m_ui8MedalType = MEDAL_TYPE_NORMAL;
	else
		pOut->m_ui8MedalType= MEDAL_TYPE_SET;

	pOut->m_ui16MedalIdx = pInfo->_idx;
	pOut->m_ui8MedalLevel = static_cast<UINT8>(pInfo->_level);

	m_vRewardResultList.push_back( MedalRewardResult(*pInfo) );
}

void CMedalManager::setMedalRewardResult(PACKET_D_GET_MEDAL_REWARD * pData)
{
	MEDAL_SET_TYPE eType =  MEDAL_SET_TYPE_NORMAL;

	if( pData->m_aMedalReward.m_ui8MedalType != MEDAL_TYPE_NORMAL )
		eType = static_cast<MEDAL_SET_TYPE>( getMedalSetTypeByIndex(pData->m_aMedalReward.m_ui16MedalIdx) );

	MEDAL_LEVEL level = static_cast<MEDAL_LEVEL>(pData->m_aMedalReward.m_ui8MedalLevel);

	MedalCompleteInfo info;
	info.SetMedalCompleteInfo(eType, pData->m_aMedalReward.m_ui16MedalIdx, level);

	i3::vector<MedalRewardResult>::iterator iter = std::find(m_vRewardResultList.begin(), m_vRewardResultList.end(), info);

	I3ASSERT(iter != m_vRewardResultList.end());

	if (iter != m_vRewardResultList.end())
	{
		iter->_success = static_cast<bool>(EV_SUCCESSED(pData->m_i32Rv));
		iter->_itemid = pData->m_aItem.m_TItemID;

		// 보상 받기가 성공한 메달만 받은 처리를 합니다.
		if (iter->_success)
		{
			MedalSetBase * pBase = getMedalSet(info._type);
			if (pBase != nullptr)
				pBase->setMedalReward(info._idx, static_cast<UINT8>(info._level));
		}
	}
}

const MQF_REWARD_INFO * CMedalManager::getMedalRewardInfo(INT32 idx)
{
	I3_BOUNDCHK(idx, (INT32)m_vRewardResultList.size()); 

	MedalCompleteInfo * pInfo = &(m_vRewardResultList[idx]._info);

	MedalSetBase * pBase = getMedalSet(pInfo->_type);
	if(pBase != nullptr)
		return pBase->getMedalReward(pInfo->_idx, static_cast<UINT8>(pInfo->_level) - 1 );

	return nullptr;
}

i3::rc_wstring CMedalManager::getMedalRewardNameForFirstFail(void)
{
	for(size_t idx = 0 ; idx < m_vRewardResultList.size() ; ++idx)
	{
		if( !m_vRewardResultList[idx]._success )
		{
			MedalCompleteInfo * pInfo = &m_vRewardResultList[idx]._info;
			if(pInfo == nullptr)	continue;

			MEDAL_INFO * pMedal = MedalSetList[pInfo->_type]->getMedal(pInfo->_idx);
			if(pMedal == nullptr)	continue;

			return 	GAME_QUEST_RCSTRING(pMedal->m_szNameKey);	
		}
	}

	return i3::rc_wstring();
}

INT32	 CMedalManager::getMedalRewardResultFailCount(void)
{
	INT32 count = 0;

	for(size_t idx = 0 ; idx < m_vRewardResultList.size() ; ++idx)
	{
		if(!m_vRewardResultList[idx]._success)
			count += 1;
	}

	return count;
}

void CMedalManager::StartRewardResultWait(void)
{
	m_rRewardResultWaitTime = 0.0f;
	m_bIsWaitResult = true;
}

void CMedalManager::EndRewardResultWait(void)
{
	m_bIsWaitResult = false;
	m_vRewardResultList.clear();
}

void CMedalManager::UpdateRewardResultWait(REAL32 dt)
{
	if(!m_bIsWaitResult)
		return;

	m_rRewardResultWaitTime += dt;

	if( m_rRewardResultWaitTime >= MEDAL_REWARD_TIME_OUT )
	{
		m_bIsWaitResult = false;
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_MEDAL_GET_REWARD);
	}
}

void CMedalManager::PopRewardMedal(const MedalCompleteInfo* info)
{
	i3::vector<MedalCompleteInfo>::iterator iter;

	for (iter = m_vRewardMedalList.begin(); iter != m_vRewardMedalList.end(); ++iter)
	{
		if (iter == m_vRewardMedalList.end())
			DEV_LOG("메달 정보가 없음");

		if(iter == info)
		{
			m_vRewardMedalList.erase(iter);

			break;
		}
	}
}

void CMedalManager::PushRewardMedal(MedalViewInfo data)
{
	MedalCompleteInfo info;
	info.SetMedalCompleteInfo((MEDAL_SET_TYPE)data.medal_tabIdx, data.medal_Idx, (MEDAL_LEVEL)data.getLevel());

	m_vRewardMedalList.push_back(info);
}

const MedalCompleteInfo * CMedalManager::getRewardMedal(INT32 idx) const
{
	// 범위 체크가 필요?
	//I3_BOUNDCHK(idx + 1, (INT32)m_vRewardMedalList.size() );
	return &m_vRewardMedalList[idx];
}

/***************************************************************************************
****								Quick Medal Reword								****
***************************************************************************************/
bool CMedalManager::_CheckCompleteMedal(const MEDAL_STATE & data, MedalCompleteInfo & out)
{
	MEDAL_SET_TYPE eType =  MEDAL_SET_TYPE_NORMAL;

	if( data.m_ui8MedalType != MEDAL_TYPE_NORMAL )
		eType = static_cast<MEDAL_SET_TYPE>( getMedalSetTypeByIndex(data.m_ui16MedalIdx));

	// 타입 값이 잘못되면 return
	if(eType == -1)	
		return false;	

	MedalSetBase * pMedalSet = MedalSetList[eType];
	INT32 i32Level = pMedalSet->getCurrentLevel(data.m_ui16MedalIdx);

	// 메달 레벨이 최대면 return
	if(i32Level >= MEDAL_LEVEL_MAX)	
		return false;	

	// 메달을 획득하였는지 확인합니다.
	{
		// 현재 level의 카운트와, 다음 level 의 카운드를 구함.
		MEDAL_INFO * pMedal = pMedalSet->getMedal(data.m_ui16MedalIdx);
		INT32 i32CurLevCnt	= pMedal->m_ui16Count[i32Level];
		INT32 i32NextLevCnt	= pMedal->m_ui16Count[i32Level + 1];

		// 다음 레벨 카운트가 없으면 return
		if(i32NextLevCnt == 0)	
			return false;	


		// 현재 달성한 카운트와, 완료 목표 카운트를 구함.
		INT32 i32AttainCnt		= data.m_ui16Count - i32CurLevCnt;
		INT32 i32CompleteCnt	= i32NextLevCnt - i32CurLevCnt;

		if(i32AttainCnt >= i32CompleteCnt)	i32AttainCnt = i32CompleteCnt;
		if(i32AttainCnt < 0)				i32AttainCnt = 0;

		// 달성 목표에 도달 못했으면 return
		if( i32AttainCnt < i32CompleteCnt)	
			return false;	
	}

	out.SetMedalCompleteInfo(eType, data.m_ui16MedalIdx, static_cast<MEDAL_LEVEL>(i32Level + 1));

	return true;
}

void CMedalManager::PushQuickRewardMedal(const MEDAL_STATE & data)
{
	MedalCompleteInfo info;

	if( !_CheckCompleteMedal(data,info) )
		return;

	if(m_vQuickRewardList.size() >= MAX_QUICK_MEDAL_REWARD_COUNT)
		m_vQuickRewardList.erase(m_vQuickRewardList.begin());

	i3::vector<MedalCompleteInfo>::iterator iter = std::find(m_vQuickRewardList.begin(), m_vQuickRewardList.end(), info);

	if( iter == m_vQuickRewardList.end() )
		m_vQuickRewardList.push_back(info);
}

void CMedalManager::SortQuickRewardList(void)
{
	struct fn_sort_medal_level
	{
		bool operator()(const MedalCompleteInfo & lhs, const MedalCompleteInfo & rhs) const
		{
			return (lhs._level > rhs._level);
		}
	};

	std::sort(m_vQuickRewardList.begin(), m_vQuickRewardList.end(), fn_sort_medal_level() );
}

const MEDAL_INFO * CMedalManager::getMedalFromQuickRewardList(INT32 idx)
{
	I3_BOUNDCHK(idx , (INT32)m_vQuickRewardList.size() );
	MedalCompleteInfo * pInfo = &m_vQuickRewardList[idx];
	return MedalSetList[pInfo->_type]->getMedal(pInfo->_idx);
}

const MedalCompleteInfo * CMedalManager::getQuickRewardMedal(INT32 idx)
{
	I3_BOUNDCHK(idx , (INT32)m_vQuickRewardList.size() );
	return &m_vQuickRewardList[idx];
}

/***************************************************************************************/

INT32	MedalViewInfo::getImageIdx() const 					{	return CMedalManager::i()->getImageIdx(medal_tabIdx, medal_viewIdx);		}
INT32	MedalViewInfo::getLevelProcessCnt(UINT8 lv) const	{	return CMedalManager::i()->getLevelProcessCnt(medal_tabIdx, medal_viewIdx, lv);	}
INT32	MedalViewInfo::getMedalProcessCnt()	const			{	return CMedalManager::i()->getMedalProcessCnt(medal_tabIdx, medal_viewIdx);	}	
UINT8	MedalViewInfo::getCurrentLevel()  const				{	return CMedalManager::i()->getCurrentLevel(medal_tabIdx, medal_viewIdx);	}
UINT8	MedalViewInfo::getMaxLevel()  const					{	return CMedalManager::i()->getMaxLevel(medal_tabIdx, medal_viewIdx); } 
float	MedalViewInfo::getLevelRate(UINT8 lv) const			{	return CMedalManager::i()->getLevelRate(medal_tabIdx, medal_viewIdx, lv);}

bool	MedalViewInfo::isAcquired() const					{	return CMedalManager::i()->isAcquired(medal_tabIdx, medal_viewIdx);	}
bool	MedalViewInfo::isChecked()	const					{	return CMedalManager::i()->isChecked(medal_tabIdx, medal_viewIdx);	}
bool	MedalViewInfo::isNew() const 						{	return CMedalManager::i()->isNew(medal_tabIdx, medal_viewIdx);		}
void	MedalViewInfo::setNew(bool bEnable)					{	CMedalManager::i()->setNew(medal_tabIdx, medal_viewIdx,bEnable);			}
	
i3::rc_wstring	MedalViewInfo::getMedalName() const				{	return CMedalManager::i()->getMedalName(medal_tabIdx, medal_viewIdx);	}
const char*	MedalViewInfo::getMedalDesc() const				{	return CMedalManager::i()->getMedalDesc(medal_tabIdx, medal_viewIdx);	}

bool	MedalViewInfo::isGetReward(UINT8 lv) const			{	return CMedalManager::i()->isGetReward(medal_tabIdx, medal_viewIdx, lv);	}
const MQF_REWARD_INFO* MedalViewInfo::getReward(UINT8 lv)	{	return CMedalManager::i()->getReward(medal_tabIdx, medal_viewIdx, lv);	}

bool	MedalViewInfo::isReceivableReward(void)	const		{	return CMedalManager::i()->isReceivableReward(medal_tabIdx, medal_viewIdx); }

