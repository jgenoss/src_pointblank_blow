#pragma once
#define		MAX_INI_READ_SIZE 1024

class CiniFileLoader
{
private:
	char		m_szFile[MAX_PATH];
	char		m_szTempBuffer[MAX_INI_READ_SIZE];

public:
	CiniFileLoader(void);
	~CiniFileLoader(void);
	
	void		SetFile(LPCTSTR szFile);
	LPCTSTR		GetFile() {return m_szFile; };

	LPCTSTR		GetIniString(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefault="");
	int			GetIniNumber(LPCTSTR szSection, LPCTSTR szKeyName, int nDefault=0);

	BOOL		SetIniString(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szString);
	BOOL		SetIniNumber(LPCTSTR szSection, LPCTSTR szKeyName, int nNumber);

};
