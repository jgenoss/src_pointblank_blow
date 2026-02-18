#pragma once
//////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------//
//								Random Map system									//
//----------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////

#define	RANDOMMAP_MAX_COUNT			30			// 랜덤맵 최대 갯수 - 모자르면 늘려줘야함(PopupRandomMap의 체크박스도 같음)

#include "ClientStageInfoUtil.h"

class RandomMapContext : public i3::shared_ginst<RandomMapContext>
{

public:
	RandomMapContext();
	~RandomMapContext();

	void			ClearStageList();
	void			AddStage(CSI_STAGE* stage);
	void			BackupStageList();

	bool			IsStageListEmpty() const;
	bool			Contain(CSI_STAGE* stage) const;
	size_t			GetStageListSize() const;
	CSI_STAGE*		GetStage(size_t idx) const;

	// 서버로부터 받은 랜덤맵 스테이지 리스트 적용(방장 인계 또는 게스트가 룸에 입장 시)
	void			TakeOverStageList();
	bool			IsStageListChanged() const;

	STAGE_MODE		GetRandomStageType() const;
	void			SetRandomStageType(STAGE_MODE val);

	STAGE_MODE		GetPrevStageType() const;
	void			SetPrevStageType(STAGE_MODE val);

	void			setPrevIndex(INT32 Prev);
	INT32			getPrevIndex() const;

private:
	i3::vector<CSI_STAGE*> m_StageOrdList;
	i3::vector<CSI_STAGE*> m_OldStageOrdList;
	
	STAGE_MODE		m_RandomStage;
	STAGE_MODE		m_PrevStage;

	INT32			m_iPrevMap;
};