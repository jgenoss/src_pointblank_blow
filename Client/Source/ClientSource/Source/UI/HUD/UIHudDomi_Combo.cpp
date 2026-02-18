#include "pch.h"
#include "UIHudDomi_Combo.h"

#include "UIHUD_Domination_Manager.h"
#include "../../Mode/Domination/Domination_Defines.h"

#include "../UIMath.h"
#include "../../../../../Common/CommonSource/CommonDediCli.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "BattleSlotContext.h"

namespace DominationUI
{
	namespace f_rat
	{
		struct linear_increase
		{ 
			REAL32 operator()( REAL32 curr, REAL32 min, REAL32 max) { return uiMath::inverse_interpolate( curr, min, max);}
		} g_linear_increase;

		struct linear_decrease
		{
			REAL32 operator()( REAL32 curr, REAL32 min, REAL32 max) { return 1.f - g_linear_increase( curr, min, max);}
		} g_linear_decrease;

		struct sin_increase
		{ 
			REAL32 operator()( REAL32 curr, REAL32 min, REAL32 max) { return i3Math::sin( I3_PI2*(uiMath::inverse_interpolate( curr, min, max)));}
		} g_sin_increase;

		struct _sin_decrease
		{ 
			REAL32 operator()( REAL32 curr, REAL32 min, REAL32 max) { return 1.f - g_sin_increase( curr, min, max);}
		} g_sin_decrease;

		struct cos_decrease
		{
			REAL32 operator()( REAL32 curr, REAL32 min, REAL32 max) { return i3Math::cos( I3_PI2*(uiMath::inverse_interpolate( curr, min, max)));}

		} g_cos_decrease;

		struct cos_increase
		{
			REAL32 operator()( REAL32 curr, REAL32 min, REAL32 max) { return 1.f - g_cos_decrease( curr, min, max);}
		} g_cos_increase;
	}

	void ui_actor::OnUpdate( REAL32 fDeltaTime)
	{
		if( !m_enable) return;

		m_fProgressTime += fDeltaTime;
		if( m_fProgressTime >= m_fPeriod) m_fProgressTime = m_fPeriod;

		if( m_fPeriod <= 0.f) return;

		//
		act();

		if( m_cb.get())
		{
			REAL32 per = m_fProgressTime/m_fPeriod;
			if( (m_cb->use_call && m_cb->target_time <= per))
			{
				(*m_cb)( static_cast<void*>(this));
				m_cb->use_call = false;
			}
		}
	}

	
	//-----------------------------------------------------------------------------------------------------
	ui_fader::ui_fader( i3UIControl* pControl /*= nullptr*/)
		: ui_actor( pControl), m_pText(nullptr), m_fMaxApha(255.f), m_fMinApha(0.f), style(FADE_IN)
	{
		if( pControl && pControl->same_of( i3UIStaticText::static_meta()))
			m_pText = (i3UIStaticText*)pControl;
	}

	void ui_fader::start( FADE_CONTROL_TYPE eType, REAL32 fPeriod, bool use_call_back /*= false*/, REAL32 target_time /*= 0.f*/)
	{
		if( m_pText == nullptr) return;
		style = eType; m_fPeriod = fPeriod; m_fProgressTime = 0.f;

		if( m_pText)
		{
			COLOR col;
			i3Color::ConvertRGBA32( m_pText->getTextColor(), &col);

			col.a = (style == FADE_IN) ? (0) : (255);
			m_pText->SetTextColor( &col);
		}
		else
		{
			COLOR* col = m_pControl->getColor();
			col->a = (style == FADE_IN) ? (0) : (255);
			m_pControl->setColor( col);
		}

		m_pControl->EnableCtrl( true);
		if( m_cb.get())
		{
			m_cb->use_call = use_call_back;
			m_cb->target_time = target_time;
		}
	}

