#include "stdafx.h"
#include "UIToolFileMgr.h"

#include "i3Base/string/ext/extract_fileext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CUIToolFileMgr::CUIToolFileMgr()
{

}

CUIToolFileMgr::~CUIToolFileMgr()
{

}

bool CUIToolFileMgr::MoveToNatioFolder( const char * pszNation )
{
	const char * _i3VTex		= "i3Vtex";
	const char * _i3VTexSub		= "i3VtexSub";
	const char * _i3SubSet		= "i3SubSet";
	const char * _i3RefTex		= "i3RefTex";

 	char szPath[ MAX_PATH];
 	char szExt[ 16];

	char szDstFile[MAX_PATH];
	char szSrcFile[MAX_PATH];

	memset( szDstFile,0,sizeof(szDstFile) );
	memset( szSrcFile,0,sizeof(szSrcFile) );

	const char * pszWorkingFolder = i3ResourceFile::GetWorkingFolder();
	i3::snprintf( szPath, sizeof(szPath), "%s/UIRe/*.*", pszWorkingFolder );
 
 	HANDLE				hFind		= INVALID_HANDLE_VALUE;
 	WIN32_FIND_DATA		ffd;
 
 	hFind = ::FindFirstFile( szPath, &ffd);
 	if (hFind == INVALID_HANDLE_VALUE)
 		return false;

 	BOOL bFind = TRUE;
 	while( bFind)
 	{
 		bFind = ::FindNextFile( hFind, &ffd);
 	
		if( !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
 		{
	//		i3String::SplitFileExt( ffd.cFileName, szExt, sizeof( szExt));
			i3::extract_fileext(ffd.cFileName, szExt);

			if( i3::generic_is_iequal( szExt, _i3VTex) ||
				i3::generic_is_iequal( szExt, _i3VTexSub) ||
				i3::generic_is_iequal( szExt, _i3RefTex) ||
				i3::generic_is_iequal( szExt, _i3SubSet) )
			{
				i3::snprintf( szSrcFile, sizeof(szSrcFile), "%s/UIRe/%s",pszWorkingFolder ,ffd.cFileName );
				i3::snprintf( szDstFile, sizeof(szDstFile), "%s/Locale/%s/UIRe/%s",pszWorkingFolder, pszNation ,ffd.cFileName );

				::CopyFile( szSrcFile,szDstFile,FALSE );
			}
 		}
 	}
 	::FindClose(hFind);
	return true;
}

void CUIToolFileMgr::RemoveNationFolderFile( const char * pszNation)
{
	const char * _i3VTex		= "i3Vtex";
	const char * _i3VTexSub		= "i3VtexSub";
	const char * _i3SubSet		= "i3SubSet";
	const char * _i3RefTex		= "i3RefTex";

 	char szPath[ MAX_PATH];
 	char szExt[ 16];

	const char * pszWorkingFolder = i3ResourceFile::GetWorkingFolder();
 	i3::snprintf( szPath, sizeof(szPath), "%s/UIRe/%s/*.*", pszWorkingFolder, pszNation );
 
 	HANDLE				hFind		= INVALID_HANDLE_VALUE;
 	WIN32_FIND_DATA		ffd;
 
 	hFind = ::FindFirstFile( szPath, &ffd);
 	if (hFind == INVALID_HANDLE_VALUE)
 		return ;

 	BOOL bFind = TRUE;
 	while( bFind)
 	{
 		bFind = ::FindNextFile( hFind, &ffd);
 	
		if( !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
 		{
		//	i3String::SplitFileExt( ffd.cFileName, szExt, sizeof( szExt));
			i3::extract_fileext(ffd.cFileName, szExt);

			if( i3::generic_is_iequal( szExt, _i3VTex) ||
				i3::generic_is_iequal( szExt, _i3VTexSub) ||
				i3::generic_is_iequal( szExt, _i3RefTex) ||
				i3::generic_is_iequal( szExt, _i3SubSet) )
			{
				char szFile[MAX_PATH];
				i3::snprintf( szFile, MAX_PATH, "%s/UIRe/%s/%s", pszWorkingFolder, pszNation, ffd.cFileName );
				::DeleteFile(szFile);
			}
 		}
 	}
 	::FindClose(hFind);
}