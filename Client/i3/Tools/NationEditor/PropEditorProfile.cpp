#include "StdAfx.h"

#include <sstream>

#include "PropEditorProfile.h"

CPropEditorProfile * g_pProfile = NULL;

CPropEditorProfile::CPropEditorProfile(void)
{
	Load();
}

CPropEditorProfile::~CPropEditorProfile(void)
{
	Save();
}

void	CPropEditorProfile::Load( void)
{
	DWORD dwType = REG_SZ;
	DWORD dwSize = MAX_PATH;
	HKEY hKey;
	TCHAR szBuffer[MAX_PATH] = {'\0' ,};

	LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE , 
		"SOFTWARE\\JuuL\\PefEditor" , 0 , KEY_READ , &hKey);

	if(ERROR_SUCCESS == lResult)
	{
		RegQueryValueEx(hKey , "WorkDir" , NULL , &dwType , (LPBYTE)szBuffer , &dwSize);
		RegQueryValueEx(hKey, "BakFile", NULL, NULL , (LPBYTE)&m_BakFile, &dwSize);
	}

	m_WorkDirectoryPath = szBuffer;

	RegCloseKey( hKey );
}

void	CPropEditorProfile::Save( void)
{
	CRegKey RegKey;
	RegKey.Create(HKEY_LOCAL_MACHINE , "SOFTWARE\\JuuL\\PefEditor");
	RegKey.SetValue(m_WorkDirectoryPath , "WorkDir");
	RegKey.SetValue(m_BakFile , "BakFile");
	RegKey.Close(); 
}