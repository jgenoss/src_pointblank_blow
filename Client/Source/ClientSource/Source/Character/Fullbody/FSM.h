#ifndef _FSM_H_
#define _FSM_H_

namespace fsm
{
	class IState;
	class IEvent;
	class IMachine;

	class IMachine
	{
	public:
		IMachine();
		virtual ~IMachine();

	public:
		virtual void	Transition(IState*);
		virtual void	Notify(IEvent*);
		virtual IState* GetCurState() const;
		virtual IState* GetPrevState() const;

	private:
		IState* m_currState;
		IState* m_prevState;
	};

	class IEvent
	{
	public:
		IEvent();
		virtual ~IEvent();

	public:
		virtual void Notify(IMachine*, IState*);
	};

	class IState
	{
	public:
		IState();
		virtual ~IState();

	public:
		virtual void OnEntry(IMachine*, IState*);
		virtual void OnExit(IMachine*, IState*);
		virtual void OnEvent(IMachine*, IEvent*);
		virtual void Transition(IMachine*, IState*);
	};
}

#endif