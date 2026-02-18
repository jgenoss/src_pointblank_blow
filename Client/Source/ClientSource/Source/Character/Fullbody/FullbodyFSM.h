#ifndef _FULLBODY_FSM_H_
#define _FULLBODY_FSM_H_

#include "FullbodyDef.h"
#include "FSM.h"
#include <unordered_map>

class CGameCharaActionControl;
class CGameCharaAnimControl;
class CGameCharaAnimContext;

namespace fb_fsm
{
	class Event;
	class StateMgr;
	class State;
	class BiStateMap;

	//--------------------------------------------------------------
	// Event : 애니메이션 id를 StateMgr에 전달
	//--------------------------------------------------------------
	class Event : public fsm::IEvent
	{
	public:
		INT32 id;

		explicit Event(INT32 _id);
				 Event(UINT8 st, UINT8 subSt, CGameCharaFullbody* chara);
		virtual ~Event();

		virtual void Notify(fsm::IMachine* m, fsm::IState* s) override;
	};

	//--------------------------------------------------------------
	// StateMgr : State Machine
	//--------------------------------------------------------------
	class StateMgr : public fsm::IMachine
	{
	public:
		StateMgr();
		virtual ~StateMgr();

	public:
		virtual void	Transition(fsm::IState* s) override;

	public:
		void			AddState(fsm::IState* st);
		State*			GetState(INT32 id) const;
		INT32			GetStateSize() const;

		void			SetName(i3::string name);
		const char*		GetName() const;

		void			SetID(INT32 id);
		INT32			GetID() const;

		void			ReserveMemory(size_t size);

		void			SetBiStateMap(BiStateMap* idMap);
		BiStateMap*		GetBiStateMap() const;

	private:
		i3::vector<State*> m_states;
		i3::string			m_name;
		BiStateMap*			m_bistateMap;
		INT32				m_id;
	};

	//--------------------------------------------------------------
	// State : 조건에 따른 애니메이션 호출
	//--------------------------------------------------------------
	class State : public fsm::IState
	{
	public:
		State(CGameCharaFullbody* chara, INT32 id);
		virtual ~State();

	public:
		virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
		virtual void OnExit(fsm::IMachine* m, fsm::IState* s) override;
		virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;

	public:
		INT32 GetID() const { return m_id; }

	protected:
		CGameCharaFullbody*	m_chara;
		INT32				m_id;
	};

	//--------------------------------------------------------------
	// AnimationBase
	//--------------------------------------------------------------
	class AnimationBase : public State
	{
	public:
		AnimationBase(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
			: State(chara, id), m_startAnimID(startAnimID), m_nextAnimID(nextAnimID) {}
		virtual ~AnimationBase() {}

		virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
		virtual void OnExit(fsm::IMachine* m, fsm::IState* s) override;
		virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;

		virtual void OnAnimStart();
		virtual void OnAnimRun(REAL32 tm);
		virtual void OnAnimEnd();

	protected:
		INT32 m_startAnimID;
		INT32 m_nextAnimID;
	};
}

#endif