	void ui_fader::act()
	{
		if( m_pControl == nullptr) return;
		REAL32 ratio = 0.f;
		
		if( style == FADE_IN) ratio = f_rat::g_sin_increase( m_fProgressTime, 0.f, m_fPeriod);
		else ratio = f_rat::g_sin_decrease( m_fProgressTime, 0.f, m_fPeriod);

		REAL32 rApha = uiMath::interpolate( ratio, m_fMinApha, m_fMaxApha);

		if( m_pText)
		{
			COLOR col;
			i3Color::ConvertRGBA32( m_pText->getTextColor(), &col);

			col.a = static_cast<UINT8>(rApha);
			m_pText->SetTextColor( &col);
		}
		else
		{	
			COLOR* col = m_pControl->getColor();
			col->a = static_cast<UINT8>(rApha);
			m_pControl->setColor( col);
		}
	}

	void ui_fader::restore()
	{
		COLOR col;
		i3Color::ConvertRGBA32( m_pText->getTextColor(), &col);

		col.a = 255;
		m_pText->SetTextColor( &col);
	}


	//-----------------------------------------------------------------------------------------------------
	ui_slider::ui_slider( i3UIControl* pControl /*= nullptr*/)
		: ui_actor( pControl), style(SLIDE_IN)
	{
		if( pControl) { i3Vector::Copy( &m_origin, pControl->getPos());}
	}
  
	void ui_slider::start( SLIDE_TYPE e, REAL32 fPeriod, bool use_call_back /*= false*/, REAL32 target_time /*= 0.f*/)
	{
		style = e; m_fProgressTime = 0.f; m_fPeriod = fPeriod;
	
		if( style == SLIDE_IN) m_pControl->setPos( &m_start);
		else m_pControl->setPos( &m_end);

		m_pControl->EnableCtrl( true);

		if( m_cb.get())
		{
			m_cb->use_call = use_call_back;
			m_cb->target_time = target_time;
		}
	}

	void ui_slider::act()
	{
		REAL32 ratio = 0.f;

		if( style == SLIDE_IN) ratio = f_rat::g_sin_increase( m_fProgressTime, 0.f, m_fPeriod);
		else ratio = f_rat::g_sin_decrease( m_fProgressTime, 0.f, m_fPeriod);

		//if( style == SLIDE_OUT)
			//I3TRACE(" >>>> slide out ratio %.3f\n", ratio);

		REAL32 x = uiMath::interpolate( ratio, (REAL32)m_start.x, (REAL32)m_end.x);
		REAL32 y = uiMath::interpolate( ratio, (REAL32)m_start.y, (REAL32)m_end.y);
		m_pControl->setPos( x, y);
	}


	//-----------------------------------------------------------------------------------------------------
	ui_text_scaler::ui_text_scaler( i3UIControl* pControl /*= nullptr*/)
		: ui_actor( pControl), m_pText(nullptr), style(TEXT_SCALE_NORMAL)
	{
		if( pControl && pControl->same_of( i3UIStaticText::static_meta()))
		{
			m_pText = (i3UIStaticText*)pControl;
			m_target_size = m_origin_size = _HeightToPoint( m_pText->getFontSize());
		}
	}

	void ui_text_scaler::start( TEXT_SCALE_TYPE e, REAL32 fPeriod, bool use_call_back /*= false*/, REAL32 target_time /*= 0.f*/)
	{
		if( m_pText == nullptr) return;
		style = e; m_fProgressTime = 0.f; m_fPeriod = fPeriod;

		restore();

		m_pControl->EnableCtrl( true);

		if( m_cb.get())
		{
			m_cb->use_call = use_call_back;
			m_cb->target_time = target_time;
		}
	}

	void ui_text_scaler::act()
	{
		if( m_pText == nullptr) return;
		REAL32 ratio = 0.f;
		
	 	if( style == TEXT_SCALE_NORMAL) ratio = f_rat::g_linear_increase( m_fProgressTime, 0.f, m_fPeriod);
		else
		{
			REAL32 half_time = m_fPeriod*0.5f;
			if( m_fProgressTime <= half_time) 
			{
				ratio = f_rat::g_linear_increase( m_fProgressTime, 0.f, half_time);
				//I3TRACE(" -- proress time %.3f   half_time %.3f\n", m_fProgressTime, half_time);
			}
			else 
			{
				ratio = f_rat::g_linear_decrease( m_fProgressTime, half_time, m_fPeriod);
				//I3TRACE(" -- proress time %.3f   half_time %.3f     period  %.3f\n", m_fProgressTime, half_time, m_fPeriod);
			}
		}

		INT32 size = uiMath::interpolate( ratio, m_origin_size, m_target_size);
		m_pText->SetFont( m_pText->getFontName(), /*static_cast<INT32>*/(size), FW_NORMAL, false);

		//I3TRACE("origin %d  target %d  scale size %d\n", m_origin_size, m_target_size, /*static_cast<INT32>*/(size));
	}

