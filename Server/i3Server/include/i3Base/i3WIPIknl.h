#ifndef __I3WIPIKNL_H
#define	__I3WIPIKNL_H

#include "WIPIHeader.h"

namespace I3WIPIKNL
{

	inline M_Int32 SetTimer(MCTimer *tm, M_Int64 timeout, void* parm)
	{
#ifdef I3_WIPI_SKT
		return OEMC_knlSetTimerEx( tm, timeout, NULL, parm);
#else
		return MC_knlSetTimer( tm, timeout, parm);
#endif
	}

	inline M_Int32 GetSysProperty(char * id, char * rtnBuf, int bufSize)
	{
#ifdef I3_WIPI_SKT
		return OEMC_knlGetSystemState( id, rtnBuf, bufSize);
#else
		return MC_knlGetSystemProperty( id, rtnBuf, bufSize);
#endif
	}
}

#endif	// __I3WIPIKNL_H
