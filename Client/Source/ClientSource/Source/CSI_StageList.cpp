#include "pch.h"
#include "CSI_StageList.h"

CSI_STAGE_LIST::CSI_STAGE_LIST()
{
	m_vStageList.clear();
}

CSI_STAGE_LIST::~CSI_STAGE_LIST()
{
	m_vStageList.clear();
}

bool CSI_STAGE_LIST::CreateStageList( iterator_rule_stage_data_pair source)
{
	iterator_rule_stage_data_mmap iter;
	for (iter = source.first; iter != source.second; iter++)
	{
		m_vStageList.push_back(&iter->second);
	}

	return m_vStageList.size() > 0 ? true : false;
}

CSI_STAGE* CSI_STAGE_LIST::GetStageData(int stage_idx)
{
	if ( 
		 ( m_vStageList.size() <= 0 ) || 
		 ( 0 > stage_idx && (size_t)stage_idx > m_vStageList.size() ) 
	   )
		return nullptr;
	else 
		return m_vStageList[stage_idx];
}

CSI_STAGE* CSI_STAGE_LIST::GetStageData(STAGE_UID stage_uid)
{
	if (STAGE_UID_NONE < stage_uid && stage_uid < STAGE_UID_MAX)
	{
		i3::vector< CSI_STAGE* >::iterator iter;
		for (iter = m_vStageList.begin(); iter < m_vStageList.end(); iter++)
		{
			if ((*iter)->GetStageUID() == stage_uid)
				return *iter;
		}
		return nullptr;
	}
	else
		return nullptr;
}