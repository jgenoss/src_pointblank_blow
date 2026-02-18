#if !defined( __CLIENT_STAGE_INFO_CONTEXT_H)
#define __CLIENT_STAGE_INFO_CONTEXT_H

/*
ClientStageInfoContext

인게임 플레이 하는 환경을 여러가지로 정의 할 수 있으나,
이제 부턴 가장 큰 개념을 Stage 라고 정의한다. ( 실제 엔진 개념을 따르기 위하여 )
해당 개념을 구현하기 위한 여러가지 기초 정보 DATA들을 서버에서 허락(제공)해준 것들만,
클라에 알맞게 저장하고(로그인시 통신으로), 그 후 해당 정보들을 활용 할 수 있도록 DATA들을 제공하는 클래스이다.

Stage		= rule + stage_uid
Rule		= mode + 각종 게임 플레이 특징. ( 자세한건 commondef_stage 참조 )
CSI_STAGE	= MAP_STAGE_MATCHING 클라용.
CSI_RULE	= MAP_RULE_DATA 클라용.
CSI_MAP		= map 데이터가 UID 별로 저장 되어 있음 ( 이름만 들어감 )
*/

#include "CSI_TypeDef.h"

typedef i3::unordered_multimap<INT32, CSI_STAGE>									rule_stage_data_mmap;
typedef rule_stage_data_mmap::iterator												iterator_rule_stage_data_mmap;
typedef i3::pair< iterator_rule_stage_data_mmap, iterator_rule_stage_data_mmap >	iterator_rule_stage_data_pair;

class ClientStageInfoContext : public i3::shared_ginst<ClientStageInfoContext>
{
public:
	
	ClientStageInfoContext();
	~ClientStageInfoContext();

	void	CreateMapDataAtPefData();			
	void	AddRuleDataAtServerData(MAP_RULE_DATA* ruledata);
	void	AddStageListAtServerData(MAP_STAGE_MATCHING* matchingdata);
	void	StrightRuleData();

	const	iterator_rule_stage_data_pair	GetStageList(INT32 ruleidx);
	const	iterator_rule_stage_data_pair	GetStageListofEvent();
	const	iterator_rule_stage_data_pair	GetStageListofEvent(INT32 ruleidx);

	i3::vector< CSI_RULE* >&			GetRuleLists(bool custom_list = false);

	const	CSI_RULE*					GetRuleData(INT32 rule_idx);
	const	CSI_RULE*					GetRuleData(UINT32 rule);
	
	const	CSI_STAGE*					GetStageData(STAGE_ID stageid);

	const	CSI_MAP*					GetMapData(STAGE_UID stage_uid);
	const	CSI_MAP*					GetMapData(const char* map_code_name);

	const	i3::vector< CSI_STAGE >*	GetALLStageList()								{ return &m_vecStageData; }

	const	INT32						GetServerRuleMaxIdx()							{ return m_nServerRuleMaxIdx; }

	/*-- 랜덤맵 개선 부스트 이벤트 값 표출 관련 --*/
	void SetUseRandomMapRenewalSystem(bool bUseRandomMapRenewalSystem) { m_bUseRandomMapRenewalSystem = bUseRandomMapRenewalSystem; }
	bool IsUseRandomMapRenewalSystem() const { return m_bUseRandomMapRenewalSystem; }
	void SetRandomMapEventData(UINT32 n32Exp, UINT32 n32Point);
	const i3::rc_wstring& GetRandomMapEventString() const { return m_RandomMapEventString; }
	bool IsRandomMapEventOpened() const  { return (!(m_nRandomMapEventExp == 0 && m_nRandomMapEventPoint == 0)); } // 랜덤맵 부스트 이벤트중이 아니면 두 값은 0으로 들어온다.

public: 

	INT32	m_nServerRuleMaxIdx;														//서버가 준 룰 인덱스 중 가장 큰걸 저장 해놓는다. (4:4등을 거르기 위하여) 										

private:

	void	MakeMaxPersonRule(	rule_stage_data_mmap&  selecte_hmmstage,
								i3::vector<CSI_RULE*>& target_v_rule, 
								CSI_RULE*			  find_rule );

	i3::vector< CSI_MAP  >		m_vecMapData;											//PEF에 등록되어 있는 모든 맵들 코드네임(PEF탐색용), 표시네임(번역되는거) 저장.

	i3::vector< CSI_RULE* >		m_vecRuleData;
	i3::vector< CSI_RULE* >		m_vecRuleDataMain;										//서버에서 받은 모든 메인 룰에 대한 데이타 저장.
	i3::vector< CSI_RULE* >		m_vecRuleDataCustom;									//서버에서 받은 모든 커스텀 룰에 대한 데이타 저장. ( 무기제한 옵션이 걸린 룰만 )
	
	i3::vector< CSI_STAGE>		m_vecStageData;											//서버에서 받은 모든 스테이지 아이디에 대한 데이타 저장.
	rule_stage_data_mmap			m_hmmStageforRuleId;									//받은 모든 RULE(사용자가 보는 모드)별 받은 논 이벤트 관련 모든 스테이지 정보 저장.
	rule_stage_data_mmap			m_hmmStageofEventforRuleId;								//받은 모든 RULE(사용자가 보는 모드)별 받은 이벤트 관련 모든 스테이지 정보 저장.

	INT32	m_nRuleMaxIdx;																//클라의 룰 인덱스 중 가장 큰걸 저장 해놓는다. (4:4등을 만들기 위하여)

	/*-- 랜덤맵 개선 부스트 이벤트 값 표출 관련 --*/
	UINT32				m_nRandomMapEventExp = 0;
	UINT32				m_nRandomMapEventPoint = 0;
	bool				m_bUseRandomMapRenewalSystem = false;
	i3::rc_wstring		m_RandomMapEventString;
};

#endif