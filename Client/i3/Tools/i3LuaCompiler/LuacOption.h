#pragma once


#include "vector"
#include "string"

using namespace std;


enum OPTION_TYPE	{ OT_LOG = 0, OT_DUMP, OT_DEBUGINFO, OT_PRINTLOADERROR, OT_PRINTSRCERROR, 
					OT_EXCLUDEFILE, OT_COUNT };
enum LOG_TYPE		{ LT_NEW = 0, LT_ATTACH, LT_NONE };
enum DEBUGINFO_TYPE	{ DT_INCLUDE = 0, DT_EXCLUDE };

class CLuacOption
{
private:
	LOG_TYPE			m_LogType;
	bool				m_bDump;
	DEBUGINFO_TYPE		m_DebugInfoType;
	bool				m_bPrintLoadError;
	bool				m_bPrintSrcError;
	bool				m_bExcludeFile;

	vector<string>		m_vecstrExFileList;

public:
	CLuacOption(void);
	~CLuacOption(void);

private:
	void	_setData( LOG_TYPE logtype, bool bDump, DEBUGINFO_TYPE m_DebugInfoType, 
					bool bPrintLoadError, bool bPrintSrcError, bool bExcludeFile );

public:
	LOG_TYPE		getLogType(void) const			{ return m_LogType;			}
	DEBUGINFO_TYPE	getDebugInfoType(void) const	{ return m_DebugInfoType;	}
	bool			IsDump(void) const				{ return m_bDump;			}
	bool			IsPrintLoadError(void) const	{ return m_bPrintLoadError;	}
	bool			IsPrintSrcError(void) const		{ return m_bPrintSrcError;	}
	bool			IsExcludeFile(void) const		{ return m_bExcludeFile;	}

	bool			parseOption( char* szOption );
	bool			setExcludeFileList( char* szFileList );
	bool			compareExcludeFile( const char* szFileName );
};
