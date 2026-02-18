#include "i3OptPCH.h"
#include "i3OptUtil.h"

#include <stdio.h>
#include <stdarg.h>	

static char szTemp[256];

static i3OptVLogCB s_i3VLogCB = nullptr;

I3_EXPORT_OPT
void i3SetOptVLogCB( i3OptVLogCB pFunc )
{
	s_i3VLogCB = pFunc;
}

I3_EXPORT_OPT
void i3OptVLog( const char *format, ...)
{
	va_list marker;

	va_start( marker, format);

	vsprintf( szTemp, format, marker);

	OutputDebugString( szTemp);

	if( s_i3VLogCB != nullptr )
	{
		s_i3VLogCB( szTemp );
	}
}


