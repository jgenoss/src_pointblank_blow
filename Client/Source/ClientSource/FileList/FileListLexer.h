#ifndef __FILELISTLEXER_H__
#define	__FILELISTLEXER_H__


enum	TOKEN_FILELIST
{
	TOKEN_FILELIST_IDENTIFIER,
	TOKEN_FILELIST_VALUE,
	TOKEN_FILELIST_INVALID,
	TOKEN_FILELIST_FILEEND,
};



#define			FILE_VER_SIZE		(sizeof(INT32))


class	CFileListLexer
{
public:
	CFileListLexer();
	~CFileListLexer();

public:
	bool	Open(LPCTSTR pFileName);
	void	Close();

	TOKEN_FILELIST	GetToken();
	bool	GetTokenInt(INT & Value);
	bool	GetTokenUInt(UINT & Value);
	bool	GetTokenUInt(DWORD & Value);
	bool	GetTokenString(LPTSTR pStr, int StrSize = MAX_PATH);
	bool	GetTokenString(CString& Str);

	bool	IsIdentifier(const CString& Token);

	INT32			GetFileVersion()		{return m_FileVer;}
	const CString&	GetLastTokenString()	{return m_LastToken;}

private:
	INT32		m_FileVer;
	CString		m_Buffer;
	UINT		m_BufferPos;
	UINT		m_BufferLength;
	CString		m_LastToken;
};


#endif

