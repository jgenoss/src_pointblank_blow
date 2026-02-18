//#include <errno.h>
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define luac_c
//#define LUA_CORE

#include "i3LuaPCH.h"
#include "i3Lua.h"

#pragma warning(push)
#pragma warning(disable : 4244)
#include "./luabind/luabind.hpp"
#pragma warning(pop)

#include "lua_typed_enums.h"

#include "LuacOption.h"


#define PROGNAME			"luac"		/* default program name */
#define	LC_LOG_FILENAME		"Luac.log"

enum ARGS_TYPE		{ AT_PROGNAME, AT_OPTION, AT_INFILTER, AT_OUTEXT, 
					AT_EXCLUDEFILELIST, AT_COUNT };



static FILE*	g_fLog		= NULL;
/*
static LOG_TYPE	g_LogType			= LT_NEW;
static bool		g_bDump				= true;
static int		g_nDebugInfo		= 1;		// DubugInfo - 0: include, 1: exclude
static bool		g_bPrintLoadError	= true;
static bool		g_bPrintSrcError	= false;
*/

static CLuacOption		g_LuacOption;

// 출력 함수들
static void PrintError( const char* pszMessage )
{
	fprintf(stderr, "ERROR: %s\n", pszMessage );

	if( g_fLog ) fprintf( g_fLog, "ERROR: %s\n", pszMessage );
}

static void PrintError( const char* pszMessage1, const char* pszMessage2 )
{
	fprintf(stderr, "ERROR: %s - %s\n", pszMessage1, pszMessage2);

	if( g_fLog ) fprintf( g_fLog, "ERROR: %s - %s\n", pszMessage1, pszMessage2 );
}


static void PrintMessage( const char* pszMessage )
{
	fprintf( stderr, "%s\n", pszMessage );

	if( g_fLog ) fprintf( g_fLog, "%s\n", pszMessage );
}

static void PrintMessage( const char* pszMessage1, const char* pszMessage2 )
{
	fprintf( stderr, "%s %s\n", pszMessage1, pszMessage2 );

	if( g_fLog ) fprintf( g_fLog, "%s %s\n", pszMessage1, pszMessage2 );
}


static void PrintLog( const char* pszMessage )
{
	if( g_fLog ) fprintf( g_fLog, "%s\n", pszMessage );
}


#define toproto(L,i) (clvalue(L->top+(i))->l.p)

static const Proto* combine(lua_State* L, int n)
{
	if (n==1)
		return toproto(L,-1);
	else
	{
		int i,pc;
		Proto* f	= i3Lua::NewProto(L);

		setptvalue2s(L,L->top,f); 
		i3Lua::IncreaseTop(L);

		f->source		= i3Lua::NewLiteral(L,"=(" PROGNAME ")");
		f->maxstacksize	= 1;
		pc				= 2 * n + 1;
		f->code			= (Instruction*)i3Lua::NewVector( L, pc, sizeof(Instruction) );
		f->sizecode		= pc;
		f->p			= (Proto**)i3Lua::NewVector( L, n, sizeof(Proto*) );
		f->sizep		= n;
		pc				= 0;

		for (i=0; i<n; i++)
		{
			f->p[i]			= toproto(L,i-n-1);
			f->code[pc++]	= CREATE_ABx(OP_CLOSURE,0,i);
			f->code[pc++]	= CREATE_ABC(OP_CALL,0,1,1);
		}

		f->code[pc++]	= CREATE_ABC(OP_RETURN,0,1,0);

		return f;
	}
}


// 루아에서 덤프할때 파일에 데이터를 저장할때 함수
static int writer(lua_State* L, const void* p, size_t size, void* u)
{
	UNUSED(L);
	return (fwrite(p,size,1,(FILE*)u)!=1) && (size!=0);
}


struct Smain {
	int		argc = 0;
	char**	argv = nullptr;
};


