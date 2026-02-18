#include "pch.h"
#include "GlobalInfoDef.h"

#include "i3Base/string/ext/sprintf.h"

void GAME_LOGIN_INFO::Init( void)
{
	m_UserId[0]			= 0;
	m_UserPw[0]			= 0;
	m_wstrUserNick.clear();

	m_MacAddress = 0;
	
	i3mem::FillZero(m_SessionKey, sizeof(m_SessionKey));
	i3mem::FillZero(m_SessionID, sizeof(m_SessionID));
	m_UserUID = -1;

	m_bWebLogin = false;
}
void GAME_LOGIN_INFO::SetUserUID(INT64 UID)
{
	m_UserUID = UID;
}

void GAME_LOGIN_INFO::SetUserID( const char * pID)
{
	StringCchCopy(m_UserId, _countof(m_UserId), pID);
}

void GAME_LOGIN_INFO::SetUserPassword( const char * pPW)
{
	StringCchCopy(m_UserPw, _countof(m_UserPw), pPW);
}

void GAME_LOGIN_INFO::SetUserNick( const i3::rc_wstring& wstrNick)
{
	m_wstrUserNick = wstrNick;
}

void GAME_LOGIN_INFO::SetExtUID(const char * pExtUID)
{
	StringCchCopy(m_strExtUID, _countof(m_strExtUID), pExtUID);
}

void GAME_LOGIN_INFO::SetBillingID(const char * pUID)
{
	StringCchCopy(m_BillingID, _countof(m_BillingID), pUID);
}

void GAME_LOGIN_INFO::SetLinkMethod(UINT8 value)
{
	m_LinkMethod = value;
}

void GAME_LOGIN_INFO::SetSessionKey( UINT8 * pKey)
{
	I3ASSERT( pKey);
	i3mem::Copy(m_SessionKey, pKey, sizeof(m_SessionKey));
}

void GAME_LOGIN_INFO::SetSessionID( const char * pID)
{
	I3ASSERT(pID);

	INT32 size = sizeof(m_SessionID) - 1;	//	32byte

	i3mem::Copy( m_SessionID, (char*) pID, size);
	m_SessionID[size] = 0;	//	¸¶Áö¸· nullptr
}

namespace FileUtil{

size_t get_file_size(const std::string & full_path_name)
{
	size_t ret = 0;

	FILE* file = fopen(full_path_name.c_str(), "r");

	if(file) 
	{
		fseek (file, 0, SEEK_END);
		ret = ftell(file);
		fclose(file);
	}

	return ret;
}

size_t get_file_size(const std::wstring & full_path_name)
{
	size_t ret = 0;

	FILE* file = _wfopen(full_path_name.c_str(), L"r");

	if(file) 
	{
		fseek (file, 0, SEEK_END);
		ret = ftell(file);
		fclose(file);
	}

	return ret;
}

bool is_exist_file(const i3::string & full_path_name)
{
    HANDLE hFind;
    WIN32_FIND_DATAA fd;

    if ((hFind = ::FindFirstFileA(full_path_name.c_str(), &fd)) != INVALID_HANDLE_VALUE) 
	{
        FindClose (hFind);
        return true;
    }

	return false;
}

bool is_exist_file(const i3::wstring & full_path_name)
{
    HANDLE hFind;
    WIN32_FIND_DATAW fd;

    if ((hFind = ::FindFirstFileW(full_path_name.c_str(), &fd)) != INVALID_HANDLE_VALUE) 
	{
        FindClose (hFind);
        return true;
    }

	return false;
}


bool is_exist_dir(const i3::string & full_path)
{
	return is_exist_file(full_path);
}

bool is_exist_dir(const i3::wstring & full_path)
{
	return is_exist_file(full_path);
}

void create_directory(const i3::string & full_path)
{
	if( !is_exist_dir(full_path) )
	{
		CreateDirectoryA( full_path.c_str(), nullptr);
	}
}

void create_directory(const i3::wstring & full_path)
{
	if( !is_exist_dir(full_path) )
	{
		CreateDirectoryW( full_path.c_str(), nullptr);
	}
}

i3::string string_by_date(const i3::string & split)
{
	SYSTEMTIME	sysTime;
	GetLocalTime(&sysTime);

	i3::string result;
	i3::sprintf(result, "%d%s%02d%s%02d", sysTime.wYear, split, sysTime.wMonth, split, sysTime.wDay);
	return i3::move(result);
}

i3::wstring wstring_by_date(const i3::wstring & split)
{
	SYSTEMTIME	sysTime;
	GetLocalTime(&sysTime);

	wchar_t buf[MAX_PATH] = L"";

#if _MSC_VER < 1400	// below 2005
	_swprintf(buf, L"%d%s%02d%s%02d", 
		sysTime.wYear, split.c_str(), sysTime.wMonth, split.c_str(), sysTime.wDay );
#else				// 2005, 2008
	swprintf(buf, MAX_PATH, L"%d%s%02d%s%02d", 
		sysTime.wYear, split.c_str(), sysTime.wMonth, split.c_str(), sysTime.wDay);
#endif

	return i3::wstring( buf );
}

i3::string string_by_time(const i3::string & split)
{
	SYSTEMTIME	sysTime;
	GetLocalTime(&sysTime);

	char buf[MAX_PATH] = "";

#if _MSC_VER < 1400	// below 2005
	_stprintf(buf, "%02d%s%02d%s%02d", 
		sysTime.wHour, split.c_str(), sysTime.wMinute, split.c_str(), sysTime.wSecond );
#else				// 2005, 2008
	_stprintf_s(buf, MAX_PATH, "%02d%s%02d%s%02d", 
		sysTime.wHour, split.c_str(), sysTime.wMinute, split.c_str(), sysTime.wSecond);
#endif

	return i3::string( buf );
}

i3::wstring wstring_by_time(const i3::wstring & split)
{
	SYSTEMTIME	sysTime;
	GetLocalTime(&sysTime);

	wchar_t buf[MAX_PATH] = L"";

#if _MSC_VER < 1400	// below 2005
	_swprintf(buf, L"%02d%s%02d%s%02d", 
		sysTime.wHour, split.c_str(), sysTime.wMinute, split.c_str(), sysTime.wSecond );
#else				// 2005, 2008
	swprintf(buf, MAX_PATH, L"%02d%s%02d%s%02d", 
		sysTime.wHour, split.c_str(), sysTime.wMinute, split.c_str(), sysTime.wSecond);
#endif

	return std::wstring( buf );
}

}
