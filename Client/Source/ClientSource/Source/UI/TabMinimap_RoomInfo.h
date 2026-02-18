#pragma once
class TabMinimap_RoomInfo : public i3::shared_ginst<TabMinimap_RoomInfo>
{
public:
	TabMinimap_RoomInfo() {}
	~TabMinimap_RoomInfo() {}

	// 만약 Kill Count가 승리 조건에 있다면, 해당 조건을 반환한다. 그렇지 않다면 0을 반환한다.
	INT32				_GetLimitKillCount(void);
	// Round 제의 Stage인 경우, 전체 Round 회수를 반환한다.
	INT32				_GetTotalRoundCount(void);
};