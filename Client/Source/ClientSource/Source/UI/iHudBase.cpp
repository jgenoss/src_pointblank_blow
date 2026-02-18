#include "pch.h"

#include "iHudBase.h"

i3::unordered_map< GAME_EVENT, iHudBase::evt_fn > iHudBase::m_evt_fn_map;
CGameCharaBase* iHudBase::s_pMyChara = nullptr;

namespace
{
	//iHudBase::init_fn() 이 함수를 호출하기 위해 만든 구조체 이다.
	struct init
	{
		init()
		{
			iHudBase::init_fn();
		};
	} ___ginit;
}

void iHudBase::init_fn()
{
	m_evt_fn_map.clear();

	m_evt_fn_map.insert( container::value_type(EVENT_START_BATTLE, &iHudBase::event_start_battle) );
	m_evt_fn_map.insert( container::value_type(EVENT_DEATH_BATTLE, &iHudBase::event_death_battle) );

	//EVENT_FIRST_RESPWAN 이벤트는 서버와 약속된 이벤트가 아니다.
	//게임 시작후 첫 리스폰 이벤트 때 호출되는 이벤트이다. 즉, 게임중에 딱 한번 호출된다.
	m_evt_fn_map.insert( container::value_type(EVENT_FIRST_RESPWAN_BATTLE, &iHudBase::event_first_respawn_battle) );
	m_evt_fn_map.insert( container::value_type(EVENT_RESPAWN_BATTLE, &iHudBase::event_respawn_battle) );
	m_evt_fn_map.insert( container::value_type(EVENT_M_PRE_ROUNDSTART_BATTLE, &iHudBase::event_m_pre_roundstart_battle) );
	m_evt_fn_map.insert( container::value_type(EVENT_M_ROUNDSTART_BATTLE, &iHudBase::event_m_roundstart_battle) );
}

/***********************//***********************/
void FrameList::add(i3UIScene* p, const char* n)
{
	add( FindControl<i3UIFrameWnd*>(p, n) );
}

void FrameList::add(i3UIFrameWnd* p)
{
	p->EnableCtrl(false);
	m_list.push_back( p );
}

void FrameList::show()
{
	struct Fn	
	{	
		void operator()(i3UIFrameWnd* p) 	
		{		
			p->EnableCtrl(true);	
		}
	};
	std::for_each(m_list.begin(), m_list.end(), Fn() );
}

void FrameList::show(size_t idx)
{
	i3UIFrameWnd* p = m_list[idx];
	p->EnableCtrl(true);
}

void FrameList::hide()
{
	struct Fn	
	{	
		void operator()(i3UIFrameWnd* p) 	
		{		
			p->EnableCtrl(false);	
		}
	};
	std::for_each(m_list.begin(), m_list.end(), Fn() );
}

void FrameList::hide(size_t idx)
{
	i3UIFrameWnd* p = m_list[idx];
	p->EnableCtrl(false);
}

const i3UIFrameWnd* FrameList::getFrameWnd( size_t idx) const
{
	if ( idx < 0 || idx >= m_list.size())
		return nullptr;

	return m_list[idx];
}

i3UIFrameWnd* FrameList::getFrameWnd(size_t idx)
{
	if (idx < 0 || idx >= m_list.size())
		return nullptr;

	return m_list[idx];
}


void FrameList::clear()
{
	m_list.clear();
}



/***********************//***********************/
iHudBase::iHudBase(UIHudManager* parent) : m_parent(parent), m_enable(1)
{
}

void iHudBase::SetEnable(bool enable)
{
	m_enable = enable;
	SetVisible(enable);
}

void iHudBase::SetVisible(bool visible)
{
	m_visivle = visible;

	if(m_enable && visible) m_FrameList.show();
	else m_FrameList.hide();
}

void iHudBase::SetVisibleFrame(bool visible, UINT32 idx)
{
	if(m_enable && visible)	m_FrameList.show(idx);
	else					m_FrameList.hide(idx);
}

void iHudBase::AddFrameWnd(i3UIScene* p, const char* n)
{
	m_FrameList.add(p,n);
}
void iHudBase::AddFrameWnd(i3UIFrameWnd* p)
{
	m_FrameList.add(p);
}

void iHudBase::OnEvent(GAME_EVENT type, INT32 arg, const i3::user_data& UserData)
{
	container::iterator it = m_evt_fn_map.find(type);

	if ( it != m_evt_fn_map.end() )
	{
		evt_fn& mf = it->second;
		(this->*mf)(arg, UserData);
	}
	else
	{
		event_default(arg, UserData);
	}
}