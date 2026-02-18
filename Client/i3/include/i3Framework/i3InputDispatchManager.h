#if !defined( __I3_INPUT_DISPATCH_MANAGER_H)
#define __I3_INPUT_DISPATCH_MANAGER_H

#include "i3GameObjBase.h"

class i3Framework;

class I3_EXPORT_FRAMEWORK i3InputDispatchManager : public i3GameObjBase
{
	I3_EXPORT_CLASS_DEFINE( i3InputDispatchManager, i3GameObjBase);

public:
	virtual void		OnChangeFocus(i3Framework * pFramework, i3EventReceiver * pFrom, i3EventReceiver * pTo) {}

	virtual bool		DispatchLButtonDown(UINT32 nFlag, POINT point) { return true; }
	virtual bool		DispatchLButtonUp( UINT32 nFlag, POINT point) { return true; }
	virtual bool		DispatchLButtonDblClk( UINT32 nFlag, POINT point) { return true; }
	virtual bool		DispatchMButtonDown( UINT32 nFlag, POINT point) { return true; }
	virtual bool		DispatchMButtonUp( UINT32 nFlag, POINT point) { return true; }
	virtual bool		DispatchRButtonDown( UINT32 nFlag, POINT point) { return true; }
	virtual bool		DispatchRButtonUp( UINT32 nFlag, POINT point) { return true; }
	virtual bool		DispatchMouseMove( UINT32 nFlag, POINT point) { return true; }
	virtual bool		DispatchMouseWheel( UINT nFlag, short zDelta, POINT point) { return true; }
	virtual bool		DispatchKeyDown( UINT32 nKey) { return true; }
	virtual bool		DispatchKeyUp( UINT32 nKey) { return true; }

	virtual bool		DispatchActivate( bool bActivate) { return true; }
	virtual bool		DispatchSetFocus(void) { return true; }
	virtual bool		DispatchKillFocus(void) { return true; }
	virtual bool		DispatchInput(void) { return true; }
};

#endif
