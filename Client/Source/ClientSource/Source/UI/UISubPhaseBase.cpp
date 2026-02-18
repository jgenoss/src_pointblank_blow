#include "pch.h"
#include "UISubPhaseBase.h"

I3_ABSTRACT_CLASS_INSTANCE( UISubPhaseBase, UIPhaseBase);

UISubPhaseBase::UISubPhaseBase()
{
}

UISubPhaseBase::~UISubPhaseBase()
{
}

/*virtual*/ void UISubPhaseBase::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate( rDeltaSeconds);

	/*if( m_SlideState == SS_SLIDING_IN ||
		m_SlideState == SS_SLIDING_OUT)
	{
		OnSliding( rDeltaSeconds);
	}*/
}

void	UISubPhaseBase::ProcessGameEvent( INT32 event, INT32 arg)
{
	
}