	void get_combo_text_state( INT32 combo_level, i3::string& out, COLOR& col)
	{
		col.a = 255;
		switch( combo_level)  
		{  
		case 1:
			out = "GOOD!"; col.r = 5; col.g = 250; col.b = 5;  
			break;

		case 2:
			out = "GREAT!!"; col.r = 255; col.g = 228; col.b = 5;  
			break;

		case 3:
			out = "AMAZING!!!"; col.r = 5; col.g = 66; col.b = 237;  
			break;

		case 4:
			out = "PERFECT!!!!"; col.r = 110; col.g = 47; col.b = 199;  
			break;

		case 5:
			out = "RAMPAGE!!!!!"; col.r = 250; col.g = 5; col.b = 5;  
			break;

		default:
			out = "???????"; col.r = 250; col.g = 250; col.b = 250;  
			break;
		}
	}

	combo_scroll_updater::combo_scroll_updater() : current_state(UPDATE_EMPTY), m_combo_level(0)
	{
		updates[UPDATE_EMPTY] = &combo_scroll_updater::update_empty;
		updates[UPDATE_CHANGE] = &combo_scroll_updater::update_change;
		updates[UPDATE_END] = &combo_scroll_updater::update_end;
	}

	combo_scroll_updater::~combo_scroll_updater()
	{
	}

	void combo_scroll_updater::set_combo( INT32 combo)
	{
		if( combo == 0)
		{
			current_state = UPDATE_END;
			m_combo_level = 0;

			if( m_main_scroll->isEnable())
				m_main_scroll->setScrollState( i3UIEditBox::UBSS_FLOW);
			else
				m_main_scroll->setScrollState( i3UIEditBox::UBSS_END);

		}
		else if( combo >= 5)
		{
			INT32 level = combo / 5;
			if( level > 5)
				level = 5;

			if( level != m_combo_level && level <= 5)
			{
				current_state = UPDATE_CHANGE;

				if( m_main_scroll->getScrollState() == i3UIEditBox::UBSS_STOP)
					m_main_scroll->setScrollState( i3UIEditBox::UBSS_FLOW);
				else
					m_main_scroll->EnableCtrl( false);

				i3::string buf = "";
				COLOR col;
				get_combo_text_state( level, buf, col);

				i3::wstring wstrText;
				{
					i3::mb_to_utf16( buf, wstrText);
				}

				m_sub_scroll->SetText( wstrText.c_str());
				m_sub_scroll->SetTextColor( &col);

				m_sub_scroll->setScrollState( i3UIEditBox::UBSS_INIT);
				m_sub_scroll->OnUpdate( 0.016f);
				m_sub_scroll->EnableCtrl( true);

				m_combo_level = level;
			}
		}
	}

	void combo_scroll_updater::update_change( REAL32 fDeltaSec)
	{
		INT32 text_width = m_sub_scroll->getUIText()->getExtentWidth();
		REAL32 current_offset = m_sub_scroll->getScrollCurrentOffset();

		bool stop_sub = false;
		bool end_main = !m_main_scroll->isEnable();

		if( (m_sub_scroll->getWidth()/2.f - text_width/2.f) >= current_offset)
		{
			m_sub_scroll->setScrollState( i3UIEditBox::UBSS_STOP);
			stop_sub = true;
		}

		if( m_main_scroll->getScrollState() == i3UIEditBox::UBSS_END)
		{
			m_main_scroll->EnableCtrl( false);
			end_main = true;
		}

		if( stop_sub && end_main)
		{
			current_state = UPDATE_EMPTY;
			m_main_scroll->setScrollState( i3UIEditBox::UBSS_END);
			
			i3UIEditBox* temp = m_main_scroll;
			m_main_scroll = m_sub_scroll;
			m_sub_scroll = temp;
		}
	}

