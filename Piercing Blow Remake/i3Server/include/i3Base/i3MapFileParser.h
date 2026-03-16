
#if !defined( __I3_MAPFILEPARSER_H)
#define __I3_MAPFILEPARSER_H

#include "i3CommonType.h"

#if defined (I3_WINDOWS)
class I3_EXPORT_BASE i3MapFileParser
{
	INT32			m_nSourceLine;
	char			m_szFunctionName[1024];
	char			m_szClassName[1024];

public:
	i3MapFileParser()	{};
	~i3MapFileParser()	{};

	BOOL			CheckExpection( const char *pszMapFile, const char* pszExceptionAddress);
	BOOL			CheckExpection( const char *pszMapFile, DWORD dwAddress);
	BOOL			CheckExpection( const char *pszMapFile, PVOID pAddress);
	BOOL			SetCheckExpection( const char *pszMapFile, DWORD dwExceptionAddress );
	
	BOOL			GetStartAddress( FILE *fp, DWORD* pdwStartAddress );
	
	BOOL			GetAddressInfo( FILE *fp, DWORD dwExceptionAddress, char *pszFunctionName, char* pszAddress, char *pszClassName );
	BOOL			GetAddressInfoReal(FILE *fp, DWORD dwExceptionAddress, char *pszFunctionName, char* pszAddress, char *pszClassName );
	
	BOOL			GetLineInfo(FILE *fp, DWORD dwAddress, const char* pObeName, const char* pszAddress, INT32* pnLine, char* pszClassName);
	void			GetLineClassName( const char* szLine, char* pszClassName );
	BOOL			GetLineReal(FILE *fp, DWORD dwAddress, const char* pszAddress, INT32* pnLine );
	

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