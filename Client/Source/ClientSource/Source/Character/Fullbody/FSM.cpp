#include "pch.h"
#include "FSM.h"

using namespace fsm;

//--------------------------------------------------------------
// IState : IMPLEMENTATION
//--------------------------------------------------------------
IState::IState() {}
IState::~IState() {}

void IState::OnEntry(IMachine* m, IState* s) {}
void IState::OnExit(IMachine* m, IState* s) {}
void IState::OnEvent(IMachine* m, IEvent* s) {}

void IState::Transition(IMachine* m, IState* s)
{
	if (m)
	{
		m->Transition(s);
	}
}

//--------------------------------------------------------------
// IMachine : IMPLEMENTATION
//--------------------------------------------------------------
IMachine::IMachine() : m_currState(0), m_prevState(0) {}
IMachine::~IMachine() {}

IState* IMachine::GetCurState() const
{
	return m_currState;
}

IState* IMachine::GetPrevState() const
{
	return m_prevState;
}

void IMachine::Transition(IState* current)
{
	m_prevState = m_currState;
	if (m_prevState)
	{
		m_prevState->OnExit(this, current);
	}

	m_currState = current;
	if (current)
	{
		current->OnEntry(this, m_prevState);
	}
}

void IMachine::Notify(IEvent* e)
{
	if (e)
	{
		e->Notify(this, m_currState);
	}
}

//--------------------------------------------------------------
// IEvent : IMPLEMENTATION
//--------------------------------------------------------------
IEvent::IEvent() {}
IEvent::~IEvent() {}

void IEvent::Notify(IMachine* m, IState* s)
{
	if (s)
	{
		s->OnEvent(m, this);
	}
}