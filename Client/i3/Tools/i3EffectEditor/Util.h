#if !defined( __UTIL_H)
#define __UTIL_H

#include "i3Base.h"

void	REAL2STR( REAL32 val, char * pszStr);

i3KeyframeTable *	CreateKeyframeTable(INT32 count);
i3KeyframeTable *	CreateColorKeyframeTable( INT32 count);

#endif