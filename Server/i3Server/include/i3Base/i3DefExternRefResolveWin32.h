#if !defined( __I3_DEF_EXTERNREF_RESOLVE_WIN32_H)
#define __I3_DEF_EXTERNREF_RESOLVE_WIN32_H

#if defined( I3_WINDOWS)
	I3_EXPORT_BASE	bool _DefExternResolveProcWin32( const char * pszPath, char * pszNewPath, INT32 lenBuff, const char * pszWorkPath, bool bSearch);
#endif

#endif
