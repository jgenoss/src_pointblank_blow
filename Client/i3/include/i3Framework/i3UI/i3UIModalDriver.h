#if !defined( __I3UI_MODAL_DRIVER_H)
#define __I3UI_MODAL_DRIVER_H

class i3UIFrameWnd;

class I3_EXPORT_FRAMEWORK i3UIModalDriver : public i3EventReceiver
{
	I3_EXPORT_CLASS_DEFINE( i3UIModalDriver, i3EventReceiver);

public:
	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
};

#endif
