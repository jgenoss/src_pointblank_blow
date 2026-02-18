#include "StdAfx.h"
#include ".\FileUtil.h"
#include ".\filefinder.h"

BOOL FindFiles( LPCTSTR szRootPath, LPCTSTR szExt, FF_CALLBACK CallBack, void *pCallBackArg, BOOL bSearchSubDir )
{
	HANDLE	hFind;
	WIN32_FIND_DATA FindInfo;
	char szPath[MAX_PATH];

	if( szRootPath && szRootPath[0] )
	{
		sprintf( szPath, "%s\\*.*", szRootPath );
	}
	else
	{
		strcpy( szPath, "*.*" );
	}

	hFind = FindFirstFile( szPath, &FindInfo );
	if( hFind == INVALID_HANDLE_VALUE )
	{
		// ASSERT(0);
		return FALSE;
	}

	do
	{
		if( FindInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if
			(
				bSearchSubDir &&
				(strcmp( FindInfo.cFileName, ".") != 0) &&
				(strcmp( FindInfo.cFileName, "..") != 0)
			)
			{
				if( szRootPath && szRootPath[0] )
				{
					sprintf( szPath, "%s\\%s", szRootPath, FindInfo.cFileName );
				}
				else
				{
					strcpy( szPath, FindInfo.cFileName );
				}

				if( !FindFiles( szPath, szExt, CallBack, pCallBackArg, bSearchSubDir ) )
				{
					return FALSE;
				}
			}
		}
		else
		{
			char ext[MAX_PATH];
			GetFileExt( ext, FindInfo.cFileName );

			if( stricmp( szExt, ext ) == 0)
			{
				if( !(*CallBack)( szRootPath, FindInfo.cFileName, pCallBackArg ) )
				{
					goto error;
				}
			}
		}

		
	} while( FindNextFile( hFind, &FindInfo ) );

	FindClose( hFind );
	return TRUE;

error:
	FindClose( hFind );
	return FALSE;
}

