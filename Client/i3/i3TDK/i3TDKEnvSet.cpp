#include "stdafx.h"
#include "i3TDKEnvSet.h"
#include <winreg.h>

I3_CLASS_INSTANCE( i3TDKEnvSet);

#define			REG_BASE_POS		_T("Software\\Zepetto\\Project")

i3TDKEnvSet::i3TDKEnvSet(void)
{
	m_Active.init();

	Load();

	typedef BOOL(WINAPI *CHANGEWINDOWMESSAGEFILTER)(UINT message, DWORD dwFlag);
	CHANGEWINDOWMESSAGEFILTER ChangeWindowMessageFilter = nullptr;
	HINSTANCE hDll;

	hDll = LoadLibrary("USER32.DLL");
	if (hDll == nullptr) {
		AfxMessageBox("USER32.DLL이 없습니다.");
		return;
	}
	ChangeWindowMessageFilter = (CHANGEWINDOWMESSAGEFILTER)GetProcAddress(hDll, "ChangeWindowMessageFilter");

	if (ChangeWindowMessageFilter != nullptr)
	{
		ChangeWindowMessageFilter(WM_DROPFILES, 1);
		ChangeWindowMessageFilter(WM_COPYDATA, 1);
		ChangeWindowMessageFilter(0x0049, 1);
	}
	else
	{
		AfxMessageBox("ChangeWindowMessageFilter함수가 없습니다.");
	}

	FreeLibrary(hDll);
}

i3TDKEnvSet::~i3TDKEnvSet(void)
{
	Save();

	if( m_hRegKey != nullptr)
	{
		RegCloseKey( m_hRegKey);
		m_hRegKey = nullptr;
	}
}

bool i3TDKEnvSet::_Open(void)
{
	INT32 rv;

	if( m_hRegKey != nullptr)
		return true;
	
	rv = RegCreateKeyEx( HKEY_CURRENT_USER, REG_BASE_POS, 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &m_hRegKey, nullptr);
	if( rv != ERROR_SUCCESS)
	{
		I3PRINTLOG(I3LOG_WARN,  "Registry에서 필요한 값을 읽어들이지 못했음. 권한 문제일 수 있음.");
		return false;
	}

	return true;
}

void i3TDKEnvSet::Load(void)
{
	INT32 rv;
	DWORD len;

	if( _Open() == false)
		return;

	// 현재 Project이름 구하기
	len = sizeof( m_Active.m_szProject);

	rv = RegQueryValueEx( m_hRegKey, _T("Active"), nullptr, nullptr, (LPBYTE) m_Active.m_szProject, &len);
	if( rv != ERROR_SUCCESS)
	{
		return;
	}

	SelectProject( m_Active.m_szProject);
}

void i3TDKEnvSet::Save(void)
{
	_Open();
}

