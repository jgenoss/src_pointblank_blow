
#if !defined( __I3_MAPFILEPARSER_H)
#define __I3_MAPFILEPARSER_H

#include "i3CommonType.h"

#if defined (I3_WINDOWS)
class I3_EXPORT_BASE i3MapFileParser
{
	INT32			m_nSourceLine = 0;
	char			m_szFunctionName[1024] = { 0 };
	char			m_szClassName[1024] = { 0 };

public:

	bool			CheckExpection( const char *pszMapFile, const char* pszExceptionAddress);
	bool			CheckExpection( const char *pszMapFile, DWORD dwAddress);
	bool			CheckExpection( const char *pszMapFile, PVOID pAddress);
	bool			SetCheckExpection( const char *pszMapFile, DWORD dwExceptionAddress );
	
	bool			GetStartAddress( FILE *fp, DWORD* pdwStartAddress );
	
	bool			GetAddressInfo( FILE *fp, DWORD dwExceptionAddress, char *pszFunctionName, char* pszAddress, char *pszClassName );
	bool			GetAddressInfoReal(FILE *fp, DWORD dwExceptionAddress, char *pszFunctionName, char* pszAddress, char *pszClassName );
	
	bool			GetLineInfo(FILE *fp, DWORD dwAddress, const char* pObeName, const char* pszAddress, INT32* pnLine, char* pszClassName);
	void			GetLineClassName( const char* szLine, char* pszClassName );
	bool			GetLineReal(FILE *fp, DWORD dwAddress, const char* pszAddress, INT32* pnLine );
	

	void			GetFunctionName( char *pszLine, char *pszFunctionName );
	void			GetObjectName( char* pszPreLineSave, char* pszClassName );

	DWORD			GetHexValue( const char *pszHex );
	INT32			GetHexConvert( const char cHex );
	INT32			GetHexMult( INT32 nMult );


	INT32			GetSourceLine()							{	return m_nSourceLine;		};
	char*			GetFunctionName()						{	return m_szFunctionName;	};
	char*			GetClassName()							{	return m_szClassName;		};
};
#endif
#endif