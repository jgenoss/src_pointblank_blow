#if !defined( __UI_SUBPHASE_BASE_H__)
#define __UI_SUBPHASE_BASE_H__

#include "UIPhaseBase.h"

class UISubPhaseBase : public UIPhaseBase
{
	I3_ABSTRACT_CLASS_DEFINE( UISubPhaseBase);

public:
	UISubPhaseBase();
	virtual ~UISubPhaseBase();
	virtual void	ProcessGameEvent( INT32 event, INT32 arg);
	
	/** \brief SubPhase는 자신이 Sliding을 업데이트합니다. */
	virtual void	OnUpdate( REAL32 rDeltaSeconds);

	
};

#endif
