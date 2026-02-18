#include "pch.h"
#include "UISlide.h"
#include "UIMath.h"
#include <iterator>

namespace detail
{
	class UISlide_Callback_Null : public UISlide_Callback
	{
		virtual void	OnSlideInOnce() {}								// 첫번째 업데이트만..
		virtual void	OnSlidingIn(REAL32 delta_time) {}					// 모든 슬라이딩 업데이트에서
		virtual void	OnSlideInDone() {}

		virtual void	OnSlideOutOnce() {}								// 첫번째 업데이트만..
		virtual void	OnSlidingOut(REAL32 delta_time) {}				// 모든 슬라이딩 업데이트에서
		virtual void	OnSlideOutDone() {}

	} call_back_null;

	class UISlideProcess
	{
	public: virtual void	ProcessUpdate(REAL32 rDeltaTime,  UISlide&	owner) = 0;
	};
	
	class UISlideProcess_Sliding : public UISlideProcess
	{
		virtual void	ProcessUpdate(REAL32 rDeltaTime, UISlide&	owner);
	} tab_sliding;
	
	class UISlideProcess_Done : public UISlideProcess
	{
		virtual void	ProcessUpdate(REAL32 rDeltaTime, UISlide&	owner);
	} tab_slide_done;

	class UISlideProcess_Ready : public UISlideProcess
	{
		virtual void	ProcessUpdate(REAL32 rDeltaTime, UISlide&	owner) {}
	} tab_slide_ready;
	
	class UISlideProcess_Delay : public UISlideProcess
	{
		virtual void	ProcessUpdate(REAL32 rDeltaTime, UISlide&	owner);
	} tab_slide_delay;
	
	class UISlideProcess_WaitLoad : public UISlideProcess
	{
		virtual void	ProcessUpdate(REAL32 rDeltaTime, UISlide&	owner);
	} tab_slide_waitload;



	void	slide_control_by_pos( i3UIControl* ctrl, const VEC2D& orig_pos, const VEC2D& mod_pos, REAL32 ratio)
	{
		VEC2D interpolate;
		interpolate.x = orig_pos.x * ratio + mod_pos.x * (1.f - ratio);
		interpolate.y = orig_pos.y * ratio + mod_pos.y * (1.f - ratio);
		ctrl->setPos(interpolate.x, interpolate.y);
	}

	struct UISlideMtd_ModPos : public ::UISlideMtd
	{
		UISlideMtd_ModPos(i3UIControl* ctrl) { orig_pos.x = ctrl->getPos()->x; orig_pos.y = ctrl->getPos()->y; }
		virtual void self_destroy() { I3_MUST_DELETE( this); }
		virtual void setOriginalPos( REAL32 x, REAL32 y) { setX( &orig_pos, x); setY( &orig_pos, y); }
		VEC2D orig_pos;					
	};

	struct UISlideMtd_ModPosSize : public ::UISlideMtd 
	{
		UISlideMtd_ModPosSize(i3UIControl* ctrl) : orig_size(*ctrl->getSize()) { orig_pos.x = ctrl->getPos()->x; orig_pos.y = ctrl->getPos()->y;  }
		virtual void self_destroy() { I3_MUST_DELETE( this); }
		virtual void setOriginalPos( REAL32 x, REAL32 y) { setX( &orig_pos, x); setY( &orig_pos, y); }
		virtual void setOriginalSize( REAL32 width, REAL32 height) { setX( &orig_size, width); setY( &orig_size, height); }
		VEC2D orig_pos, orig_size;
	};

	struct UISlideMtd_L_TO_R : public UISlideMtd_ModPos
	{
		UISlideMtd_L_TO_R(i3UIControl* ctrl) : UISlideMtd_ModPos(ctrl) {}

		virtual void operator()(i3UIControl* ctrl, REAL32 ratio)
		{
			VEC2D mod =	orig_pos;		
			mod.x -= g_pViewer->GetViewWidth();		// mod.y -= g_pViewer->GetViewHeight();
			
			detail::slide_control_by_pos(ctrl, orig_pos, mod, ratio);
		}

	};
		
	struct UISlideMtd_R_TO_L : public UISlideMtd_ModPos
	{
		UISlideMtd_R_TO_L(i3UIControl* ctrl) : UISlideMtd_ModPos(ctrl) {}

