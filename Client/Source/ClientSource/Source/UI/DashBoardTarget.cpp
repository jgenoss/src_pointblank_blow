#include "pch.h"
#include "DashBoardTarget.h"

//#include "iHudBase.h"
#include "UIHudDefine.h"
#include "UIMath.h"

#include "UIHudManager.h"
#include "UIHudMinimap.h"


//#define i3_profile_enable
#include "i3Base/profile/profile.h"

namespace
{
	enum { TURN_TIME = 1000/4, MAX_WARNING_CNT = 14,};
}

//- dboard::target -------------------------//
void dboard::target::init_control(i3UIScene* pScn, const char* parent, const char* targ, const char* bg, const char* progress, bool bMinimapUpdate)
{
	i3_prof_func();

	pImgSet_Wnd = FindControl<i3UIFrameWnd*>(pScn, parent);
	pImgSet_Target = FindControl<i3UIButtonImageSet*>(pScn, targ);
	pImgSet_Bg = FindControl<i3UIButtonImageSet*>(pScn, bg);
	pProgressgBar = FindControl<i3UIProgressBar*>(pScn, progress);

	is_update_warning = false;
	enable_target = false;
	is_finish_update = true;
	is_minimap_update = bMinimapUpdate;
	target_normal_shape_idx = -1;
	target_die_shape_idx = -1;
	lastest_hp = -1;
}

void dboard::target::set_target_normal_shape_idx(INT32 idx)
{
	i3_prof_func();

	target_normal_shape_idx = idx;
	pImgSet_Target->EnableCtrl(true);
	pImgSet_Target->SetShapeIdx(idx);
}

void dboard::target::reset_target_shape()
{
	i3_prof_func();

	if( target_normal_shape_idx != -1 )
	{
		pImgSet_Target->EnableCtrl(true);
		pImgSet_Target->SetShapeIdx(target_normal_shape_idx);
	}

	pImgSet_Bg->EnableCtrl(is_after_warning_bg_enable);
}

void dboard::target::set_target_die_shape_idx(INT32 idx)
{
	target_die_shape_idx = idx;
}

void dboard::target::set_minimap_warning_idx(INT32 idx)
{
	minimap_idx = idx;
}

void dboard::target::set_progress_invert()
{
	i3_prof_func();

	pProgressgBar->set_invert(true);
	pProgressgBar->setProgress(1.0f);
}

void dboard::target::set_bg_idx(INT32 idx, bool enable)
{
	i3_prof_func();

	is_after_warning_bg_enable = enable;
	pImgSet_Bg->EnableCtrl(enable); //이 부분에서 SetEnable 이 함수 호출 하면 작동 안 함..
	pImgSet_Bg->SetShapeIdx(idx);
}

void dboard::target::set_enable_update(bool enable)
{
	is_update_warning = false;

	if(enable_target == true)
		is_finish_update = !enable;
	else
		is_finish_update = true;
}

 void dboard::target::set_hp(INT32 hp, INT32 hp_max)
{
	this->hp = hp; 
	this->max_hp = hp_max;
}

void dboard::target::update_warning(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	delta_time -= INT32(rDeltaSeconds * 1000);
	delta_time = uiMath::limited(delta_time, (INT32)0, (INT32)TURN_TIME);
                    
	if( delta_time == 0 )
	{
		delta_time = TURN_TIME;

		++warning_cnt;
		pImgSet_Bg->EnableCtrl( ( warning_cnt % 2 ) != 0 );

		if( warning_cnt > MAX_WARNING_CNT )
		{
			if ( is_minimap_update)
			{
				UIHudMinimap* mini_map = static_cast<UIHudMinimap*>(HUD::instance()->GetHud(HUD::MINIMAP));
				mini_map->SetObjectUnderAttack(minimap_idx, false);
			}

			pImgSet_Bg->EnableCtrl(is_after_warning_bg_enable);

			is_update_warning = false;               
		}
	}
}

void dboard::target::update(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if( is_finish_update ) return; //더 이상 업데이트 없다.

	if( is_update_warning ) update_warning(rDeltaSeconds); //warning  업데이트

	if(hp == max_hp) //현재 hp가 최대 hp랑 같은 경우...
	{
		lastest_hp = hp; is_update_warning = false; pProgressgBar->setProgress(1.f);
	}
	if( lastest_hp == hp )	return; //현재 hp와 마지막 hp 와 같은 경우엔 더 이상 할 일이 없다.

	//현재 hp가 마지막 hp와 다를 경우 처리 루틴
	lastest_hp = hp;

	REAL32 factor = uiMath::inverse_interpolate( hp, 0, max_hp); //progress bar 위치 조정
	factor = uiMath::limited(factor, 0.f, 1.f);
	pProgressgBar->setProgress(factor);

	if( factor == 0.f ) //progress bar 가 0 이 된 경우
	{
		pImgSet_Bg->EnableCtrl(is_after_warning_bg_enable);
	
		if( target_die_shape_idx != -1)
			pImgSet_Target->SetShapeIdx(target_die_shape_idx);

		is_finish_update = true;
	}

	if ( is_minimap_update)
	{
		UIHudMinimap* mini_map = static_cast<UIHudMinimap*>(HUD::instance()->GetHud(HUD::MINIMAP)); //minimap에 warnig 표시
		mini_map->SetObjectUnderAttack(minimap_idx, (factor == 0.f) ? false : true);
	}

	warning_cnt = 0; //warning 업데이트 시작 한다.
	is_update_warning = true;
	delta_time = TURN_TIME;
}

void dboard::target::update_color()
{
	i3_prof_func();

	float fHpRate = (float)hp / (float)max_hp * 100.f;

	if ( fHpRate >= 80.f)
	{
		pProgressgBar->SetTemplateByName("Ingame10_004");
	}
	else if( fHpRate >= 60.f)
	{
		pProgressgBar->SetTemplateByName("Ingame10_004_0");
	}
	else if (fHpRate >= 40.f)
	{
		pProgressgBar->SetTemplateByName("Ingame10_004_1");
	}
	else if (fHpRate >= 20.f)
	{
		pProgressgBar->SetTemplateByName("Ingame10_004_2");
	}
	else if (fHpRate >= 0.f)
	{
		pProgressgBar->SetTemplateByName("Ingame10_004_3");
	}
}


//- dboard::target_1_5 -------------------------//
void dboard::target_1_5::set_enable(bool enable)
{
	i3_prof_func();

	enable_target = enable;

	pImgSet_Wnd->EnableCtrl(enable);
	pProgressgBar->EnableCtrl(enable);

	if(enable == false )
	{
		pImgSet_Bg->EnableCtrl(false);
	}
	else
	{
		pImgSet_Bg->EnableCtrl(is_after_warning_bg_enable);
	}
}

//- dboard::target_1_0 -------------------------//
void dboard::target_1_0::set_enable(bool enable)
{
	i3_prof_func();

	enable_target = enable;

	pImgSet_Wnd->EnableCtrl(enable);

	if(enable == false )
	{
		pImgSet_Bg->EnableCtrl(false);
	}
	else
	{
		pImgSet_Bg->EnableCtrl(is_after_warning_bg_enable);
	}
}
