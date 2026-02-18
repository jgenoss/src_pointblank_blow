#include "pch.h"
#include "UIHudKillStar.h"

#include "UIHudDefine.h"
#include "UIHudUtil.h"
#include "UI/UIUtil.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

namespace inner
{
	enum {SRC_WIDTH = 1024, };

	const REAL32 killstar_tartget_time = 2.5f;
	const INT32 CK_OFFSET = 65;
	INT32 scn_middle_x = (SRC_WIDTH>>1);

	VEC2D center_killstar_origin_size;

	struct tmResizeSlideMtd : public UI::tmSlideMtd
	{
		virtual void operator()( UI::tmSlideAtt* ctrl, REAL32 ratio)
		{
			COLOR* color = ctrl->frame->getColor();

//			REAL32 alpha = uiMath::limited(alpha, 0.0f, 1.0f);
			color->a = (UINT8)(255 * ratio);

			ctrl->frame->setColor( color );

			//ctrl->frame->getColor()

			//REAL32 scale_w = center_killstar_origin_size.x * ratio;
			//REAL32 scale_h = center_killstar_origin_size.y * ratio;

	
		}
	} tm_resize_slide_mtd;
	UI::tmLinearSlideMtd tm_linear_slide_mtd;
}

/*********************************/
UIHudKillStar::UIHudKillStar(UIHudManager* p) : iHudBase(p)
{
//	m_slide.add_slide_att( &inner::tm_resize_slide_mtd, 0.5f );
//	m_slide.add_slide_att( &inner::tm_linear_slide_mtd, 0.5f );
	m_slide_callback.set_obj( this);
	m_slide.SetSlideEndCallBack( &m_slide_callback );
}

UIHudKillStar::~UIHudKillStar()
{
}


/*********************************/
void UIHudKillStar::finish_multi_slide()
{
	i3_prof_func();

//	if( idx == 1 )
	{
		m_CenterKillStar->EnableCtrl(false);

		if(m_pushed_killstar_context.img_idx == HUD::KST_DISABLE)
		{
			m_bottom_killstar_context_list.clear();
		}
		else
		{
			m_bottom_killstar_context_list.push_back( m_pushed_killstar_context );
		}

		update_bottom_killstar_mark(false);
	}
}

/*********************************/
void UIHudKillStar::InitializeAtLoad( i3UIScene * pScene)
{

	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "Killstar");

	m_CenterKillStar = FindControl<i3UIButtonImageSet*>(pScene, "i3UIBtnImageSet_Center");
	inner::center_killstar_origin_size = *m_CenterKillStar->getSize();

	UI::tmSlideAtt* pAtt = new UI::tmSlideAtt( (i3UIFrameWnd*)m_CenterKillStar );
	MEMDUMP_NEW( pAtt, sizeof( UI::tmSlideAtt));
	m_slide.add(pAtt);

	for(size_t i=0;i<MAX_KILL_STAR;++i)
	{
		i3::stack_string str_name;
		i3::sprintf(str_name, "i3UIBtnImageSet%d", i);

		m_BottomKillStar[i] = FindControl<i3UIButtonImageSet*>(pScene, str_name.c_str() );
	}
}

void UIHudKillStar::update_bottom_killstar_mark(bool is_popup_in_container)
{
	i3_prof_func();

	size_t size = m_bottom_killstar_context_list.size(); 

	if(size == MAX_KILL_STAR +1) //MAX_KILL_STAR 를 넘으면 맨 앞부분 제거
	{
		m_bottom_killstar_context_list.pop_front();
		--size;
	}

	for(size_t i = 0; i<MAX_KILL_STAR; ++i )
	{
		if( i < size )
		{
			size_t idx = m_bottom_killstar_context_list[i].img_idx;

			REAL32 x = (REAL32)GameUI::AutoOffset::get_auto_offset_position(GameUI::AOD_MIDDLE,
				inner::scn_middle_x, inner::CK_OFFSET, i, size);

			x -= (m_BottomKillStar[i]->getSize()->x / 2.f);

			REAL32 y = m_BottomKillStar[i]->getPos()->y;
			m_BottomKillStar[i]->setPos(x, y);
			m_BottomKillStar[i]->SetShapeIdx(idx); 
			m_BottomKillStar[i]->EnableCtrl(true);
		}
		else
		{
			m_BottomKillStar[i]->EnableCtrl(false);
		}
	}

	if( is_popup_in_container == true)
	{
		if( size > 0 )
		{
			m_bottom_killstar_context_list.pop_front();
		}
	}
}

void	UIHudKillStar::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	m_slide.OnUpdate(rDeltaSeconds);

	m_killstar_elapsed_time += rDeltaSeconds;
	if( m_killstar_elapsed_time >= inner::killstar_tartget_time )
	{
		update_bottom_killstar_mark(true);
		m_killstar_elapsed_time = 0.f;
	}
}

void UIHudKillStar::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	m_pushed_killstar_context.img_idx = arg1;
	m_pushed_killstar_context.kill_cnt = arg2;

	m_CenterKillStar->EnableCtrl(true);
	m_slide.start(&inner::tm_resize_slide_mtd, 2.5f);
}

/*********************************/
void UIHudKillStar::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	iHudBase::SetEnable(true);

	m_killstar_elapsed_time = 0.f;

	m_CenterKillStar->EnableCtrl(false);
	update_bottom_killstar_mark(false);
}