		virtual void operator()(i3UIControl* ctrl, REAL32 ratio)
		{
			VEC2D mod =	orig_pos;		
			mod.x += g_pViewer->GetViewWidth();		// mod.y += g_pViewer->GetViewHeight();

			detail::slide_control_by_pos(ctrl, orig_pos, mod, ratio);
		}
	};
	
	struct UISlideMtd_T_TO_B :  public UISlideMtd_ModPos
	{
		UISlideMtd_T_TO_B(i3UIControl* ctrl) : UISlideMtd_ModPos(ctrl) {}

		virtual void operator()(i3UIControl* ctrl, REAL32 ratio)
		{
			VEC2D mod =	orig_pos;		
			mod.y -= g_pViewer->GetViewHeight();

			detail::slide_control_by_pos(ctrl, orig_pos, mod, ratio);
		}
	};

	struct UISlideMtd_B_TO_T :  public UISlideMtd_ModPos
	{
		UISlideMtd_B_TO_T(i3UIControl* ctrl) : UISlideMtd_ModPos(ctrl) {}

		virtual void operator()(i3UIControl* ctrl, REAL32 ratio)
		{
			VEC2D mod =	orig_pos;		
			mod.y += g_pViewer->GetViewHeight();

			detail::slide_control_by_pos(ctrl, orig_pos, mod, ratio);
		}
	};

	struct UISlideMtd_Popup : public UISlideMtd_ModPosSize
	{
		UISlideMtd_Popup(i3UIControl* ctrl)	: UISlideMtd_ModPosSize(ctrl) {}

		virtual void operator()(i3UIControl* ctrl, REAL32 ratio)
		{	
			REAL32 x, y;
		
			VEC2D vScaled;

			i3Vector::Scale( &vScaled, &orig_size, ratio );

			if( ctrl->getShape( 0) != nullptr)
			{	// 좌측상단 쉐입의 최소 사이즈를 구한다.
				// 이것은 테두리 부분이 안으로 파고 들어가는 것을 막기 위함
				VEC2D v2;
				i3Vector::Set( &v2, ctrl->getShape( 0)->getWidth()*2.f, ctrl->getShape( 0)->getHeight()*2.f);

				x = getX( &orig_size) * ratio + getX( &v2) * (1.f - ratio);
				y = getY( &orig_size) * ratio + getY( &v2) * (1.f - ratio);

				i3Vector::Set( &vScaled, x, y);
			}

			ctrl->setSize(&vScaled);
			
			x = orig_pos.x + (getX( const_cast<VEC2D*>(&orig_size) ) - getX( const_cast<VEC2D*>(&vScaled)) ) * 0.5f;
			y = orig_pos.y + (getY( const_cast<VEC2D*>(&orig_size) ) - getY( const_cast<VEC2D*>(&vScaled)) ) * 0.5f;
			ctrl->setPos( x, y);
		}
	};

	::UISlideMtd*		create_slide_l_to_r(i3UIControl* ctrl) { UISlideMtd_L_TO_R * p = new UISlideMtd_L_TO_R(ctrl); MEMDUMP_NEW(p, sizeof(UISlideMtd_L_TO_R)); return p; }
	::UISlideMtd*		create_slide_r_to_l(i3UIControl* ctrl) { UISlideMtd_R_TO_L * p = new UISlideMtd_R_TO_L(ctrl); MEMDUMP_NEW(p, sizeof(UISlideMtd_R_TO_L)); return p; }
	::UISlideMtd*		create_slide_t_to_b(i3UIControl* ctrl) { UISlideMtd_T_TO_B * p = new UISlideMtd_T_TO_B(ctrl); MEMDUMP_NEW(p, sizeof(UISlideMtd_T_TO_B)); return p; }
	::UISlideMtd*		create_slide_b_to_t(i3UIControl* ctrl) { UISlideMtd_B_TO_T * p = new UISlideMtd_B_TO_T(ctrl); MEMDUMP_NEW(p, sizeof(UISlideMtd_B_TO_T)); return p; }
	::UISlideMtd*		create_slide_popup(i3UIControl* ctrl) { UISlideMtd_Popup * p = new UISlideMtd_Popup(ctrl); MEMDUMP_NEW(p, sizeof(UISlideMtd_Popup)); return p; }
	