	void combo_scroll_updater::update_end( REAL32 fDeltaSec)
	{
		if( m_main_scroll->getScrollState() == i3UIEditBox::UBSS_END)
		{
			current_state = UPDATE_EMPTY;
			m_main_scroll->EnableCtrl( false);
		}
	}
};

LuaFuncReg UIHUDDomi_Combo_Glue[] = {
	{nullptr,				nullptr}
};

I3_CLASS_INSTANCE(CUIHudDomi_Combo);

CUIHudDomi_Combo::CUIHudDomi_Combo(void) : 
m_pFrameWnd(nullptr),
m_have_add(false), m_is_comboing(false), m_is_working(false), m_combo_time(0.f), m_combo(0), m_add(0), m_sum(0)
{
	memset( m_FadeList, 0, sizeof(dui::ui_fader*)*dui::combo_control_max);
	memset( m_SlideList, 0, sizeof(dui::ui_slider*)*dui::combo_control_max); 
	memset( m_SlideList, 0, sizeof(dui::ui_text_scaler*)*dui::combo_control_max); 
}

CUIHudDomi_Combo::~CUIHudDomi_Combo(void)
{
	for( INT32 i = 0; i < dui::combo_control_max; ++i)
	{
		I3_SAFE_DELETE( m_FadeList[i]);
		I3_SAFE_DELETE( m_SlideList[i]);
		I3_SAFE_DELETE( m_ScaleList[i]);
	}
}

/*virtual*/ void CUIHudDomi_Combo::OnCreate( i3GameNode* pParent)
{
	UIHUDDominationBase::OnCreate( pParent); 
  
	AddScene( "Scene/Domination/PBRe_Domi_combo.i3UIs", UIHUDDomi_Combo_Glue, true, false);
}

/*virtual*/ void CUIHudDomi_Combo::_InitializeAtLoad( i3UIScene * pScene)
{
	UIHUDDominationBase::_InitializeAtLoad( pScene);

	m_pFrameWnd = (i3UIFrameWnd*)pScene->FindChildByName( "i3UIFrameWnd_combo");
	I3ASSERT( m_pFrameWnd != nullptr);

	for( INT32 i = 0; i < dui::combo_control_max; ++i)
	{
		i3UIControl* pControl = (i3UIControl*)m_pFrameWnd->FindChildByName( dui::g_combo_desc[i]);
		I3ASSERT( pControl != nullptr);

		dui::call_back_ptr _ptr;
		
		//fade
		dui::member_call_back<CUIHudDomi_Combo> * pHUDCombo = new dui::member_call_back<CUIHudDomi_Combo>( this, &CUIHudDomi_Combo::cbFade, reinterpret_cast<void*>(i));
		MEMDUMP_NEW( pHUDCombo, sizeof( dui::member_call_back<CUIHudDomi_Combo>)); 
		_ptr.reset( pHUDCombo);
		m_FadeList[i] = new dui::ui_fader( pControl);
		MEMDUMP_NEW( m_FadeList[i], sizeof(  dui::ui_fader));
		m_FadeList[i]->set_call_back( _ptr);

		//slide
		pHUDCombo = new dui::member_call_back<CUIHudDomi_Combo>( this, &CUIHudDomi_Combo::cbSilde, reinterpret_cast<void*>(i));
		_ptr.reset( pHUDCombo);
		m_SlideList[i] = new dui::ui_slider( pControl);
		MEMDUMP_NEW( m_SlideList[i], sizeof(  dui::ui_slider));
		m_SlideList[i]->set_call_back( _ptr);

		//scale
		pHUDCombo = new dui::member_call_back<CUIHudDomi_Combo>( this, &CUIHudDomi_Combo::cbScale, reinterpret_cast<void*>(i));
		_ptr.reset( pHUDCombo);
		m_ScaleList[i] = new dui::ui_text_scaler( pControl);
		MEMDUMP_NEW( m_ScaleList[i], sizeof( dui::ui_text_scaler));
		m_ScaleList[i]->set_call_back( _ptr);
	}

	setup_fade_slide_scale();
	m_ActiveList.reserve( dui::combo_control_max * 3);
}

