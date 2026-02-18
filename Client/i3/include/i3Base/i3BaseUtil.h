#ifndef __I3_BASE_UTIL_H__
#define __I3_BASE_UTIL_H__

#include "i3ElementBase.h"


class I3_EXPORT_BASE i3ToolProgressCtrl : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3ToolProgressCtrl, i3ElementBase );
public:
	virtual void SetRange(INT32 nRange) {}
	virtual void SetPos(INT32 nPos) {}
};

#endif

