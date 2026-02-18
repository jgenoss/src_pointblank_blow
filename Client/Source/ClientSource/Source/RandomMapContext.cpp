#include "pch.h"
#include "RandomMapContext.h"
#include "MyRoomInfoContext.h"
#include "UI/UIUtil.h"

RandomMapContext::RandomMapContext()
{
	m_PrevStage = STAGE_MODE_DEATHMATCH;
}
RandomMapContext::~RandomMapContext()
{
	m_StageOrdList.clear();
	m_OldStageOrdList.clear();
}

void RandomMapContext::ClearStageList()
{
	m_StageOrdList.clear();
}

void RandomMapContext::AddStage(CSI_STAGE* ord)
{
	m_StageOrdList.push_back(ord);
}

void RandomMapContext::BackupStageList()
{
	m_OldStageOrdList = m_StageOrdList;
}

bool RandomMapContext::IsStageListEmpty() const
{
	return m_StageOrdList.empty(); 
}

bool RandomMapContext::Contain(CSI_STAGE* ord) const
{
	return (i3::vu::int_index_of(m_StageOrdList, ord) == -1) ? false : true;
}

size_t RandomMapContext::GetStageListSize() const
{
	return m_StageOrdList.size();
}

CSI_STAGE* RandomMapContext::GetStage(size_t idx) const
{
	return m_StageOrdList.at(idx);
}

void RandomMapContext::TakeOverStageList()
{
	ROOM_INFO_BASIC info;
	ROOM_INFO_ADD addInfo;

	MyRoomInfoContext::i()->getMyRoomInfo(&info);
	MyRoomInfoContext::i()->getMyRoomInfoAdd(&addInfo);

	if (info._StageID == 0) return;

	CSI_STAGE_LIST temp_list = CStageInfoUtil::GetStageList(CStageInfoUtil::GetStageData(info._StageID)->m_i32RuleIdx);

	RandomMapContext::i()->ClearStageList();
	for (size_t i=0 ; i< temp_list.GetStageListSize(); i++)
	{
		CSI_STAGE* stage		= temp_list.GetStageData(i);
		STAGE_UID  stage_uid	= stage->GetStageUID();
		if (GET_ENABLE_RANDOM_MAP(addInfo._ui64OpenStage, stage_uid))
			RandomMapContext::i()->AddStage(temp_list.GetStageData(i));
	}
}

bool RandomMapContext::IsStageListChanged() const
{
	if (m_StageOrdList.size() != m_OldStageOrdList.size()) return true;

	i3::vector<CSI_STAGE*> latest(m_StageOrdList);
	i3::vector<CSI_STAGE*> old(m_OldStageOrdList);

	i3::sort(latest.begin(), latest.end());
	i3::sort(old.begin(), old.end());

	return (i3::equal(latest.begin(), latest.end(), old.begin())) ? false : true;
}

STAGE_MODE RandomMapContext::GetRandomStageType() const
{ 
	return m_RandomStage; 
}

void RandomMapContext::SetRandomStageType(STAGE_MODE val)		
{ 
	m_RandomStage = val; 
}

STAGE_MODE RandomMapContext::GetPrevStageType() const
{ 
	return m_PrevStage; 
}

void RandomMapContext::SetPrevStageType(STAGE_MODE val)		
{ 
	m_PrevStage = val; 
}

void RandomMapContext::setPrevIndex(INT32 Prev)	
{  
	m_iPrevMap = Prev;  
}

INT32 RandomMapContext::getPrevIndex() const
{ 
	return m_iPrevMap; 
}