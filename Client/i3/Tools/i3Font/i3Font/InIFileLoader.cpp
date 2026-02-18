#include "StdAfx.h"
#include "IniFileLoader.h"

CiniFileLoader::CiniFileLoader(void)
{
	m_szFile[0]	= 0;
}

CiniFileLoader::~CiniFileLoader(void)
{
}

void	CiniFileLoader::SetFile(LPCTSTR szFile)
{
	lstrcpy(m_szFile, szFile);
}

LPCTSTR CiniFileLoader::GetIniString(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefault)
{
	if(!szSection || !szKeyName || !m_szFile[0]) 
		return "";

	GetPrivateProfileString(szSection, szKeyName, szDefault, m_szTempBuffer, sizeof(m_szTempBuffer),  m_szFile);
	
	return m_szTempBuffer;
}

int CiniFileLoader::GetIniNumber(LPCTSTR szSection, LPCTSTR szKeyName, int nDefault)
{
	if(!szSection || !szKeyName || !m_szFile[0]) 
		return NULL;

	int nTemp=NULL;
	
	nTemp = GetPrivateProfileInt(szSection, szKeyName, nDefault,  m_szFile);
	
	return nTemp;
}


BOOL CiniFileLoader::SetIniString(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR lpString)
{
	if(!szSection || !szKeyName || !m_szFile[0]) 
		return FALSE;

	if(!WritePrivateProfileString(szSection, szKeyName, lpString, m_szFile)) return FALSE;

	return TRUE;
}


BOOL CiniFileLoader::SetIniNumber(LPCTSTR szSection, LPCTSTR szKeyName, int nNumber)
{
	if(!szSection || !szKeyName || !m_szFile[0]) 
		return FALSE;

	char  lpString[200]; 
	
	sprintf(lpString, "%d",nNumber);

	if(!WritePrivateProfileString(szSection, szKeyName, lpString, m_szFile)) return FALSE;

	return TRUE;
}