	::UISlideMtd*	(*g_aSlideMtdTable[ UISlide::slider_max ])(i3UIControl* ctrl) = 
		{ &create_slide_l_to_r, &create_slide_r_to_l, &create_slide_t_to_b, &create_slide_b_to_t, &create_slide_popup  };
	
	void (UISlide_Callback::*g_SlideOnce[2])() = { &UISlide_Callback::OnSlideOutOnce, &UISlide_Callback::OnSlideInOnce };
	void (UISlide_Callback::*g_Sliding[2])(REAL32) = { &UISlide_Callback::OnSlidingOut, &UISlide_Callback::OnSlidingIn };
	void (UISlide_Callback::*g_SlideDone[2])() = { &UISlide_Callback::OnSlideOutDone, &UISlide_Callback::OnSlideInDone	};
	

	void	UISlideProcess_Sliding::ProcessUpdate(REAL32 rDeltaTime, UISlide&	owner)
	{
		REAL32 fRatio;
		
		REAL32 target_time = owner.m_target_time[ owner.m_is_slide_in ];

		if ( target_time > FLT_MIN)
		{
			owner.m_curr_time += rDeltaTime;
			fRatio = owner.m_curr_time / target_time;	
		}
		else
		{
			fRatio = 1.f + FLT_MIN;
		}
	
		if (fRatio >= 1.f) 
		{
			owner.m_slider = &tab_slide_done;	// fRatio = 1.f;
			owner.m_curr_ratio = REAL32(owner.m_is_slide_in);
		}
		else
		{
			owner.m_curr_ratio = i3Math::sin( (REAL32(!owner.m_is_slide_in) + fRatio) * I3_PI2 );		// 0~90도와 매칭시키면 됨..
		}

		const size_t cnt = owner.m_ctrl_list.size();
	
		for ( size_t i = 0; i < cnt ; ++i )
		{
			UISlideMtd* mtd = owner.m_mtd_list[i];
			i3UIControl*   ctrl = owner.m_ctrl_list[i];
			(*mtd)(ctrl, owner.m_curr_ratio );			
		}
		
		(owner.m_callback->*g_Sliding[owner.m_is_slide_in])(rDeltaTime);
	}	
	
	void	UISlideProcess_Done::ProcessUpdate(REAL32 rDeltaTime, UISlide&	owner)
	{
		owner.m_slider = &tab_slide_ready;
		(owner.m_callback->*g_SlideDone[owner.m_is_slide_in])();
	}
	
	void	UISlideProcess_Delay::ProcessUpdate(REAL32 rDeltaTime, UISlide&	owner)
	{
		owner.m_curr_time += rDeltaTime;

		REAL32 delay_time = owner.m_init_delay_time[ owner.m_is_slide_in ];

		if ( owner.m_curr_time >= delay_time )
		{
			const size_t list_cnt = owner.m_ctrl_list.size();

			owner.m_slider = &tab_sliding;					// 객체 변환..
			owner.m_curr_time = 0.f;						// 로딩직후 rDelta값이 크게 오는 문제가 있을수 있기 때문에 우선 0으로 설정한다..
//			owner.m_curr_time -= owner.m_init_delay_time;
			
			for ( size_t i = 0; i < list_cnt ; ++i )
			{
				i3UIControl*   ctrl = owner.m_ctrl_list[i];
				
				if (ctrl->isEnable() == false) 
					continue;

				if (ctrl->isState( I3UI_STATE_WAITLOAD) == true)
				{
					owner.m_slider = &tab_slide_waitload;
					return;						// 로딩 기다리는 상황이면 m_curr_time계산을 막을 것..
				}
			}
			
			owner.m_is_delay_waitload = false;

			const size_t num_sync_slide = owner.m_sync_slide_list.size();

			for (size_t i = 0 ; i < num_sync_slide ; ++i )
			{
				UISlide* sync_slide = owner.m_sync_slide_list[i];
				if ( sync_slide->m_is_delay_waitload )
				{
					owner.m_slider = &tab_slide_waitload;					// 객체 변환..
					owner.m_curr_time = 0.f;
					return;
				}
			}

			if ( owner.m_parent_slide )		// 자식슬라이드인 경우..부모수준에서 다 처리됬는지 질의
			{
				if ( owner.m_parent_slide->m_waitload_all_done == false )
				{
					owner.m_slider = &tab_slide_waitload;					// 객체 변환..
					owner.m_curr_time = 0.f;
					return;
				}
			}
			
			owner.m_waitload_all_done = true;

			owner.m_slider->ProcessUpdate(0.f, owner);
			(owner.m_callback->*g_SlideOnce[owner.m_is_slide_in])();			// 
			
		}
		
	}

