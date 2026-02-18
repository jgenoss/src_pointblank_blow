#include "i3StateType.h"
#include "i3Base.h"
#include "i3StateMachine.h"
#include "i3StateGraph.h"

I3_CLASS_INSTANCE( i3StateMachine, i3ElementBase);

i3StateMachine::i3StateMachine(void)
{
	i3mem::FillZero( &m_Timer, sizeof( m_Timer));
	i3mem::FillZero( &m_Counter, sizeof( m_Counter));
}

i3StateMachine::~i3StateMachine(void)
{
}

void i3StateMachine::RunStateGraph( i3StateGraph * pGraph)
{
	i3State * pCurState, * pNewState;
	UINT32 i;
	INT32 highPriority = 0;
	i3Event * pEvent, * pSignaledEvent;

	I3ASSERT( pGraph != NULL);

	pCurState = pGraph->GetCurrentState();

	pNewState = NULL;
	pSignaledEvent = NULL;

	for( i = 0; i < pCurState->GetEventCount(); i++)
	{
		pEvent = pCurState->GetEvent( i );

		if( pEvent->IsSignaled())
		{
			// 조건이 충족된 Event가 있다면...

			if( pSignaledEvent == NULL)
			{
				// 처음 발견된 Event..
				highPriority = pEvent->GetPriority();
				pSignaledEvent = pEvent;
			}
			else
			{
				// 다른 Event 보다 우선 순위가 높은 Event인가?
				if( pEvent->GetPriority() > highPriority)
				{
					highPriority = pEvent->GetPriority();
					pSignaledEvent = pEvent;
				}
			}
		}
	}

	if( pSignaledEvent != NULL)
	{
		// Event의 발생으로 전이해야 하는 State를 구한다.
		pNewState = pSignaledEvent->GetToState();

		// 이전 State의 OnFinish()를 호출
		pCurState->OnFinish( this);

		// 새로운 State를 State Graph의 현재 State로 설정.
		pGraph->SetCurrentState( pNewState);

		// 새로운 State를 초기화 한다.
		pNewState->OnInit( this);

		pCurState = pNewState;
	}
	
	pCurState->OnRun( this);
}