/*virtual*/ bool CUIHudDomi_Combo::OnEntranceStart( void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if( UIHUDDominationBase::OnEntranceStart( pArg1, pArg2) == false) return false;

	for( INT32 i = 0; i < dui::combo_control_max; ++i)
		m_SlideList[i]->enable_control( false);
	
	//
	m_scroller.set_scrolls( (i3UIEditBox*)m_SlideList[dui::scroll_main]->get_control(), 
		(i3UIEditBox*)m_SlideList[dui::scroll_sub]->get_control());

	return true;
}

/*virtual*/ void CUIHudDomi_Combo::OnUpdate( REAL32 rDeltaSec)
{
	UIHUDDominationBase::OnUpdate( rDeltaSec);

	bool is_queuing = UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SCORE_QUEUE, dsq::SCORE_E_IS_WORKING_SCORE, -1);
	if( m_is_comboing && is_queuing == false)
	{
		m_combo_time += rDeltaSec;
		if( m_combo_time > (COMBO_CHAIN_TIME + 0.2f))
			SetHudEvent( dui::COMBO_E_END, -1);
	}

	i3::vector<dui::ui_actor*>::iterator itr;
	for( itr = m_ActiveList.begin(); itr != m_ActiveList.end();)
	{
		dui::ui_actor* item = *itr;
		item->OnUpdate( rDeltaSec);

		REAL32 per = item->get_progress() / item->get_period();
		if( per >= 1.f)
		{
			item->set_enable( false);
			itr = m_ActiveList.erase( itr);
			continue;
		}
		++itr;
	}

	m_scroller.update( rDeltaSec);
}

/*virtual*/ bool CUIHudDomi_Combo::SetHudEvent( INT32 nEventNum, INT32 arg /* = nullptr */)
{
	switch( nEventNum)
	{
	case dui::COMBO_E_START:
		//if( m_is_comboing) break;

		//image 슬라이딩
		//push_active_list( m_SlideList[dui::img]);
		//m_SlideList[dui::img]->start( dui::SLIDE_IN, 0.5f, true, 0.8f);
		
		
		//text 슬라이딩
		//push_active_list( m_SlideList[dui::text]);
		//m_SlideList[dui::text]->start( dui::SLIDE_IN, 0.5f, true, 0.8f);

		push_active_list( m_SlideList[dui::bg]);
		m_SlideList[dui::bg]->start( dui::SLIDE_IN, 0.5f, true, 0.8f);
		
		m_is_working = true;
		m_is_comboing = true;
		m_combo_time = 0.f;
		break;

	case dui::COMBO_E_ADD:
		//combo 스케일
		pop_active_list( m_FadeList[dui::combo]);		//진행 되고 있는 Fade 제거.
		pop_active_list( m_SlideList[dui::combo]);		//진행 되고 있는 Slide 제거.

		m_FadeList[dui::combo]->restore();
		m_SlideList[dui::combo]->restore();
		m_SlideList[dui::combo]->enable_control( true);
		//push_active_list( m_ScaleList[dui::combo]);
		//m_ScaleList[dui::combo]->start( dui::TEXT_SCALE_RESTORE, 0.2f);
		
		{
			i3UIStaticText* text = (i3UIStaticText*)m_FadeList[dui::combo]->get_control();
			char buf[24] = {0,};
			
			sprintf_s( buf, 24, "%d", m_combo);
			text->SetText( buf);
		}
		



		//sum visible
		pop_active_list( m_SlideList[dui::sum_frame]);	//진행 되고 있는 Slide 제거.
		pop_active_list( m_FadeList[dui::sum]);			//진행 되고 있는 Fade 제거.
		pop_active_list( m_FadeList[dui::pts]);			//진행 되고 있는 Fade 제거.
	


		m_FadeList[dui::sum]->restore();
		m_FadeList[dui::pts]->restore();
		m_SlideList[dui::sum_frame]->restore();
		
		

		m_FadeList[dui::sum]->enable_control( true);
		m_FadeList[dui::pts]->enable_control( true);
		m_SlideList[dui::sum_frame]->enable_control( true);

		{
			i3UIStaticText* text = (i3UIStaticText*)m_FadeList[dui::sum]->get_control();
			char buf[24] = {0,};
			
			sprintf_s( buf, 24, "+%d", m_sum);
			text->SetText( buf);
		}


		//add 스라이딩, 페이드 시작
		push_active_list( m_FadeList[dui::add]);
		m_FadeList[dui::add]->start( dui::FADE_OUT, 1.f, true, 0.5f);

		push_active_list( m_SlideList[dui::add]);
		m_SlideList[dui::add]->start( dui::SLIDE_IN, 0.8f);

		{
			i3UIStaticText* text = (i3UIStaticText*)m_FadeList[dui::add]->get_control();
			char buf[24] = {0,};
			
			sprintf_s( buf, 24, "+%d", m_add);
			text->SetText( buf);
		}
		

		m_combo_time = 0.f;
		m_scroller.set_combo( m_combo);
		break;

	case dui::COMBO_E_END:
		//combo 패이드.
		push_active_list( m_FadeList[dui::combo]);
		m_FadeList[dui::combo]->start( dui::FADE_OUT, 0.3f, true, 0.8f);


		//combo 슬라이드
		push_active_list( m_SlideList[dui::combo]);
		m_SlideList[dui::combo]->start( dui::SLIDE_IN, 0.3f);

		m_is_comboing = false;
		m_combo = 0;

		//끄기.
		m_scroller.set_combo( m_combo);
		break;

	case dui::COMBO_E_RECEIVE:
		{
			dui::combo_info* info = reinterpret_cast<dui::combo_info*>(arg);
			if( info->combo < 2) break;

			UINT32 prev_combo = m_combo;
			m_combo = info->combo;

			if( m_have_add)
			{
				m_sum += m_add;
				{//바로 더해준다.
					i3UIStaticText* text = (i3UIStaticText*)m_FadeList[dui::sum]->get_control();
					char buf[24] = {0,};
					
					sprintf_s( buf, 24, "+%d", m_sum);
					text->SetText( buf);
				}
			}
			m_add = (info->calculated_score);
			m_have_add = true;


			if( m_is_comboing == false || prev_combo > m_combo)
			{
				// combo를 시작하기 전 sum을 total score에 더하는 중일 경우.
				if( m_sum > 0) 
				{
					domi_state_info::i()->total_score[BattleSlotContext::i()->getMySlotIdx()/2] += m_sum;
					UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::flow_state);

					m_sum = 0;
				}
				SetHudEvent( dui::COMBO_E_START, -1);
			}
			else
			{
				SetHudEvent( dui::COMBO_E_ADD, -1);
			}
			//debug code..
			//I3TRACE("-> process combo sum(%d) flow total socre(%d)\n", m_sum, tinst<domi_state_info>()->total_score[BattleSlotContext::i()->getMySlotIdx()/2]);

		}
		break;

	case dui::COMBO_E_IS_COMBOING:	return m_is_comboing;
	case dui::COMBO_E_IS_WORKING:	return m_is_working;
	}

	return false;
}

