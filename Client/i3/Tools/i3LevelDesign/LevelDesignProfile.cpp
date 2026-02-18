#include "pch.h"
#include "LevelDesignProfile.h"
#include "GlobalVariable.h"
#include "i3Base/string/ext/extract_fileext.h"

CLevelDesignProfile::CLevelDesignProfile(void)
{
	m_szResPath[0] = 0;
	m_szWorkingDir[0]	= 0;
	m_szStageType[0] = 0;

	m_PosSnap = 0.2f;
	m_ScaleSnap = 0.2f;
	m_RotateSnap = 1.0f;
}

CLevelDesignProfile::~CLevelDesignProfile(void)
{
	Save();
}

void	CLevelDesignProfile::Save()
{
	AfxGetApp()->WriteProfileString( "OPTION", "ResPath", m_szResPath);
	AfxGetApp()->WriteProfileString( "OPTION", "WorkDir", m_szWorkingDir);
	AfxGetApp()->WriteProfileString( "OPTION", "StageType", m_szStageType);
	AfxGetApp()->WriteProfileInt( "OPTION", "SerializeRes", m_bSerializeRes);
	AfxGetApp()->WriteProfileInt( "OPTION", "LaunchGame", m_bLaunchGame);

	char conv[256];

	sprintf_s( conv, _countof( conv), "%f", m_PosSnap);
	AfxGetApp()->WriteProfileString( "OPTION", "Pos Snap", conv);

	sprintf_s( conv, _countof( conv), "%f", m_ScaleSnap);
	AfxGetApp()->WriteProfileString( "OPTION", "Scale Snap", conv);

	sprintf_s( conv, _countof( conv), "%f", m_RotateSnap);
	AfxGetApp()->WriteProfileString( "OPTION", "Rotate Snap", conv);
}

void	CLevelDesignProfile::Load()
{
	CString str;
	
	str = AfxGetApp()->GetProfileString( "OPTION", "ResPath", "");
	strcpy( m_szResPath, LPCTSTR( str));

	str = AfxGetApp()->GetProfileString( "OPTION", "WorkDir", "");
	strcpy( m_szWorkingDir, LPCTSTR( str));

	str = AfxGetApp()->GetProfileString( "OPTION", "StageType", "");
	strcpy( m_szStageType, LPCTSTR( str));

	m_bSerializeRes = AfxGetApp()->GetProfileInt( "OPTION", "SerializeRes", 1);
	m_bLaunchGame	= AfxGetApp()->GetProfileInt( "OPTION", "LaunchGame", 0);

	str = AfxGetApp()->GetProfileString( "OPTION", "Pos Snap", "0.2");
	m_PosSnap = (REAL32) atof( str.GetBuffer());

	str = AfxGetApp()->GetProfileString( "OPTION", "Scale Snap", "0.2");
	m_ScaleSnap = (REAL32) atof( str.GetBuffer());

	str = AfxGetApp()->GetProfileString( "OPTION", "Rotate Snap", "1.0");
	m_RotateSnap = (REAL32) atof( str.GetBuffer());

	Apply();
}

static BOOL _LoadClassMetaProc( INT32 Level, char * pszPath, WIN32_FIND_DATA * pInfo, void * pUserData)
{
	if( pInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return TRUE;

	char szExt[128];
//	i3String::SplitFileExt( pInfo->cFileName, szExt, sizeof(szExt) - 1);
	i3::extract_fileext(pInfo->cFileName, szExt);

	if( i3::generic_is_iequal( szExt, "BIN")  || 
		i3::generic_is_iequal( szExt, "i3Exec") )
	{
		char szFull[ MAX_PATH], szBack[ MAX_PATH];
		sprintf( szFull, "%s\\%s", pszPath, pInfo->cFileName);

		::GetCurrentDirectory( sizeof( szBack) - 1, szBack);
		::SetCurrentDirectory( pszPath);

		HMODULE hModule = ::LoadLibrary( szFull);
/*		hModule을 못읽은 경우 에러메시지 확인 위해 삽입..디버깅용도로 쓸수도 있으니..
        대체루틴없으면 주석삭제 하지마세요.(2012.09.18.수빈)	
		if (hModule == NULL)
		{
			LPVOID lpMsgBuf;
			DWORD last_err = GetLastError();
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				last_err,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR) &lpMsgBuf,
				0, NULL );
			const char* msg = static_cast<const char*>(lpMsgBuf);
			LocalFree(lpMsgBuf);
		}
*/

		::SetCurrentDirectory( szBack);

		if( hModule != NULL)
		{
			bool bLoaded = false;
			UINT32 version = 0;

			I3_CREATE_GAME_INTERFACE_PROC pProc;

			pProc = (I3_CREATE_GAME_INTERFACE_PROC) GetProcAddress( hModule, I3_CREATE_GAME_INTERFACE_PROC_NAME_STR);
			if( pProc != NULL)
			{
				g_pInterface = pProc( pszPath);

				{
					I3_GETVERSION_PROC pVersionProc = (I3_GETVERSION_PROC) GetProcAddress( hModule, I3_GETVERSION_PROC_NAME_STR);

					if( pVersionProc != NULL)
					{
						version = pVersionProc();
					}
				}

				if( version == I3_VERSION)
				{
					g_pInterface->OnRegisterMeta();

					g_hInterface = hModule;

					i3Error::SetChannelName( I3LOG_NOTICE, "i3LevelDesign.log" );
					i3Error::SetChannelName( I3LOG_FATAL, "i3LevelDesign.log");
					i3Error::SetChannelName( I3LOG_WARN, "i3LevelDesign.log");

					i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );
					i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE | I3ERR_CHN_CALLBACK);
					i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );

					bLoaded = true;
				}
				else
				{
					NotifyBox( NULL, "%s Module은 i3LevelDesign Tool과 동일한 버전의 i3Engine으로 컴파일되지 않아 사용이 불가합니다. 개발자에게 알려주세요.", szFull);
				}
			}
			
			if( bLoaded == false)
			{
				g_pInterface = NULL;
				FreeLibrary( hModule);
			}
		}
		else
		{
			if(i3::generic_is_iequal(szExt, "i3Exec"))
			{
				NotifyBox( NULL, "%s Module은 i3LevelDesign Tool과 동일한 버전의 i3Engine으로 컴파일되지 않아 사용이 불가합니다. 툴재빌드를 위해 개발자에게 알려주세요.", szFull);
			}
		}
	}

	return TRUE;
}

void CLevelDesignProfile::Apply()
{
	i3ResourceFile::SetWorkingFolder( m_szResPath);
	i3ResourceFile::setSearchExternRefEnable( true);

	// 해당 Folder 밑에 있는 EXE나 DLL을 검색한다.
	{
		i3System::LookForFiles( m_szResPath, "*.*", (FINDFILEPROC) _LoadClassMetaProc, NULL, 0);
	}

	if( g_pScene != NULL)
	{
		g_pScene->setSnap_Position( m_PosSnap);
		g_pScene->setSnap_Rotation( m_RotateSnap);
		g_pScene->setSnap_Scaling( m_ScaleSnap);
	}
}