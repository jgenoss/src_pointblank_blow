#include "pch.h"
#include "i3sRpnFile_s.h"

i3sRespawnFile::i3sRespawnFile(void)
{
	m_hFile = INVALID_HANDLE_VALUE;
	i3mem::FillZero(&m_head, sizeof(m_head));
	
	m_head.size		= sizeof(m_head);
	m_head.version	= I3SRPN_VERSION;

	m_bSave = FALSE;
}

i3sRespawnFile::~i3sRespawnFile(void)
{
}

// 파일 오픈 (read mode는 dedication server에서 사용된다)
HANDLE i3sRespawnFile::Open(const char* pstrFilePath, BOOL bSave, BOOL bSkipHeader )
{
	// file mode
	m_bSave = bSave;
	
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle( m_hFile);
	}

	if(bSave) // write mode
	{
		// open file for writing
		m_hFile = CreateFile(pstrFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if( INVALID_HANDLE_VALUE == m_hFile ) return m_hFile;
		
		// skip header
		if( ! bSkipHeader )
		{
			if(INVALID_SET_FILE_POINTER != ::SetFilePointer(m_hFile, m_head.size, 0, FILE_BEGIN))
			{
				return m_hFile; // success
			}
		}
		return m_hFile; 
	}
	else
	{
		// open file for reading
		m_hFile = CreateFile(pstrFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (INVALID_HANDLE_VALUE == m_hFile) return m_hFile;
		
		// read header
		DWORD dwBytesRead = 0;
		if( TRUE == ReadFile(m_hFile, &m_head, sizeof(m_head), &dwBytesRead, NULL) )
		{
			return m_hFile; // success
		}
	}

	// failure
	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
	return m_hFile;
}

// 파일 닫기
void i3sRespawnFile::Close(void)
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		if(m_bSave)
		{
			// write header
			if(INVALID_SET_FILE_POINTER != ::SetFilePointer(m_hFile, 0, 0, FILE_BEGIN))
			{
				DWORD dwBytesWritten = 0;
				WriteFile(m_hFile, &m_head, sizeof(m_head), &dwBytesWritten, NULL);
			}
		}
		
		CloseHandle( m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

// 오브젝트 기록
DWORD i3sRespawnFile::Write( DS_RESPAWN_OBJECT* pRespawnObj)
{
	DWORD dwBytesWritten = 0;
	
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		if(TRUE == WriteFile(m_hFile, pRespawnObj, sizeof(DS_RESPAWN_OBJECT), &dwBytesWritten, NULL))
		{
			m_head.count++;
			return dwBytesWritten;
		}
	}
	
	return 0;
}


// 오브젝트 읽기
DWORD i3sRespawnFile::Read( DS_RESPAWN_OBJECT* pRespawnObj )
{
	DWORD dwBytesRead = 0;

	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		if(TRUE == ReadFile(m_hFile, pRespawnObj, sizeof(DS_RESPAWN_OBJECT), &dwBytesRead, NULL))
		{
			return dwBytesRead;
		}
	}
	
	return 0;
}