	void	UISlideProcess_WaitLoad::ProcessUpdate(REAL32 rDeltaTime, UISlide&	owner)
	{
		if (owner.m_is_delay_waitload == true)
		{
			const size_t cnt = owner.m_ctrl_list.size();

			for ( size_t i = 0; i < cnt ; ++i )
			{
				i3UIControl*   ctrl = owner.m_ctrl_list[i];

				if (ctrl->isEnable() == false) 
					continue;

				if (ctrl->isState( I3UI_STATE_WAITLOAD) == true)
					return;
			}
		
			owner.m_is_delay_waitload = false;
		}

		const size_t num_sync_slide = owner.m_sync_slide_list.size();
	
		for (size_t i = 0 ; i < num_sync_slide ; ++i )
		{
			UISlide* sync_slide = owner.m_sync_slide_list[i];
			if ( sync_slide->m_is_delay_waitload )
				return;
		}
		
		if ( owner.m_parent_slide )		// 자식슬라이드인 경우..부모수준에서 다 처리됬는지 질의
		{
			if ( owner.m_parent_slide->m_waitload_all_done == false )
				return;
		}
		
		owner.m_waitload_all_done = true;

		owner.m_slider = &tab_sliding;	
		owner.m_slider->ProcessUpdate(0.f, owner);
		(owner.m_callback->*g_SlideOnce[owner.m_is_slide_in])();			// 
		// 로딩 기다리는 상황이면 m_curr_time계산을 막을 것..
	}
	


}

UISlide::UISlide() : m_slider(&detail::tab_slide_ready), m_curr_time(0.f), 
m_callback(&detail::call_back_null), m_is_slide_in(true), m_curr_ratio(0.f), 
m_is_delay_waitload(false), m_waitload_all_done(true), m_parent_slide(nullptr)
{
	m_target_time[0] = m_target_time[1] = 0.f;
	m_init_delay_time[0] = m_init_delay_time[1] = 0.f;
}

UISlide::~UISlide()
{
	UISlide::ClearFrameWndList();
}

void	UISlide::AddFrameWnd(i3UIControl* pWnd, UISlideMtd* mtd)
{
	// 기존 목록에서 콘트롤을 찾도록 수정...
	const size_t num = m_ctrl_list.size();

	for ( size_t i = 0 ; i < num ; ++i )
	{
		i3UIControl* ctrl = m_ctrl_list[i];
		if ( pWnd == ctrl )
		{
			UISlideMtd* list_mtd = m_mtd_list[i];
			if (list_mtd != mtd)
			{
				list_mtd->self_destroy();
				m_mtd_list[i] = mtd;
			}
			return;
		}
	}
			
	m_ctrl_list.push_back(pWnd);
	m_mtd_list.push_back(mtd);
}

void	UISlide::AddFrameWnd(i3UIControl* pWnd, slider_type t )
{
	UISlide::AddFrameWnd(pWnd, (detail::g_aSlideMtdTable[t])(pWnd)  );
}

void	UISlide::AddFrameWnds( i3UIScene* pScn, slider_type t )
{
	for ( i3GameNode* n =	pScn->getFirstChild(); n != nullptr ; n = n->getNext() )
	{
		i3UIControl* ctrl = static_cast<i3UIControl*>(n);
		if ( ctrl->isFrameWnd() )
			UISlide::AddFrameWnd(ctrl, t);
	}
}

void	UISlide::ResetOriginalPos()
{
	// 기존 목록에서 콘트롤을 찾도록 수정...
	const size_t num = m_ctrl_list.size();

	for (size_t i = 0; i < num; ++i)
	{
		i3UIControl* ctrl = m_ctrl_list[i];
		VEC3D* pos = ctrl->getPos();
		m_mtd_list[i]->setOriginalPos(pos->x, pos->y);
	}
}

