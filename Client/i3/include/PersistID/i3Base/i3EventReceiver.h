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
	I3_ABSTRACT_CLASS_DEFINE( i3EventReceiver);
public:

	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
};

#endif