INT32 i3TDKEnvSet::getProjectCount(void)
{
	if( _Open() == false)
		return 0;

	INT32 rv;
	DWORD cnt = 0;

	rv = RegQueryInfoKey( m_hRegKey, nullptr, nullptr, nullptr, &cnt, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
	if( rv != ERROR_SUCCESS)
		return 0;

	return cnt;
}

bool i3TDKEnvSet::getProject( INT32 idx, I3TDK_PROJECT_INFO * pInfo)
{
	INT32 rv;
	DWORD len;
//	HKEY	hKey = nullptr;
//	bool bResult = false;

	len = sizeof( pInfo->m_szProject);

	rv = RegEnumKeyEx( m_hRegKey, idx, pInfo->m_szProject, &len, nullptr, nullptr, nullptr, nullptr);
	if( rv != ERROR_SUCCESS)
	{
		I3PRINTLOG(I3LOG_WARN,  "Project 설정을 읽을 수 없습니다.");
		return false;
	}

	return getProject( pInfo->m_szProject, pInfo);
}

bool i3TDKEnvSet::getProject( const TCHAR * pszProjectName, I3TDK_PROJECT_INFO * pInfo)
{
	INT32 rv;
	DWORD len;
	HKEY	hKey = nullptr;
	TCHAR szPath[ MAX_PATH];
	bool bResult = false;

	i3::snprintf( szPath, MAX_PATH, _T("%s\\%s"), REG_BASE_POS, pszProjectName);

	rv = RegOpenKeyEx( HKEY_CURRENT_USER, szPath, 0, KEY_ALL_ACCESS, &hKey);
	if( rv != ERROR_SUCCESS)
	{
		I3PRINTLOG(I3LOG_WARN,  "Registry에서 필요한 값을 읽어들이지 못했음. 권한 문제일 수 있음.");
		goto ExitPart;
	}

	// Project Name
	i3::safe_string_copy( pInfo->m_szProject, pszProjectName, 256);

	// Work Dir
	{
		len = sizeof( pInfo->m_szWorkDir);
		pInfo->m_szWorkDir[0] = 0;

		rv = RegQueryValueEx( hKey, _T("WorkDir"), nullptr, nullptr, (LPBYTE) pInfo->m_szWorkDir, &len);

	}
	// ChangeTexPefPath
	{
		pInfo->m_vecChangeTexPefPath.clear();

		TCHAR strPath[MAX_PATH];
		len = sizeof( strPath );

		strPath[0] = 0;
		rv = RegQueryValueEx( hKey, _T("ChangeTexPefPath"), nullptr, nullptr, (LPBYTE) strPath, &len);

		if (rv == ERROR_SUCCESS )	// 성공..
		{
			pInfo->m_vecChangeTexPefPath.push_back(strPath);
			::RegDeleteValue( hKey, _T("ChangeTexPefPath") );		// 더 이상 쓰지 않습니다.  호환성 코드..
		}
		
		DWORD numPath = 0;
		len = sizeof(DWORD);
		rv = RegQueryValueEx(hKey, _T("NumChangeTexPefPath"), nullptr, nullptr, (LPBYTE)&numPath, &len);

		if (rv == ERROR_SUCCESS )
		{
			pInfo->m_vecChangeTexPefPath.resize(numPath);			
			i3::stack_string strPathSubKey;
		
			for (DWORD i = 0 ; i < numPath ; ++i)
			{
				i3::sprintf(strPathSubKey, "ChangeTexPefPath_%03d", i);
				strPath[0] = 0;
				len = sizeof(strPath);
				rv = RegQueryValueEx( hKey, strPathSubKey.c_str(), nullptr, nullptr, (LPBYTE) strPath, &len);
				
				if (rv == ERROR_SUCCESS)
				{
					pInfo->m_vecChangeTexPefPath[i] = strPath;		// 실패시엔 그냥 공백문자열로 방치..
				}
			}
		}

	}

	// Description
	{
		len = sizeof( pInfo->m_szDesc);
		pInfo->m_szDesc[0] = 0;

		rv = RegQueryValueEx( hKey, _T("Desc"), nullptr, nullptr, (LPBYTE) pInfo->m_szDesc, &len);
	}

	bResult = true;

ExitPart:

	if( hKey != nullptr)
	{
		RegCloseKey( hKey);
	}

	return bResult;
}

bool i3TDKEnvSet::setProject( I3TDK_PROJECT_INFO * pInfo)
{
	INT32 rv;
	DWORD len;
	HKEY	hKey = nullptr;
	TCHAR szPath[ MAX_PATH];
	bool bResult = false;

	i3::snprintf( szPath, MAX_PATH, _T("%s\\%s"), REG_BASE_POS, pInfo->m_szProject);

	rv = RegCreateKeyEx( HKEY_CURRENT_USER, szPath, 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &hKey, nullptr);
	if( rv != ERROR_SUCCESS)
	{
		I3PRINTLOG(I3LOG_WARN,  "Registry에서 필요한 값을 저장하지 못했음. 권한 문제일 수 있음.");
		goto ExitPart;
	}

	// Work Dir
	{
		len = i3::generic_string_size( pInfo->m_szWorkDir) + 1;

		rv = RegSetValueEx( hKey, _T("WorkDir"), 0, REG_SZ, (const BYTE *) pInfo->m_szWorkDir, len * sizeof(TCHAR) );
		if( rv != ERROR_SUCCESS)
		{
			I3PRINTLOG(I3LOG_WARN,  "Registry에서 필요한 값을 읽어들이지 못했음. 권한 문제일 수 있음.");
			goto ExitPart;
		}
	}
	
	// ChangeTex pef file	// 레지스트리 저장 포맷이 바뀝니다..  여러개 등록하므로 갯수를 따로 등록, "_00x"형태의 넘버링을 쓰도록 합니다.
	{
		::RegDeleteValue( hKey, _T("ChangeTexPefPath") );		// 더 이상 쓰지 않습니다.  호환성 코드..
		
		i3::stack_string strSubKey;

		DWORD oldNumPath = 0;
		len = sizeof(DWORD);
		rv = RegQueryValueEx(hKey, _T("NumChangeTexPefPath"), nullptr, nullptr, (LPBYTE)&oldNumPath, &len);
		
		if (rv == ERROR_SUCCESS)	// 기존에 있던 것은 리셋해주어야한다..
		{
			for (DWORD i = 0 ; i < oldNumPath ; ++i)
			{
				i3::sprintf(strSubKey, "ChangeTexPefPath_%03d", i);
				::RegDeleteValue(hKey, strSubKey.c_str());
			}
		}
	
		const DWORD numPath = DWORD(pInfo->m_vecChangeTexPefPath.size());
		
		rv = RegSetValueEx(hKey, _T("NumChangeTexPefPath"), 0, REG_DWORD, (const BYTE*)&numPath, sizeof(DWORD));

		for (DWORD i = 0 ; i < numPath ; ++i)
		{
			i3::sprintf(strSubKey, "ChangeTexPefPath_%03d", i);

			len = INT32( pInfo->m_vecChangeTexPefPath[i].length() ) + 1;
			rv = RegSetValueEx(hKey, strSubKey.c_str(), 0, REG_SZ, (const BYTE*)pInfo->m_vecChangeTexPefPath[i].c_str(), len * sizeof(TCHAR) );
		}
	}

	// Description
	{
		len = i3::generic_string_size( pInfo->m_szDesc) + 1;

		rv = RegSetValueEx( hKey, _T("Desc"), 0, REG_SZ, (const BYTE *) pInfo->m_szDesc, len * sizeof(TCHAR) );
		if( rv != ERROR_SUCCESS)
		{
			I3PRINTLOG(I3LOG_WARN,  "Registry에서 필요한 값을 읽어들이지 못했음. 권한 문제일 수 있음.");
			goto ExitPart;
		}
	}
	
	bResult = true;

ExitPart:

	if( hKey != nullptr)
	{
		RegCloseKey( hKey);
	}

	return bResult;
}

void i3TDKEnvSet::SelectProject( const TCHAR* pszProjName)
{
	if( getProject( pszProjName, &m_Active) == true)
	{
		RegSetValueEx( m_hRegKey, _T("Active"), 0, REG_SZ, (const BYTE *) pszProjName, (i3::generic_string_size( pszProjName) + 1) * sizeof(TCHAR) );
	}
}

void i3TDKEnvSet::DeleteProject(I3TDK_PROJECT_INFO * pInfo)
{
	if (pInfo)
	{
		RegDeleteKey(m_hRegKey, pInfo->m_szProject);
	}
}