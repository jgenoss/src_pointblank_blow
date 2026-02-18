#pragma once

#ifndef __CSI_STAGE_LIST__
#define __CSI_STAGE_LIST__

#include "CSI_TypeDef.h"

typedef i3::unordered_multimap<INT32, CSI_STAGE>									rule_stage_data_mmap;
typedef rule_stage_data_mmap::iterator												iterator_rule_stage_data_mmap;
typedef i3::pair< iterator_rule_stage_data_mmap, iterator_rule_stage_data_mmap >	iterator_rule_stage_data_pair;

class CSI_STAGE_LIST : public i3::class_common_pool<CSI_STAGE_LIST>
{

public:

	CSI_STAGE_LIST();
	~CSI_STAGE_LIST();

	bool  CreateStageList( iterator_rule_stage_data_pair source );
	
	CSI_STAGE*			GetStageData(int stage_idx);
	CSI_STAGE*			GetStageData(STAGE_UID stage_uid);
	size_t				GetStageListSize()					{ return m_vStageList.size(); };

private :

	i3::vector< CSI_STAGE* >	m_vStageList;

};

#endif