void CUIHudDomi_Combo::cbFade( void* caller, void* pData)
{
	INT32 d = reinterpret_cast<INT32>(pData);
	dui::COMBO_CONTROL e = dui::COMBO_CONTROL(d);
	switch(e)
	{
	case dui::add:
		m_sum += m_add;
		m_have_add = false;

		m_add = 0;

		//sum 스케일
		//push_active_list( m_ScaleList[dui::sum]);
		//m_ScaleList[dui::sum]->start( dui::TEXT_SCALE_RESTORE, 0.15f);

		{
			i3UIStaticText* text = (i3UIStaticText*)m_FadeList[dui::sum]->get_control();
			char buf[24] = {0,};
			
			sprintf_s( buf, 24, "+%d", m_sum);
			text->SetText( buf);
		}

		{
			//인터 미션 일 경우는 바로 끝내기.
			bool have_score = UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SCORE_QUEUE, dsq::SCORE_E_IS_WORKING_SCORE, -1);
			if( tinst<domi_state_info>()->is_wave_end && have_score == false)
				SetHudEvent( dui::COMBO_E_END, -1);
		}

		break; 

	case dui::combo:
		//image 슬라이딩
		//push_active_list( m_SlideList[dui::img]);
		//m_SlideList[dui::img]->start( dui::SLIDE_OUT, 0.5f, true, 0.6f);


		//text 슬라이딩
		//push_active_list( m_SlideList[dui::text]);
		//m_SlideList[dui::text]->start( dui::SLIDE_OUT, 0.5f, true, 0.6f);

		if( m_is_comboing == false)
		{//combo 중이 아닐 경우만 bg slider out!!
		//combo_end(dui::combo fade 등록 함..) -> combo_start(dui::bg slider in 등록 함..) 연속으로 할 경우
		//dui::combo fade가 dui::bg slider out 을 호 출 할 수 있음.!!.
			push_active_list( m_SlideList[dui::bg]);
			m_SlideList[dui::bg]->start( dui::SLIDE_OUT, 0.5f, true, 0.6f);
		}
		break;
	}
}

