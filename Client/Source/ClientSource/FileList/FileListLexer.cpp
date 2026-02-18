#include "stdafx.h"
#include "FileListLexer.h"

#include "strsafe.h"



char		s_Separator[]	= ";, \t\r\n\b\0";

bool	IsWhiteChar(char c, char* pWhites)
{
	if ( c == NULL )
		return true;
	while ( *pWhites != NULL )
	{
		if ( c == *pWhites )
			return true;
		pWhites++;
	}
	return false;
}

CFileListLexer::CFileListLexer()
: m_BufferPos(0), m_FileVer(0), m_BufferLength(0)
{

}

CFileListLexer::~CFileListLexer()
{
}

//UserFileList.txt , FileList.txt
bool	CFileListLexer::Open(LPCTSTR pFileName)
{
	Close();

	CFile	File;
	CFileException	FileException;
	if (!File.Open(pFileName, CFile::modeRead, &FileException))
	{
		TCHAR	Error[MAX_PATH] = {};
		if (FileException.GetErrorMessage(Error, MAX_PATH))	TRACE2("%s%s\n", _T("CFileListLexer::Open - "), Error);
		else												TRACE2("%s%s\n", _T("CFileListLexer::Open - "), _T("no message"));
		return false;
	}
	
	//Read File And Decript 
	UINT8 * pFileInfo = NULL; 
	INT32 size = (INT32)File.GetLength( ); 
	pFileInfo = (UINT8*)malloc( size ); 
	File.Read( pFileInfo, size );
	BitRotateDecript(pFileInfo, size, 4);


	// 파일 버전 읽기
	memcpy(&m_FileVer, pFileInfo, sizeof(int) );


	// 파일 데이터 복사
	m_Buffer = (char*)(pFileInfo + sizeof(int));
	if( pFileInfo != NULL )free(pFileInfo); 

	//Set Length
	m_BufferLength	= m_Buffer.GetLength();

	return true;
}


void	CFileListLexer::Close()
{
	m_FileVer	= 0;
	m_Buffer.Empty();
	m_BufferPos		= 0;
	m_BufferLength	= 0;
	m_LastToken.Empty();
}


TOKEN_FILELIST	CFileListLexer::GetToken()
{
	m_LastToken.Empty();

	//
	// 앞에 공백 문자 제거
	//
	char c;
	do
	{
		if ( m_BufferLength == m_BufferPos )
			return TOKEN_FILELIST_FILEEND;

		c = m_Buffer.GetAt(m_BufferPos++);
	} while(IsWhiteChar(c, s_Separator));

	//
	// 토큰화 시키기
	//

	while (true)
	{
		if ( c == '"' )
		{
			int idx = m_Buffer.Find('"', m_BufferPos);
			m_LastToken = m_Buffer.Mid(m_BufferPos, idx - m_BufferPos);
			m_BufferPos = idx+1;
			return TOKEN_FILELIST_VALUE;
		}

		m_LastToken += c;

		if ( m_BufferLength == m_BufferPos )
			return TOKEN_FILELIST_FILEEND;

		c = m_Buffer.GetAt(m_BufferPos++);
		if ((true == IsWhiteChar(c, s_Separator))
			|| (c == ',') || (c == ';'))
		{
			break;
		}
	}

	if (IsIdentifier(m_LastToken))
		return TOKEN_FILELIST_IDENTIFIER;

	return TOKEN_FILELIST_VALUE;
}


bool		CFileListLexer::GetTokenInt(INT & Value)
{
	if (TOKEN_FILELIST_VALUE != GetToken())
		return false;

	Value = _ttoi(GetLastTokenString());
	return true;
}

bool	CFileListLexer::GetTokenUInt(UINT & Value)
{
	if (TOKEN_FILELIST_VALUE != GetToken())
		return false;

	char * unused = NULL;
	Value = _tcstoul(GetLastTokenString(), &unused, 10);
	return true;
}


bool	CFileListLexer::GetTokenUInt(DWORD & Value)
{
	if (TOKEN_FILELIST_VALUE != GetToken())
		return false;

	char * unused = NULL;
	Value = _tcstoul(GetLastTokenString(), &unused, 10);
	return true;
}


bool	CFileListLexer::GetTokenString(LPTSTR pStr, int StrSize)
{
	if (TOKEN_FILELIST_VALUE != GetToken())
		return false;
	StringCchCopy(pStr, StrSize, GetLastTokenString());
	return true;
}

bool	CFileListLexer::GetTokenString(CString& Str)
{
	if (TOKEN_FILELIST_VALUE != GetToken())
		return false;
	Str = GetLastTokenString();
	return true;
}


bool	CFileListLexer::IsIdentifier(const CString& Token)
{
	return Token.GetAt(0) == '*';
}