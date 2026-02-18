#include "stdafx.h"
#include "i3LevelResScanThread.h"
#include "i3LevelRes.h"
#include "i3LevelResAnimation.h"
#include "i3LevelResChara.h"
#include "i3LevelResSceneGraph.h"
#include "i3LevelResSound.h"
#include "i3LevelResTexture.h"
#include "i3LevelResDatabase.h"
#include "i3LevelResObject.h"
#include "i3Base/pool/create_by_common_pool.h"


#include <cctype>

I3_CLASS_INSTANCE( i3LevelResScanThread);


// BOOL (*FINDFILEPROC)( int Level, LPCTSTR Path, WIN32_FIND_DATA *FileInfo, LPVOID UserData);

//BOOL i3LevelResScanThread::_ResScanProc( int Level, LPCTSTR Path, WIN32_FIND_DATA * pInfo, LPVOID pUserData)
//{
//	i3::vector<i3::rc_string>&	List = *static_cast<i3::vector<i3::rc_string>*>(pUserData);
//	bool bIgnore;
//	
//	bIgnore =  (pInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
//
//	if( i3::contain_string( Path, ".svn") != -1)
//		return FALSE;
//
//	if( bIgnore == false)
//	{
//		char conv[MAX_PATH];
//
//		sprintf( conv, "%s/%s", Path, pInfo->cFileName);
//
//		List.push_back( conv);
//	}
//	
//	return TRUE;
//}

UINT32 i3LevelResScanThread::OnRunning( void * pUserData)
{
	
	::PostMessage( m_hwnd, WM_RESSCAN_START, 0, 0);

//	m_pFileList = new i3::vector<i3::rc_string>();				// 여기서의 new가 조금 위험할수도 있다..vcrt사용방식에 무관하려면..
	m_pFileList = i3::create_by_common_pool(i3::identity< i3::vector<i3::rc_string> >() );	
																// 메모리생성이 i3Base.dll의 생성에 집중되어야한다..
																// 일단, TDK와 레벨에디터 모두 vcrt.dll을 쓰므로 예상으로는 문제는 없을 것으로 보인다.
																// i3ElementBase::new_object()와 약간 유사한 것을 고려해야될것 같다.
																// 참고로 가변매크로지원도 생각을 해봐야될것 같다..

	m_pDB->setUpdateEnableState( false);
	//i3System::LookForFiles( m_szResPath, "*.*", (FINDFILEPROC) _ResScanProc, m_pFileList, FFT_SUBDIR);
	_ScanResourceAll();
	m_pDB->setUpdateEnableState( true);

	::PostMessage( m_hwnd, WM_RESSCAN_CONFIG, (WPARAM) INT32(m_pFileList->size()), 0);

	for(size_t i = 0; i < m_pFileList->size(); i++)
	{
		::PostMessage( m_hwnd, WM_RESSCAN_RES, (WPARAM) (*m_pFileList)[i].c_str(), 0);

		::SleepEx( 10, FALSE);
	}

	::SleepEx( 100, FALSE);
	::PostMessage( m_hwnd, WM_RESSCAN_END, (WPARAM) m_pFileList, 0);			// 레벨에디터에서 m_pFileList를 지연삭제처리하도록 되있다...
																				// 나중에 그쪽 릴리즈처리를 신경써줄것...(2012.05.20.수빈)
	return 0;
}

void	i3LevelResScanThread::_ScanResourceAll()
{
	TCHAR szCurDir[ MAX_PATH ];
	::GetCurrentDirectory( MAX_PATH, szCurDir );

	// scan res
	{
		_ScanResource(  m_szResPath, "", true );
	}

	::SetCurrentDirectory( szCurDir );
}

void	i3LevelResScanThread::_ScanResource( const i3::string& rootPath, const i3::string& parentPath, bool bRecursive )
{
	if( true == rootPath.empty() )
	{
		return;
	}

	i3::string findPath = rootPath;
	
	if( parentPath.empty() )
	{
		findPath += ("\\*.*");
	}
	else
	{
		findPath += ("\\" + parentPath + "\\*.*");
	}

	WIN32_FIND_DATAA findData;
	HANDLE hFind;
	
	hFind = ::FindFirstFileA( findPath.c_str(), &findData );

	if( INVALID_HANDLE_VALUE == hFind )
	{
		return;
	}

	i3::string fileName;
	bool isDirectory;
	
	// path 자체가 파일일 경우
	{
		isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FALSE;

		if( false == isDirectory )
		{
			// 리소스 파일 추가
			_AddResourceFile( rootPath, parentPath, findData.cFileName );
		}
	}	
	
	while( ::FindNextFileA( hFind, &findData ) )
	{	
		fileName = findData.cFileName;
		isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FALSE;

		if( isDirectory )
		{
			if( "." == fileName || ".." == fileName )
			{
				continue;
			}

			if( bRecursive )
			{
				i3::string folderPath;
				
				if( parentPath.empty() )
					folderPath = findData.cFileName;
				else
					folderPath = parentPath + "\\" + findData.cFileName;


				// 특정 폴더 무시
				if( false == m_ingnoreFolders.empty() )
				{
					i3::transform( folderPath.begin(), folderPath.end(), folderPath.begin(), std::tolower );

					if( m_ingnoreFolders.end() != m_ingnoreFolders.find( folderPath.c_str() ) )
					{
						continue;
					}
				}
				
				if( i3::string::npos != folderPath.find(".svn"))
					continue;

				// 
				_ScanResource( rootPath, folderPath, bRecursive );
			}
		}
		else
		{
			// 리소스 파일 추가
			_AddResourceFile( rootPath, parentPath, findData.cFileName );
		}
	} 

	if( nullptr != hFind )
		::FindClose( hFind );
}

bool	i3LevelResScanThread::_AddResourceFile( const i3::string& rootPath, const i3::string& path, const i3::string& fileName )
{
	if( "." == fileName || ".." == fileName )
	{
		return false;
	}

	if( i3::string::npos != fileName.find( ".svn" ) )
	{
		return false;
	}

	if( m_pFileList )
	{
		i3::string sz = rootPath + "\\";

		if( path.empty() )
			sz += (fileName);
		else
			sz += (path + "\\" + fileName);

		m_pFileList->push_back( sz.c_str() );
	}

	return true;
}