void CUIHudDomi_Combo::cbSilde( void* caller, void* pData)
{
	INT32 d = reinterpret_cast<INT32>(pData);
	dui::COMBO_CONTROL e = dui::COMBO_CONTROL(d);
	switch(e)
	{ 
	case dui::bg:
		if( m_is_comboing)
		{
			SetHudEvent( dui::COMBO_E_ADD, -1);
		}
		else
		{
			//sum 슬라이드.
			push_active_list( m_SlideList[dui::sum_frame]);			
			m_SlideList[dui::sum_frame]->start( dui::SLIDE_IN, 0.4f, true, 0.9f);
		}
		break;    

	case dui::sum_frame:
		
		//sum 패이드.
		push_active_list( m_FadeList[dui::sum]);
		m_FadeList[dui::sum]->start( dui::FADE_OUT, 0.2f);

		push_active_list( m_FadeList[dui::pts]);
		m_FadeList[dui::pts]->start( dui::FADE_OUT, 0.2f);

		{
			domi_state_info::i()->total_score[BattleSlotContext::i()->getMySlotIdx()/2] += m_sum;
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::flow_state);
			UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_USER_STATE, dus::USER_STATE_E_SYNC);

			m_sum = 0;
			m_is_working = false;
		}

		//sum 스케일.
		//push_active_list( m_ScaleList[dui::sum]);
		//m_ScaleList[dui::sum]->start( dui::TEXT_SCALE_NORMAL, 0.13f);
		break;
	}
}  

void CUIHudDomi_Combo::cbScale( void* caller, void* pData)
{
}

bool CUIHudDomi_Combo::push_active_list( dui::ui_actor* p)
{
	if( p->is_enalbe() == false)
	{
		m_ActiveList.push_back( p);
		p->set_enable( true);
     
		return true;
	}

	return false;
}

void CUIHudDomi_Combo::pop_active_list( dui::ui_actor* p)
{   
	if( i3::vu::remove( m_ActiveList, p))
	{ 
		p->set_enable( false);
	}
}

void CUIHudDomi_Combo::setup_fade_slide_scale()
{
	i3UIControl* pControl;
	VEC3D start, end;
	VEC3D* pos;

	//slide 부분.
	{
		pControl = m_SlideList[dui::bg]->get_control(); 
		I3ASSERT( pControl != nullptr);
		pos = m_SlideList[dui::bg]->get_origin();
		i3Vector::Set( &start, pControl->getWidth() + 20.f, pos->y, pos->z);
		i3Vector::Copy( &end, pos);
		m_SlideList[dui::bg]->set_target( &start, &end);


		pos = m_SlideList[dui::add]->get_origin();
		i3Vector::Set( &end, pos->x, pos->y - 45.f, pos->z);
		m_SlideList[dui::add]->set_target( pos, &end);
  

		pos = m_SlideList[dui::combo]->get_origin();
		i3Vector::Set( &end, pos->x, pos->y - 15.f, pos->z);
		m_SlideList[dui::combo]->set_target( pos, &end);


		pos = m_SlideList[dui::sum_frame]->get_origin();
		INT32 screen_width = i3UI::getManager()->getWidth();
		INT32 screen_height = i3UI::getManager()->getHeight();
		VEC3D* frame_pos = m_pFrameWnd->getPos();
		i3Vector::Set( &end, (screen_width*0.6f) - frame_pos->x, (screen_height*0.05f) - frame_pos->y, 0.f);
		m_SlideList[dui::sum_frame]->set_target( pos, &end);
	}

	//scale 부분.
	{
		//m_ScaleList[dui::combo]->set_target( 28);
		//m_ScaleList[dui::sum]->set_target( 30);
	}
}