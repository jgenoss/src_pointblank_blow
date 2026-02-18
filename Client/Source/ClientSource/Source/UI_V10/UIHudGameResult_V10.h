#pragma once

#include "../UI/UIHudGameResult.h"

class UIHudGameResult_V10 : public UIHudGameResult
{
public:
	UIHudGameResult_V10(UIHudManager* p);
	virtual ~UIHudGameResult_V10();

protected:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	//virtual void Update( REAL32 rDeltaSeconds);

protected:
	virtual void SetParameter(INT32 arg1, INT32 arg2) override;

private:
	i3UIControl* QuestMatArray[8];
protected://±‚¡∏ inner func
	virtual void update_result_date(result::state state) override;
	virtual void update_end_user_list(const i3::vector< ResultState::UserInform > list[TEAM_COUNT]) override;
};