// 경로에서 해당하는 파일 돌아가면서 덤프
static int pmain(lua_State* L )
{
	struct Smain*	s			= (struct Smain*)lua_touserdata(L, 1);
	int				argc		= s->argc;
	char**			argv		= s->argv;
	char*			pszPathFilter	= argv[0];
	char*			pszOutExt		= argv[1];

	char			szDrive[ _MAX_DRIVE ];
	char			szDir[ _MAX_DIR ];
	char			szFileName[ _MAX_FNAME ];
	char			szExt[ _MAX_EXT ];

	char			szInPath[ MAX_PATH ];
	char			szInFilePath[ MAX_PATH ];
	char			szOutFilePath[ MAX_PATH ];

	char			szTemp[256];

	_finddatai64_t	c_file;
	intptr_t		hFile;

	const Proto*	pProto;

	// 경로에서 파일들을 얻어옴
	hFile	= _findfirsti64( pszPathFilter, &c_file);

	if( hFile == -1L ) 
	{
		switch (errno) 
		{
			case ENOENT:
				PrintError("File Not Exist"); 
				break;
			case EINVAL:
				PrintError("Invalid path name."); 
				break;
			case ENOMEM:
				PrintError("Not enough memory or file name too long.");
				break;
			default:
				PrintError("Unknown error.");
				break;
		}

		return 0;
	} // end if

	if ( !lua_checkstack(L, argc) ) 
	{
		PrintError( "Too many input files" );
		return 0;
	}


	_splitpath( pszPathFilter, szDrive, szDir, NULL, NULL );
	_makepath( szInPath, szDrive, szDir, NULL, NULL );

	// 해당 파일들을 전부 덤프
	do 
	{
		// 파일 로드
		sprintf_s( szInFilePath, "%s%s", szInPath, c_file.name );

		if( c_file.attrib & _A_SUBDIR ) continue;				// 폴더는 스킵

		if( g_LuacOption.compareExcludeFile( c_file.name ) )	// 제외할 파일
		{
			if( g_LuacOption.IsDump() || g_LuacOption.IsPrintSrcError() ||  g_LuacOption.IsPrintLoadError() )
			{
				PrintMessage( "> SKIP", szInFilePath );
			}

			continue;
		}

		int	ret	= i3Lua::LoadFile( L, szInFilePath );

		if ( ret == 0)
		{
			if( g_LuacOption.IsDump() || g_LuacOption.IsPrintSrcError() )
			{
				PrintMessage( ">", szInFilePath );
			}

			// 덤프
			bool	bDump	= true;

			if( g_LuacOption.IsDump() )
			{
				_splitpath( szInFilePath, szDrive, szDir, szFileName, szExt );
				_makepath( szOutFilePath, szDrive, szDir, szFileName, pszOutExt );

				FILE*	f = fopen( szOutFilePath,"wb" );

				if( f == NULL )
				{
					PrintError( "File Open", szOutFilePath );
					continue;
				}

				pProto	= combine( L, 1 );

				i3Lua::Dump( L, pProto, writer, f, g_LuacOption.getDebugInfoType() );

				if(ferror(f))	
				{
					PrintError( "File Write", szOutFilePath );
					bDump	= false;
				}
				if(fclose(f)) PrintError( "File Close", szOutFilePath );
			}
			else
			{
				bDump	= false;
			}

			if( g_LuacOption.IsPrintSrcError() )
			{
				int		nRet = i3Lua::PCall( L, 0, LUA_MULTRET, 0);
				if( nRet != 0 )
				{
					memset( szTemp , 0, 256 );
					sprintf_s( szTemp, "ret: %d", nRet );

					PrintError( szTemp, i3Lua::GetErrorString( L) );
				}
			}

			if( bDump ) PrintMessage( "Complete Dump -", szOutFilePath );
		}
		else if( g_LuacOption.IsPrintLoadError() )
		{
			PrintMessage( "\n>", szInFilePath );

			memset( szTemp , 0, 256 );
			sprintf_s( szTemp, "ret: %d - %s\n", ret, i3Lua::GetErrorString( L) );

			PrintError( szTemp );
		}

	} while( _findnexti64(hFile, &c_file) == 0 );

	_findclose(hFile); // _findfirsti64(), _findnexti64()에 사용된 메모리를 반환

	return 0;
}


int main(int argc, char* argv[])
{
	if( argc != AT_COUNT			// 제외파일 리스트 없을때
		&& argc != AT_COUNT - 1 )	// 제외파일 리스트 있을때
	{
		PrintError( "arg Count" );
		return EXIT_SUCCESS;
	}

	if( g_LuacOption.parseOption( argv[ AT_OPTION ] ) == false )
	{
		PrintError( "arg - Option" );
		return EXIT_SUCCESS;
	}

	if( argc == AT_COUNT )
	{
		if( !g_LuacOption.IsExcludeFile() )
		{
			PrintError( "arg - Exclude File List" );
			return EXIT_SUCCESS;
		}

		g_LuacOption.setExcludeFileList( argv[ AT_EXCLUDEFILELIST] );
	}

	argc--; argv++;
	argc--; argv++;

	LOG_TYPE	logtype	= g_LuacOption.getLogType();

	if( logtype != LT_NONE )
	{
		if( logtype == LT_NEW )	g_fLog	= fopen( LC_LOG_FILENAME, "w" );
		if( logtype == LT_ATTACH)	g_fLog	= fopen( LC_LOG_FILENAME, "a" );

		if( g_fLog == NULL )
		{
			PrintError( "Log File Open" );
			return EXIT_SUCCESS;
		}

		// 날짜 출력
		if( logtype == LT_ATTACH )
		{
			PrintLog( "\n" );
		}

		char		szTemp[256];
		struct tm*	now;
		time_t		current_time;
 
		time(&current_time);
		now		= localtime(&current_time);

		sprintf_s( szTemp, "[%d%d%d:%d:%d:%d] %s\n", 
			now->tm_year + 1900, now->tm_mon, now->tm_mday, 
			now->tm_hour, now->tm_min, now->tm_sec, 
			argv[0] );

		PrintMessage( szTemp );
	}

	lua_State*		L;
	struct Smain	s;

	L		= i3Lua::Create( 0 );
	if( L == NULL)
	{
		PrintError( "Not enough memory for state" );
		fclose( g_fLog );

		return EXIT_SUCCESS;
	}

	s.argc	= argc;
	s.argv	= argv;

	if( lua_cpcall( L, pmain, &s ) != 0 )
	{
		PrintError( lua_tostring(L,-1) );
		fclose( g_fLog );

		return EXIT_SUCCESS;
	}

	i3Lua::Close(L);
	if( g_fLog != NULL)
		fclose( g_fLog );

	return EXIT_SUCCESS;
}