void	UISlide::ClearFrameWndList()
{
	const size_t num = m_mtd_list.size();

	for (size_t  i = 0 ; i < num ; ++i )
	{
		UISlideMtd* mtd = m_mtd_list[i];
		mtd->self_destroy();
	}

	m_mtd_list.clear();
	m_ctrl_list.clear();
}

void	UISlide::RemoveFrameWnd(i3UIControl* pWnd)
{
	const size_t num = m_mtd_list.size();

	for (size_t i = 0 ; i < num ; ++i )
	{
		i3UIControl* ctrl = m_ctrl_list[i];
		if ( pWnd == ctrl )
		{
			UISlideMtd* mtd = m_mtd_list[i];
			mtd->self_destroy();
			i3::vu::erase_index(m_mtd_list, i);
			i3::vu::erase_index(m_ctrl_list, i);
			break;
		}
	}
	
}

void	UISlide::RemoveFrameWnds(i3UIScene* pScn)
{
	for ( i3GameNode* n =	pScn->getFirstChild(); n != nullptr ; n = n->getNext() )
	{
		i3UIControl* ctrl = static_cast<i3UIControl*>(n);
		if (ctrl->isFrameWnd() )
			UISlide::RemoveFrameWnd(ctrl);
	}

}





void	UISlide::Start(bool Is_In)
{
	m_curr_time = 0.f;
	m_curr_ratio = 1.f * REAL32(!Is_In);
	m_is_slide_in = Is_In;
	m_is_delay_waitload = true;
	m_waitload_all_done = false;

	m_slider = &detail::tab_slide_delay;

	if ( m_ctrl_list.empty() )
		return;
	
	UISlide::_Init_Pos();
}

void	UISlide::ReadyToStart(bool Is_In)
{
	m_curr_time = 0.f;
	m_curr_ratio = 1.f * REAL32(!Is_In);
	m_is_slide_in = Is_In;
	m_is_delay_waitload = false;
	m_waitload_all_done = true;

	m_slider = &detail::tab_slide_ready;

	if ( m_ctrl_list.empty() )
		return;

	UISlide::_Init_Pos();

}

void	UISlide::Restore()
{
	UISlide::ReadyToStart(false);
}
 

void	UISlide::ForceEnd()
{
	if ( m_slider == &detail::tab_slide_done )
	{
		m_slider->ProcessUpdate(0.f, *this);		
	}
	else
	if ( m_slider != &detail::tab_slide_ready )
	{
		m_curr_time = m_target_time[m_is_slide_in];
		m_curr_ratio = 1.f * REAL32(m_is_slide_in);
		m_is_delay_waitload = false;
		m_waitload_all_done = true;

		m_slider = &detail::tab_sliding;
		m_slider->ProcessUpdate(0.f, *this);
	
		m_slider = &detail::tab_slide_done;
		m_slider->ProcessUpdate(0.f, *this);
	}

}

void	UISlide::_Init_Pos()
{

	const size_t cnt = m_ctrl_list.size();

	for ( size_t i = 0; i < cnt ; ++i )
	{
		UISlideMtd* mtd = m_mtd_list[i];
		i3UIControl*   ctrl = m_ctrl_list[i];
		(*mtd)(ctrl, m_curr_ratio);
	}

}

void	UISlide::Update(REAL32 rDeltaTime)
{
	m_slider->ProcessUpdate(rDeltaTime, *this);
}

bool	UISlide::IsSlideDelayWaitLoad() const
{
	//return m_slider == &detail::tab_slide_delay || m_slider == &detail::tab_slide_waitload;
	return !m_waitload_all_done;   //m_is_delay_waitload;
}

void UISlide::SetOriginalPos( REAL32 x, REAL32 y)
{

	for(size_t i = 0; i < m_mtd_list.size(); i++)
	{
		UISlideMtd* mtd = m_mtd_list[i];
		mtd->setOriginalPos( x, y);
	}
}

void UISlide::SetOriginalSize( REAL32 width, REAL32 height)
{
	
	for(size_t i = 0; i < m_mtd_list.size(); i++)
	{
		UISlideMtd* mtd = m_mtd_list[i];
		mtd->setOriginalSize( width, height);
	}
}

