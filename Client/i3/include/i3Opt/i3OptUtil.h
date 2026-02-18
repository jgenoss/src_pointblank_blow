#ifndef __I3_OPT_UTIL_H__
#define __I3_OPT_UTIL_H__


typedef void (*i3OptVLogCB)( char* szMsg );

I3_EXPORT_OPT	void i3SetOptVLogCB( i3OptVLogCB pFunc );
I3_EXPORT_OPT	void i3OptVLog( const char *format, ...);

#endif // __I3_OPT_UTIL_H__
