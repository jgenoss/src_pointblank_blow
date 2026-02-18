#include "stdafx.h"
#include "imagechecker.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3UI/i3VTFileFormatUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CImageChecker::CImageChecker()
{
	_Init();
}

CImageChecker::~CImageChecker()
{

}

void CImageChecker::_Init()
{
	m_bSetting = false;
	m_mapDefImage.clear();
	m_conErrorFile.clear();
}

bool CImageChecker::IsEnableCheck()
{
	return m_bSetting;
}
bool CImageChecker::LoadDefaultImageInfo()
{
	char szFolder[MAX_PATH];
	i3::snprintf(szFolder, MAX_PATH, "%s/Image", i3ResourceFile::GetWorkingFolder() );
	m_mapDefImage.clear();
	m_bSetting = LoadDefaultImageInfo( szFolder );
	return m_bSetting;
}

bool CImageChecker::LoadDefaultImageInfo( const char * pszFolder  )
{
	char szPath[MAX_PATH];
	i3::snprintf( szPath, MAX_PATH, "%s/*.*", pszFolder );

	HANDLE				hFind		= INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		ffd;
	hFind = ::FindFirstFile( szPath, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	BOOL bFind = TRUE;
	char szExt[16];
 	while( bFind)
 	{
 		bFind = ::FindNextFile( hFind, &ffd);
		if( i3::generic_is_iequal( ffd.cFileName, "..") || 
			i3::generic_is_iequal( ffd.cFileName, ".") )
			continue;
 	
		if( !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
 		{
		//	i3String::SplitFileExt( ffd.cFileName, szExt, sizeof( szExt));
			i3::extract_fileext(ffd.cFileName, szExt);

			if( i3::generic_is_iequal( szExt, "tga") )
			{
				char szImage[MAX_PATH];
				i3::snprintf( szImage, MAX_PATH, "%s/%s", pszFolder, ffd.cFileName );
				i3::make_unix_path( szImage );


				if(!i3System::IsFile( szImage ))
					continue;
				i3Texture * pTex = NULL;
				i3ImageFile file;
				pTex  = file.Load( szImage );
				if( !pTex )
				{
					continue;
				}
				IMG_INFO info;
				info.nWidth = pTex->GetWidth();
				info.nHeight = pTex->GetHeight();
				i3::safe_string_copy( info.szPath , szImage, sizeof( info.szPath ) );
				m_mapDefImage.insert( std::make_pair(ffd.cFileName, info) );
				pTex->Release();
			}
 		}
		else if( (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			char szFolder[MAX_PATH];
			i3::snprintf(szFolder, MAX_PATH, "%s/%s", pszFolder, ffd.cFileName );
			LoadDefaultImageInfo( szFolder );
		}
 	}
 	::FindClose(hFind);
	return true;
}

bool CImageChecker::CheckNationImageInfo( const char * pszNation )
{
	char szFolder[MAX_PATH];
	i3::snprintf(szFolder, MAX_PATH, "%s/Locale/%s/UIRe/Image", i3ResourceFile::GetWorkingFolder(),pszNation );
	m_conErrorFile.clear();
	LoadNationImageInfo( szFolder );

	bool bRet = true;
	if( !m_conErrorFile.empty() )
	{
		char szTemp[4096] = {"경고 : 기본 이미지 폴더의 이미지크기와 다릅니다.\n"};
		for( std::vector< std::string >::iterator iter = m_conErrorFile.begin(); iter != m_conErrorFile.end(); ++iter )
		{
			i3::snprintf( szTemp, sizeof( szTemp ), "%s - %s \n", szTemp, iter->c_str() );
		}
		::AfxMessageBox( szTemp );
		bRet = false;
	}
	return bRet;
}
bool CImageChecker::LoadNationImageInfo( const char * pszFolder )
{
	char szPath[MAX_PATH];
	i3::snprintf( szPath, MAX_PATH, "%s/*.*", pszFolder );

	HANDLE				hFind		= INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		ffd;
	hFind = ::FindFirstFile( szPath, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	BOOL bFind = TRUE;
	char szExt[16];
 	while( bFind)
 	{
 		bFind = ::FindNextFile( hFind, &ffd);
		if( i3::generic_is_iequal( ffd.cFileName, "..") || 
			i3::generic_is_iequal( ffd.cFileName, ".") )
			continue;
 	
		if( !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
 		{
		//	i3String::SplitFileExt( ffd.cFileName, szExt, sizeof( szExt));
			i3::extract_fileext(ffd.cFileName, szExt);

			if( i3::generic_is_iequal( szExt, "tga") )
			{
				char szImage[MAX_PATH];
				i3::snprintf( szImage, MAX_PATH, "%s/%s", pszFolder, ffd.cFileName );
				i3::make_unix_path( szImage );


				if(!i3System::IsFile( szImage ))
					continue;
				i3Texture * pTex = NULL;
				i3ImageFile file;
				pTex  = file.Load( szImage );
				if( !pTex )
				{
					continue;
				}
				std::map< std::string, IMG_INFO >::iterator iter = m_mapDefImage.find( ffd.cFileName );
				if( iter != m_mapDefImage.end() )
				{
					if( iter->second.nWidth != pTex->GetWidth() ||
						iter->second.nHeight != pTex->GetHeight() ) 
					{
						char szFull[MAX_PATH];
						i3::snprintf( szFull, MAX_PATH,  "%s/%s", pszFolder, ffd.cFileName );
						i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), szFull, szFull  );
						i3::make_unix_path( szFull );

						m_conErrorFile.push_back( szFull );
					}
				}
				pTex->Release();
			}
 		}
		else if( (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			char szFolder[MAX_PATH];
			i3::snprintf(szFolder, MAX_PATH, "%s/%s", pszFolder, ffd.cFileName );
			LoadNationImageInfo( szFolder );
		}
 	}
 	::FindClose(hFind);
	return true;
}

bool CImageChecker::GetNationFromVirtualTexture( char * pszNation )
{
	if( NULL == pszNation )
		return false;

	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	if( pVTex )
	{
		INT32 nCount = pVTex->getRefTextureCount();
		for( INT32 nIDX = 0; nIDX < nCount; ++nIDX )
		{
			I3VT_TEXINFO * pInfo = pVTex->getRefTexture( nIDX );
			if( pInfo )
			{
				char szNation[MAX_PATH];
				i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), pInfo->m_strName, szNation );
				i3::make_unix_path( szNation );

				vt_util::GetVariousUIFileName( pszNation ,szNation, 1 );
			}

			if( i3::generic_string_size(pszNation) > 0 )
				return true;
		}
	}
	return false;
}
