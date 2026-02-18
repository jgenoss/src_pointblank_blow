#if !defined( __I3_EVENT_RECEIVER_H)
#define __I3_EVENT_RECEIVER_H

#include "i3NamedElement.h"

enum I3_EVT_CODE
{
	I3_EVT_CODE_ACTIVATE = 0,
	I3_EVT_CODE_DEACTIVATE
};

class I3_EXPORT_BASE i3EventReceiver : public i3NamedElement
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3EventReceiver, i3NamedElement);
public:

	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) { return true; }
	virtual void	OnInput(void) {}

#if defined( I3_WINDOWS)
	virtual void	OnMapCoord(POINT * pPoint) {}
	virtual void	OnLButtonDown(UINT32 nFlag, POINT point) {}
	virtual void	OnLButtonUp(UINT32 nFlag, POINT point) {}
	virtual void	OnLButtonDblClk(UINT32 nFlag, POINT point) {}
	virtual void	OnMButtonDown(UINT32 nFlag, POINT point) {}
	virtual void	OnMButtonUp(UINT32 nFlag, POINT point) {}
	virtual void	OnRButtonDown(UINT32 nFlag, POINT point) {}
	virtual void	OnRButtonUp(UINT32 nFlag, POINT point) {}

	virtual void	OnMouseMove(UINT32 nFlag, POINT point) {}
	virtual void	OnMouseWheel(UINT nFlag, short zDelta, POINT point) {}

	virtual	void	OnKeyDown(UINT32 nKey) {}
	virtual void	OnKeyUp(UINT32 nKey) {}
#endif
};




#endif
