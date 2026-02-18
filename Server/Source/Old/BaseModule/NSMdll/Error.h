#ifndef __ERROR_H__
#define __ERROR_H__

#include "DbgHelp.h"

class NSM_cExceptionHandler
{
	MINIDUMP_TYPE	m_eDumpType;
	char			m_szDmpFileName[ MAX_PATH ];

public:
	NSM_cExceptionHandler();
	virtual ~NSM_cExceptionHandler();

	void			SetPath( char* pszDmpFileName );
	void			SetExceptionHandlerOption( MINIDUMP_TYPE eOption );
	void			SetCreateMiniDump( PEXCEPTION_POINTERS pExceptionInfo );
};

namespace NSM_nsError
{
	EXPORT_BASE void	SetExceptionHandler( char* pszDmpFileName );
	EXPORT_BASE void	SetExceptionHandlerOption( MINIDUMP_TYPE eOption );
}

#endif