void	UISlide::ResetCallback()
{
	m_callback = &detail::call_back_null;
}

UISlide_Callback*	UISlide::GetCallback_Null()
{
	return &detail::call_back_null;
}

void	UISlide::ResetTime()
{
	m_curr_time = 0.f;
	m_slider = &detail::tab_slide_ready;
}

void	UISlide::AddSyncSlide(UISlide* slide)
{
	slide->m_parent_slide = this;
	m_sync_slide_list.push_back(slide);	
}





//- UI::tmSlideAttContainer --------------------//
void UI::tmSlideAttContainer::add(const char* frm_name)
{
	i3UIFrameWnd* wnd = (i3UIFrameWnd*)scene->FindChildByName(frm_name);
	if( !wnd )
	{
		I3PRINTLOG(I3LOG_FATAL, "[tmSlideAttList] i3UIFrameWnd 가 없네.. i3Uie 파일 확인");
	}
	UI::tmSlideAtt * pSlide = new UI::tmSlideAtt(wnd);
	MEMDUMP_NEW( pSlide, sizeof( UI::tmSlideAtt));
	mList.push_back( pSlide);
}

void UI::tmSlideAttContainer::add(const char* frm_name, void * pBox, cb_SlideEvent evt1, cb_EndEvent evt2)
{
	i3UIFrameWnd* wnd = (i3UIFrameWnd*)scene->FindChildByName(frm_name);
	if( !wnd && !pBox)
	{
		I3PRINTLOG(I3LOG_FATAL, "[tmSlideAttList] i3UIFrameWnd 가 없네.. i3Uie 파일 확인");
	}
	UI::tmSlideAtt * pSlide = new UI::tmSlideAtt(wnd, pBox, evt1, evt2);
	MEMDUMP_NEW( pSlide, sizeof( UI::tmSlideAtt));
	mList.push_back( pSlide);
}

void UI::tmSlideAttContainer::set_origin(INT32 x, INT32 y, bool move)
{
	size_t size = mList.size();
	for(size_t i=0; i<size; ++i)
	{
		mList[i]->origin_x = x;	mList[i]->origin_y = y;
		if(move)
		{
			mList[i]->frame->setPos( (REAL32)x, (REAL32)y );
		}
	}
}

void UI::tmSlideAttContainer::set_origin_by_offset(INT32 offset_x, INT32 offset_y, bool move)
{
	size_t size = mList.size();
	for(size_t i=0; i<size; ++i)
	{
		set_origin( mList[i]->origin_x + offset_x, mList[i]->origin_y + offset_y, move);
	}
}

void UI::tmSlideAttContainer::set_target(INT32 x, INT32 y)
{
	size_t size = mList.size();
	for(size_t i=0; i<size; ++i)
	{
		mList[i]->set_target(x, y);
	}
}

void UI::tmSlideAttContainer::set_target_by_offset(INT32 offset_x, INT32 offset_y)
{
	size_t size = mList.size();
	for(size_t i=0; i<size; ++i)
	{
		mList[i]->set_target_by_offset(offset_x, offset_y);
	}
}

void UI::tmSlideAttContainer::set_target_by_offset_and_size(INT32 offset_x, INT32 offset_y, REAL32 size_x, REAL32 size_y)
{
	size_t size = mList.size();
	for(size_t i=0; i<size; ++i)
	{
		//mList[i]->set_target_by_offset(offset_x, offset_y);
		mList[i]->set_target_by_offset_and_size(offset_x, offset_y, size_x, size_y);
	}
}

//- UI::tmLinearSlideMtd --------------------//
void UI::tmLinearSlideMtd::operator()( tmSlideAtt* slide_att, REAL32 ratio)
{
	REAL32 factor = (invert) ? ratio : (1.f - ratio);

	REAL32 x = uiMath::interpolate(factor, (REAL32)slide_att->target_x, (REAL32)slide_att->origin_x);
	REAL32 y = uiMath::interpolate(factor, (REAL32)slide_att->target_y, (REAL32)slide_att->origin_y);
	slide_att->frame->setPos(x, y);
}

