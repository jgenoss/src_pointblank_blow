#ifndef _UIPOPUP_QUICK_JOIN_SECOND_BEST_H_
#define _UIPOPUP_QUICK_JOIN_SECOND_BEST_H_

#include "../UIPopupBase.h"

class UIPopupQuickJoinSecondBest : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupQuickJoinSecondBest, UIPopupBase);

public:
	UIPopupQuickJoinSecondBest();
	virtual ~UIPopupQuickJoinSecondBest();

	virtual void OnCreate(i3GameNode* parent) override;
	virtual void _InitializeAtLoad(i3UIScene* scn) override;
	virtual bool OnEntranceStart(void* arg1, void* arg2) override;
	virtual void OnExitEnd() override;
	virtual void OnUpdate(REAL32 tm) override;
	virtual void ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
	virtual bool OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;

	void AgreeToSecondBestRoom();

private:
	void _SetRecommendRoomInfo();
	void _UpdateRoomPlayTime(REAL32 tm);

private:
	bool m_bAgreeJoin;
	INT32 m_playTime; 
	REAL32 m_tm;
};
#endif