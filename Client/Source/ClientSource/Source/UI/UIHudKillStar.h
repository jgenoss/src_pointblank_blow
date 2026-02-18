#pragma once

#include "iHudBase.h"
#include "UISlide.h"

class UIHudManager;

namespace kill_star
{
	struct context
	{
		size_t img_idx, kill_cnt;
	};
}

class UIHudKillStar : public iHudBase
{
public:
	UIHudKillStar(UIHudManager* p);
	virtual ~UIHudKillStar();

public:
	virtual void InitializeAtLoad( i3UIScene * pScene) override;
	virtual void Update( REAL32 rDeltaSeconds) override;

	virtual void SetParameter(INT32 arg1, INT32 arg2 = -1) override;

private:
	void event_start_battle(INT32 arg, const i3::user_data& UserData);

private:
	void update_bottom_killstar_mark(bool is_popup_in_container);


private:
	enum { MAX_KILL_STAR = 9, };

private:
	std::deque< kill_star::context > m_bottom_killstar_context_list;
	kill_star::context m_pushed_killstar_context;

	i3UIButtonImageSet* m_BottomKillStar[MAX_KILL_STAR];
	i3UIButtonImageSet* m_CenterKillStar;

	REAL32 m_killstar_elapsed_time;

private:
	UI::tmSlide m_slide;

private:
	void finish_multi_slide();
	UI::TCallBack<UIHudKillStar, &UIHudKillStar::finish_multi_slide> m_slide_callback;
};