void UI::tmPopupSlideMtd::operator()( tmSlideAtt* slide_att, REAL32 ratio)
{
	REAL32 factor = (invert) ? ratio : (1.f - ratio);

	REAL32 xPos = uiMath::interpolate(factor, (REAL32)slide_att->target_x, (REAL32)slide_att->origin_x);
	REAL32 yPos = uiMath::interpolate(factor, (REAL32)slide_att->target_y, (REAL32)slide_att->origin_y);
	slide_att->frame->setPos(xPos, yPos);

	REAL32 xSize = uiMath::interpolate(factor, (REAL32)slide_att->target_size_x, (REAL32)slide_att->origin_size_x);
	REAL32 ySize = uiMath::interpolate(factor, (REAL32)slide_att->target_size_y, (REAL32)slide_att->origin_size_y);
	slide_att->frame->setSize(xSize,ySize);

	cb_SlideEvent *	pEvent = slide_att->getCBSlideEvent();
	if(*pEvent)
		(*pEvent)((void*)(slide_att->frame), (void*)(slide_att->_3dBox));
	if(slide_att->target_size_x == xSize)
	{
		cb_EndEvent * pEndEvent = slide_att->getCBEndEvent();
		if(pEndEvent)
			(*pEndEvent)();
	}

}

//- UI::tmSlide --------------------//
namespace inner
{
	struct tmSlideNullMtd : public UI::tmSlideMtd
	{
		virtual void operator()( UI::tmSlideAtt* ctrl, REAL32 ratio){}
	} nullSlideMtd;

	struct NullSCallBack : public UI::SCallBack
	{
		virtual void Call() {} 
	} nullCallBack;
}

UI::tmSlide::tmSlide() : m_slideMtd(&inner::nullSlideMtd), m_pCallFunc( &inner::nullCallBack )
{
	m_accumulate_time = 0.f;
	m_play_time = 0.f;
}
UI::tmSlide::~tmSlide()
{
	clear();
}

void UI::tmSlide::ResetSlideEndCallBack()
{
	m_pCallFunc = &inner::nullCallBack;
}

void UI::tmSlide::add(const UI::tmSlideAttContainer & list)
{
	std::back_insert_iterator< SlideAttList > back_iter(mSlideAttList);

	std::copy( list.mList.begin(), list.mList.end(), back_iter );
}

UI::tmSlideAtt* UI::tmSlide::add(UI::tmSlideAtt* att)
{
	mSlideAttList.push_back( att );
	return att;
}

void UI::tmSlide::clear()
{
	size_t size = mSlideAttList.size();
	for(size_t i=0; i<size; ++i)
	{
		I3_SAFE_DELETE( mSlideAttList[i]);
	}
	mSlideAttList.clear();
}

void UI::tmSlide::start(UI::tmSlideMtd * mtd, REAL32 play_time)
{
	m_accumulate_time = 0.f;
	m_play_time = play_time;
	m_slideMtd = mtd;
}

void UI::tmSlide::OnUpdate(REAL32 rDeltaTime)
{
	if( m_play_time != 0.f )
	{
		m_accumulate_time += rDeltaTime;

		REAL32 ratio = uiMath::inverse_interpolate(m_accumulate_time, 0.f, m_play_time);
		ratio = uiMath::limited( ratio, 0.0f, 1.0f );

		size_t size = mSlideAttList.size();
		for(size_t i=0; i<size; ++i)
		{
			(*m_slideMtd)(mSlideAttList[ i ], ratio);
		}

		if( ratio >= 1.0f )
		{
			m_accumulate_time = 0.f;
			m_play_time = 0.f;
			m_slideMtd = &inner::nullSlideMtd;
			m_pCallFunc->Call();
		}
	}
}

void UI::tmSlide::move_origin()
{
	size_t size = mSlideAttList.size();
	for(size_t i=0; i<size; ++i)
	{
		mSlideAttList[i]->frame->setPos( (REAL32)mSlideAttList[i]->origin_x, (REAL32)mSlideAttList[i]->origin_y );
	}
	m_play_time = 0.f;
}
void UI::tmSlide::move_target()
{
	size_t size = mSlideAttList.size();
	for(size_t i=0; i<size; ++i)
	{
		mSlideAttList[i]->frame->setPos( (REAL32)mSlideAttList[i]->target_x, (REAL32)mSlideAttList[i]->target_y );
	}
	m_play_time = 0.f;
}

