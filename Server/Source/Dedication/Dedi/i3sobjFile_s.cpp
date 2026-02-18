#include "pch.h"
#include "i3sobjFile_s.h"

i3sobjFile::i3sobjFile(void)
{
	m_hFile = INVALID_HANDLE_VALUE;
	i3mem::FillZero(&m_head, sizeof(m_head));
	
	m_head.size		= sizeof(m_head);
	m_head.version	= I3SOBJ_VERSION;

	m_bSave = FALSE;

	m_szFileName[0] = '\0';
}

i3sobjFile::~i3sobjFile(void)
{
}

// 파일 오픈 (read mode는 dedication server에서 사용된다)
HANDLE i3sobjFile::Open(const char* szFilePath, BOOL bSave)
{
	// file mode
	m_bSave = bSave;
	if( !szFilePath ) return INVALID_HANDLE_VALUE;


	const char* pName = strrchr(szFilePath, '\\');
	if( pName ) 
	{
		strcpy_s( m_szFileName, MAX_PATH, pName + 1 );
	}
	else
	{
		return INVALID_HANDLE_VALUE;
	}
	
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle( m_hFile);
	}

	if(bSave) // write mode
	{
		// open file for writing
		m_hFile = CreateFile(szFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if(INVALID_HANDLE_VALUE == m_hFile ) return m_hFile;
		
		// skip header
		if(INVALID_SET_FILE_POINTER != ::SetFilePointer(m_hFile, m_head.size, 0, FILE_BEGIN))
		{
			return m_hFile; // success
		}
	}
	else
	{
		// open file for reading
		m_hFile = CreateFile( szFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
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
void i3sobjFile::Close(void)
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
DWORD i3sobjFile::Write(DS_GAME_OBJECT* gameObj)
{
	DWORD dwBytesWritten = 0;
	
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		if(TRUE == WriteFile(m_hFile, gameObj, sizeof(DS_GAME_OBJECT), &dwBytesWritten, NULL))
		{
			m_head.count++;
			return dwBytesWritten;
		}
	}
	
	return 0;
}


// 오브젝트 읽기
DWORD i3sobjFile::Read( DS_GAME_OBJECT* pGameObj )
{
	DWORD dwBytesRead = 0;

	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		if(TRUE == ReadFile(m_hFile, pGameObj, sizeof(DS_GAME_OBJECT), &dwBytesRead, NULL))
		{
			if ( pGameObj->m_i16NetType == I3_NETWORK_GAMEOBJECT_TYPE_ANIM &&
				 pGameObj->m_pDurations[0] == 0.0f)
			{
				I3TRACE("!!!!! [%s] object duration is zero..net index=%d, stype=%d\n", m_szFileName, pGameObj->m_i16NetIdx, pGameObj->m_iSType);
			}
			
			return dwBytesRead;
		}
	}
	
	return 0;
}

