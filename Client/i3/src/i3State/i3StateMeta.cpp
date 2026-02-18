#include "i3StateType.h"
#include "i3State.h"
#include "i3Event.h"
#include "i3EventTouch.h"
#include "i3EventTime.h"
#include "i3EventCounter.h"
#include "i3EventInput.h"
#include "i3EventIO.h"
#include "i3EventState.h"
#include "i3State.h"
#include "i3StateInit.h"
#include "i3StateFinish.h"
#include "i3StateGraph.h"
#include "i3StateMachine.h"

static BOOL		s_bStateMetaRegistered = FALSE;

void i3StateRegisterMetas(void)
{
	if( s_bStateMetaRegistered == FALSE)
	{
		i3Event::RegisterMeta();
		i3EventTouch::RegisterMeta();
		i3EventTime::RegisterMeta();
		i3EventCounter::RegisterMeta();
		i3EventInput::RegisterMeta();
		i3EventIO::RegisterMeta();
		i3EventState::RegisterMeta();
		i3State::RegisterMeta();
		i3StateInit::RegisterMeta();
		i3StateFinish::RegisterMeta();
		i3StateGraph::RegisterMeta();
		i3StateMachine::RegisterMeta();

		s_bStateMetaRegistered = TRUE;
	}
}
