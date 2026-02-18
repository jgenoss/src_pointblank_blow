#pragma once
#ifndef __CStageInfoUtil__
#define __CStageInfoUtil__

#include "CSI_TypeDef.h"

namespace CStageInfoUtil
{
	void	CreateCSIContextMapData();
	void	AddRuleDatatoCSICotext(MAP_RULE_DATA* ruledata);
	void	AddStageListtoCSICotext(MAP_STAGE_MATCHING* matchingdata);


	i3::vector< CSI_RULE* >&		GetRuleList(bool custom_list = false);
	CSI_STAGE_LIST					GetStageList(INT32 ruleidx);
	CSI_STAGE_LIST					GetStageListofEvent();
	CSI_STAGE_LIST					GetStageListofEvent(INT32 ruleidx);

	CSI_MAP*				GetMapData(STAGE_UID	stage_uid);
	CSI_MAP*				GetMapData(STAGE_ID		stageid	 );
	CSI_MAP*				GetMapData(const char*	map_code_name);
	
	CSI_RULE*				GetRuleData(INT32		ruleidx	 );
	CSI_RULE*				GetRuleData(STAGE_ID	stageid, bool isStageid = true);
	
	CSI_STAGE*				GetStageData(STAGE_ID	stageid	 );
	
	const INT32				GetServerRuleMaxIdx();

	/** \breif 매개변수의 백터에 해당 맵(UID)을 포함(오픈)한 룰들을 전부다 PushBack 해준다. */
	void					GatherRuleDatasAtUID(i3::vector< CSI_RULE*>* vec, STAGE_UID stage_uid);



	/****** CSI_STAGE등으로 판별할 수 있는 각종 예외적인 상항들을 여기다 모아 놓는다. ******/
	
	/** \breif 슬롯모드를 Maxperson으로 판별하여 알려준다. */
	STAGE_SLOT_MODE	GetSlotMode(CSI_STAGE stage);

	/** \breif 모드 값을 포함하는 룰이 있다 정도로 보시면 됩니다.*/
	bool	IsAvailableStageMode(STAGE_MODE stagemode);

	/** \breif stageid로 event타입 검사하기*/
	bool	IsStageEventforStageId(STAGE_ID stageid, STAGE_EVENT eventtype);

	/** \breif 방만들기 팝업에서 설정된 모드와 기본 모드를 제외한 모드가 나오면 안되는 모드들을 검사하는 함수. */
	bool	IsDiableChaneMode(CSI_STAGE stage);
	bool	IsDiableChaneMode(CSI_RULE  rule );

	/** \breif 모드 중 라운드로 진행 되는 모드들 검사 함수. */
	bool	IsModeRoundType(CSI_STAGE stage);

	/** \breif 리스폰이 무한으로 되는 모드들 검사 함수. (IsRoundTypeandEternerlRespawn 도 포함) */
	bool	IsEternerlRespawn(CSI_STAGE stage);

	/** \breif 라운드 모드 중 리스폰이 무한으로 되는 모드들 검사 함수. ex) 방어미션. */
	bool	IsRoundTypeandEternerlRespawn(CSI_STAGE stage);

	/** \breif 강제적으로 라운드 갯수를 정하는 모드들 검사 함수. */
	bool	IsFixedRoundTypeMode(CSI_STAGE stage);

	/** \breif 미션 오브젝트를 사용하는 모드 검사 함수. */
	bool	IsUsingMissionObject(CSI_STAGE stage);

	/** \brief 떨어진 무기에 대한 처리 여부 */
	bool	IsDeletableDroppedWeapon(CSI_STAGE stage);

	/** \brief Observer Mode 상태 여부
		\note 옵션에 따라 Observer상태로 갈 수 있는 경우(GM Observer)도 판단한다. */
	bool	IsObserverModeStage(CSI_STAGE stage);

	/** \brief Observer용 stage 여부 
		\note 리스폰이 한번 뿐인 라운드 모드를 걸러낸다고 생각하면 된다. */
	bool	IsObserverModeStageByOnlyMode(CSI_STAGE stage);

	/** \breief KillMode? 먼지 모르지만 이걸로 멀 거른다. */
	bool	IsKillMode(CSI_STAGE stage);

	/** \breief ModeOption에 따른 SUB_TYPE 제공 함수. ( 미션, 시간) */
	UINT8	getMissionRoundType(CSI_STAGE stage);
};

#endif