void UI::tmSlide::move_origin_and_size()
{
	size_t size = mSlideAttList.size();
	for(size_t i=0; i<size; ++i)
	{
		mSlideAttList[i]->frame->setPos( (REAL32)mSlideAttList[i]->origin_x, (REAL32)mSlideAttList[i]->origin_y );
		mSlideAttList[i]->frame->setSize(mSlideAttList[i]->origin_size_x, mSlideAttList[i]->origin_size_y);
	}
	m_play_time = 0.f;
}

void UI::tmSlide::move_target_and_size()
{
	size_t size = mSlideAttList.size();
	for(size_t i=0; i<size; ++i)
	{
		mSlideAttList[i]->frame->setPos( (REAL32)mSlideAttList[i]->target_x, (REAL32)mSlideAttList[i]->target_y );
		mSlideAttList[i]->frame->setSize(mSlideAttList[i]->target_size_x, mSlideAttList[i]->target_size_y);
	}
	m_play_time = 0.f;
}

void UI::tmSlide::update_origin_pos(INT32 x, INT32 y, bool move)
{
	size_t size = mSlideAttList.size();
	for(size_t i=0; i<size; ++i)
	{
		mSlideAttList[i]->origin_x = x;	mSlideAttList[i]->origin_y = y;
	}
	if(move) move_origin();
}
void UI::tmSlide::update_origin_pos_by_offset(INT32 offset_x, INT32 offset_y, bool move)
{
	size_t size = mSlideAttList.size();
	for(size_t i=0; i<size; ++i)
	{
		mSlideAttList[i]->origin_x += offset_x;	mSlideAttList[i]->origin_y += offset_y;
	}
	if(move) move_origin();
}


void UI::tmSlide::update_target_pos(INT32 x, INT32 y, bool move)
{
	size_t size = mSlideAttList.size();
	for(size_t i=0; i<size; ++i)
	{
		mSlideAttList[i]->set_target(x,y);
	}
	if(move) move_target();
}
void UI::tmSlide::update_target_pos_by_offset(INT32 offset_x, INT32 offset_y, bool move)
{
	size_t size = mSlideAttList.size();
	for(size_t i=0; i<size; ++i)
	{
		mSlideAttList[i]->target_x = mSlideAttList[i]->origin_x + offset_x;	
		mSlideAttList[i]->target_y = mSlideAttList[i]->origin_y + offset_y;
	}
	if(move) move_target();
}


//- UI::multi_tmSlide --------------------//
namespace inner
{
	struct NullMultiSlideCallBack : public UI::multi_slide_CallBack
	{
		virtual void finish_multi_slide(size_t idx) {

		} 
	} null_multi_slide_callback;
}

UI::multi_tmSlide::multi_tmSlide() : m_pCallFunc( &inner::null_multi_slide_callback )
	, slide_att_index(0)
{
	m_slide_callback.set_obj(this);
	m_slide.SetSlideEndCallBack( &m_slide_callback );
}

UI::multi_tmSlide::~multi_tmSlide()
{
}

void UI::multi_tmSlide::add_slide_att(tmSlideMtd* play_mtd, REAL32 play_time)
{
	multi_slide_att tmp;

	tmp.play_mtd = play_mtd;
	tmp.play_time = play_time;

	multi_slide_att_list.push_back(tmp);
}

void UI::multi_tmSlide::start()
{
	slide_att_index = 0;
	apply_slide_att();
}

void UI::multi_tmSlide::force_begin()
{

}

void UI::multi_tmSlide::force_end()
{

}

void UI::multi_tmSlide::slide_finish()
{
	m_pCallFunc->finish_multi_slide(slide_att_index);
	if( slide_att_index >= multi_slide_att_list.size() ) return;

	++slide_att_index;
	apply_slide_att();
}

void UI::multi_tmSlide::apply_slide_att()
{
	const multi_slide_att& att = multi_slide_att_list[slide_att_index];
	m_slide.start( att.play_mtd, att.play_time );
}

void UI::multi_tmSlide::OnUpdate(REAL32 rDeltaTime)
{
	m_slide.OnUpdate(rDeltaTime);
}