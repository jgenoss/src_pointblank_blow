#include "pch.h"
#include "FullbodyFSM.h"
#include "GameCharaFullbody.h"
#include "FullbodyBiStateMapFactory.h"

using namespace fb_fsm;

//--------------------------------------------------------------
// Event
//--------------------------------------------------------------
Event::Event(INT32 _id) : id(_id)
{
}

Event::Event(UINT8 st, UINT8 subSt, CGameCharaFullbody* chara) : id(0)
{
	BiStateMap* bist = chara->GetFBStateMgr()->GetBiStateMap();
	if (bist)
		id = bist->Find(st, subSt);
}

Event::~Event() 
{
}

void Event::Notify(fsm::IMachine* m, fsm::IState* s)
{
	fsm::IEvent::Notify(m, s);
}


//--------------------------------------------------------------
// StateMgr
//--------------------------------------------------------------
StateMgr::StateMgr() : m_bistateMap(0), m_id(-1)
{
}

StateMgr::~StateMgr()
{
	i3::vector<State*>::const_iterator it;
	for (it = m_states.begin(); it != m_states.end(); it++)
	{
		I3_MUST_DELETE( *it);
	}
	m_states.clear();

	I3_SAFE_DELETE( m_bistateMap);
}

void StateMgr::Transition(fsm::IState* s)
{
#ifdef ENABLE_FB_FSM_TRACE
	State* previous = static_cast<State*>(GetCurState());
	State* current  = static_cast<State*>(s);

	I3TRACE("[%s, %d]Transition : ", m_name, m_id);
	if (previous)	I3TRACE("%s -> ", previous->GetName());
	else			I3TRACE("nullptr -> ");

	if (current)	I3TRACE("%s\n", current->GetName());
	else			I3TRACE("nullptr\n");
#endif

	fsm::IMachine::Transition(s);
}

void StateMgr::AddState(fsm::IState* s)
{
	m_states.push_back(static_cast<State*>(s));
}

State* StateMgr::GetState(INT32 id) const
{
	if (!m_states.empty())
		return m_states[id];

	return nullptr;
}

INT32 StateMgr::GetStateSize() const
{
	return m_states.size();
}

void StateMgr::SetBiStateMap(BiStateMap* bist)
{
	I3_SAFE_DELETE( m_bistateMap);

	if( bist != nullptr)
		m_bistateMap = bist;
}

BiStateMap* StateMgr::GetBiStateMap() const
{
	return m_bistateMap;
}

void StateMgr::SetName(i3::string name)
{
	m_name = name;
}

const char* StateMgr::GetName() const
{
	return m_name.c_str();
}

void StateMgr::SetID(INT32 id)
{
	m_id = id;
}

INT32 StateMgr::GetID() const
{
	return m_id;
}

void StateMgr::ReserveMemory(size_t size)
{
	m_states.reserve(size);
}

//--------------------------------------------------------------
// State
//--------------------------------------------------------------
State::State(CGameCharaFullbody* chara, INT32 id) 
	: m_chara(chara), m_id(id) {}

State::~State() {}

void State::OnEntry(fsm::IMachine* m, fsm::IState* s)
{
#ifdef ENABLE_FB_FSM_TRACE
	StateMgr* sm = static_cast<StateMgr*>(m);
	I3TRACE("[%s, %d]OnEntry : %s\n", sm->GetName(), sm->GetID(), GetName());
#endif
}

void State::OnExit(fsm::IMachine* m, fsm::IState* s)
{
#ifdef ENABLE_FB_FSM_TRACE
	StateMgr* sm = static_cast<StateMgr*>(m);
	I3TRACE("[%s, %d]OnExit : %s\n", sm->GetName(), sm->GetID(), GetName());
#endif
}

void State::OnEvent(fsm::IMachine* m, fsm::IEvent* e)
{
	StateMgr* sm	= static_cast<StateMgr*>(m);
	Event* evt		= static_cast<Event*>(e);

	Transition(sm, sm->GetState(evt->id));
}


//--------------------------------------------------------------
// AnimationBase
//--------------------------------------------------------------
void AnimationBase::OnEntry(fsm::IMachine* m, fsm::IState* s)
{
	State::OnEntry(m, s);
	m_chara->PlayFullbody(m_startAnimID);
}

void AnimationBase::OnExit(fsm::IMachine* m, fsm::IState* s)
{
	State::OnExit(m, s);
}

void AnimationBase::OnEvent(fsm::IMachine* m, fsm::IEvent* e)
{
	if (static_cast<fb_fsm::State*>(m->GetCurState())->GetID() != static_cast<fb_fsm::Event*>(e)->id)
	{
		OnAnimEnd();
		State::OnEvent(m, e);
	}
}

void AnimationBase::OnAnimStart() 
{
}

void AnimationBase::OnAnimRun(REAL32 tm) 
{
}

void AnimationBase::OnAnimEnd() 
{
	// NONE, REPEAT을 제외하고 실행
	// * REPEAT : 캐릭터 에디터에서 반복으로 설정한 경우임을 표시하기 위해 임의로 정의함(엔진에서 호출하므로 여기서 호출할 필요없음)
	if (m_nextAnimID > 0) 
		m_chara->PlayFullbody(m_nextAnimID);
}

