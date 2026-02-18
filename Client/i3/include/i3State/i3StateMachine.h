#if !defined( __I3_STATE_MACHINE_H)
#define __I3_STATE_MACHINE_H

#include "i3Base.h"
#include "i3State.h"
#include "i3StateGraph.h"

#define		I3_STATE_TIMER_MAX		10
#define		I3_STATE_COUNTER_MAX	128

class i3StateMachine : public i3ElementBase
{
	I3_CLASS_DEFINE( i3StateMachine);

protected:
	INT64			m_Timer[I3_STATE_TIMER_MAX];
	INT32			m_Counter[ I3_STATE_COUNTER_MAX];

public:
	i3StateMachine(void);
	virtual ~i3StateMachine(void);

	void			ResetTimer( INT32 idx)
	{
		m_Timer[idx] = 0;
	}

	INT64			GetTimer( INT32 idx)
	{
		return m_Timer[idx];
	}

	void			SetCounter( INT32 idx)
	{
		m_Counter[idx] = 0;
	}

	INT32			GetCounter( INT32 idx)
	{
		return m_Counter[idx];
	}

	void			AddCounter( INT32 idx, INT32 val)
	{
		m_Counter[idx] += val;
	}

	void			SubCounter( INT32 idx, INT32 val)
	{
		m_Counter[idx] -= val;
	}

	void			MulCounter( INT32 idx, INT32 val)
	{
		m_Counter[idx] *= val;
	}

	void			DivCounter( INT32 idx, INT32 val)
	{
		m_Counter[idx] /= val;
	}

	void RunStateGraph( i3StateGraph * pStartState);
};

#endif