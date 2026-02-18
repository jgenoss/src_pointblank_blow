#include "i3CommonType.h"
#include "i3StreamFile.h"
#include "i3Debug.h"
#include "i3Macro.h"
#include "i3Performance.h"
#include "i3ErrorHandler.h"
#include "i3PackFileSystem.h"

I3_CLASS_INSTANCE( i3FileStream, i3Stream);


//////////////////////////////////////////////////////////////////////////////////////////////////
// File Stream
BOOL i3FileStream::m_bCaseSensitive = FALSE;

i3FileStream::i3FileStream(void)
{
	m_pFile = __INVALID_FP;
	m_Pos = 0;
	i3mem::FillZero( FileName, sizeof(FileName));

	m_pPackFile = NULL;

	m_InternalOffset = 0;
}

i3FileStream::~i3FileStream(void)
{
	Close();
}

BOOL i3FileStream::Create( const char *Name, INT32 Acc, UINT32 /*Size*/)
{
	if((Acc & STREAM_READ) && !(Acc & STREAM_WRITE))
	{
		m_pPackFile = i3Pack::Open( Name);

		if( m_pPackFile != NULL)
		{
			I3ASSERT( (Acc & STREAM_ASYNC) == 0);

			m_pPackFile->AddRef();
		}
	}

	if( m_pPackFile == NULL)
	{
		UINT32 FileAccess = 0, ShareMode = 0;
		UINT32 flag = FILE_ATTRIBUTE_NORMAL;

		if( Acc & STREAM_READ)
			FileAccess |= GENERIC_READ;

		if( Acc & STREAM_WRITE)
			FileAccess |= GENERIC_WRITE;

		if( Acc & STREAM_SHAREREAD) 
		{
			ShareMode |= FILE_SHARE_READ;
			Acc |= (INT32) STREAM_READ;
		}

		if( Acc & STREAM_SHAREWRITE) 
		{
			ShareMode |= FILE_SHARE_WRITE;
			Acc |= (INT32) STREAM_WRITE;
		}

		if( Acc & STREAM_ASYNC)
		{
			// Async I/O
			flag |= FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED;
		}

		m_pFile = CreateFile( (char *)Name, FileAccess, ShareMode, NULL, CREATE_ALWAYS, flag, NULL);
		if( m_pFile == INVALID_HANDLE_VALUE) 
		{
			m_LastCode = STREAM_IOERROR;
			return FALSE;
		}

		m_Size = CalculateSize();
	}
	else
	{
		m_Size = m_pPackFile->GetSize();
	}

	//if( s_bLogEnable)
	//	__WriteLog( Name);

	m_Pos = 0;

	strncpy( (char *)FileName, (char *)Name, sizeof(FileName)-1);
	FileName[ sizeof(FileName) - 1] = 0;

	m_Access = Acc;
	m_LastCode = STREAM_NOERROR;

	return TRUE;
}

BOOL i3FileStream::Open( const char *Name, INT32 Acc, UINT32 /*Size*/)
{
	if((Acc & STREAM_READ) && !(Acc & STREAM_WRITE))
	{
		m_pPackFile = i3Pack::Open( Name);

		if( m_pPackFile != NULL)
			m_pPackFile->AddRef();
	}

	if( m_pPackFile == NULL)
	{
		UINT32 FileAccess = 0, ShareMode = 0;
		UINT32 flag = FILE_ATTRIBUTE_NORMAL;

		if( Acc & STREAM_READ)
			FileAccess |= GENERIC_READ;

		if( Acc & STREAM_WRITE)
			FileAccess |= GENERIC_WRITE;

		if( Acc & STREAM_SHAREREAD) 
		{
			ShareMode |= FILE_SHARE_READ;
			Acc |= STREAM_READ;
		}

		if( Acc & STREAM_SHAREWRITE) 
		{
			ShareMode |= FILE_SHARE_WRITE;
			Acc |= STREAM_WRITE;
		}

		if( Acc & STREAM_ASYNC)
		{
			flag |= FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED;
		}

		// 대소문자를 구분할 필요가 있을 때
		if( m_bCaseSensitive)
		{
			char szTemp[MAX_PATH];
			if( 0 == GetShortPathName( Name, szTemp, MAX_PATH ) )
			{
				I3TRACE( "Error : [i3FileStream::Open] Failed to GetShortPathName. (%s)\n", Name );
				m_LastCode = STREAM_IOERROR;
				return FALSE;
			}
			if( 0 == GetLongPathName( szTemp, szTemp, MAX_PATH ) )
			{
				I3TRACE( "Error : [i3FileStream::Open] Failed to GetLongPathName. (%s)\n", szTemp );
				m_LastCode = STREAM_IOERROR;
				return FALSE;
			}
			if( strcmp( Name, szTemp ) != 0 )
			{
				I3TRACE( "Error : [i3FileStream::Open] Case Sensitive Error. (%s) != (%s)\n", Name, szTemp );
				m_LastCode = STREAM_IOERROR;
				return FALSE;
			}
		}

		m_pFile = CreateFile( (char *)Name, FileAccess, ShareMode, NULL, OPEN_EXISTING, flag, NULL);
		if( m_pFile == INVALID_HANDLE_VALUE) 
		{
			m_LastCode = STREAM_IOERROR;
			//I3WARN( "Failed to CreateFile( \"%s\" ). err code = %d", Name, ::GetLastError()  );
			return FALSE;
		}

		m_Size = CalculateSize();
	}
	else
	{
		m_Size = m_pPackFile->GetSize();
	}

	m_Pos = 0;
	strncpy( (char *)FileName, (char *)Name, sizeof(FileName)-1);
	FileName[ sizeof(FileName) - 1] = 0;

	m_Access = Acc;
	m_LastCode = STREAM_NOERROR;

	return TRUE;
}

BOOL i3FileStream::Close(void)
{
	if( m_pPackFile != NULL)
	{
		i3Pack::Close( m_pPackFile);
		m_pPackFile = NULL;
	}
	else
	{
		if( m_pFile != NULL)
		{
			CloseHandle( m_pFile);
		}
		
		m_pFile = NULL;
	}

	m_Size = 0;
	m_Pos = 0;
	
	FileName[0] = 0;
	m_Access = 0;
	m_LastCode = STREAM_NOTINITIALIZED;

	return TRUE;
}

UINT32 i3FileStream::Read( void * pBuf, INT32 Length)
{
	INT32 ReadBytes;

	if( !(m_Access & STREAM_READ)) 
	{
		m_LastCode = STREAM_ACCESSVIOLATION;
		return STREAM_ERR;
	}

	if( m_pPackFile != NULL)
	{
		ReadBytes = m_pPackFile->Read( pBuf, Length);
		if( ReadBytes == STREAM_ERR)
		{
			//m_InternalPos = m_pPackFile->GetPosition();
			return STREAM_ERR;
		}

		if( ReadBytes < Length)
		{
			m_LastCode = STREAM_EOS;

			return STREAM_ERR;
		}
	}
	else
	{
		// DirectX or anything else...
		BOOL Rv;

		Rv = ReadFile( m_pFile, pBuf, Length, (DWORD *)&ReadBytes, NULL);
		if((ReadBytes != Length) || (Rv == FALSE)) 
		{
			if( ReadBytes != Length)
				m_LastCode = STREAM_EOS;
			else
				m_LastCode = STREAM_IOERROR;

			return STREAM_ERR;
		}
	}
	
	m_Pos += Length;
	if( m_Pos > m_Size)
		m_Size = m_Pos;

	m_LastCode = STREAM_NOERROR;

	return ReadBytes;
}

UINT32 i3FileStream::Write( const void * pBuf, INT32 Length)
{
	INT32 WriteBytes;

	if( !(m_Access & STREAM_WRITE))
	{
		m_LastCode = STREAM_ACCESSVIOLATION;
		return STREAM_ERR;
	}

	BOOL Rv;
	OVERLAPPED ovl, * pOvl = NULL;

	if( m_Access & STREAM_ASYNC)
	{
		pOvl = &ovl;

		ovl.hEvent = 0;
		ovl.Internal = 0;
		ovl.InternalHigh = 0;
		ovl.Offset = 0;
		ovl.OffsetHigh = 0;
		ovl.Pointer = 0;
	}

	Rv = WriteFile( m_pFile, pBuf, Length, (DWORD *)&WriteBytes, pOvl);
	if((WriteBytes != Length) || (Rv == (BOOL)FALSE)) 
	{
//			DWORD err = GetLastError();			// 릴리즈에 한해 경고라서 아래처럼 처리..(12.04.12.수빈)
		I3TRACE( "Error Code : %X\n", (GetLastError()));

		if( WriteBytes != Length)
			m_LastCode = STREAM_NOTENOUGHSPACE;
		else
			m_LastCode = STREAM_IOERROR;

		m_Pos = SetFilePointer( m_pFile, 0, NULL, FILE_CURRENT) - m_InternalOffset;
		m_Size = GetFileSize( m_pFile, NULL);

		return STREAM_ERR;
	}
		
	m_Pos += Length;
	m_RecordSize += Length;

	if( m_Pos > m_Size)
		m_Size = m_Pos;
	
	m_LastCode = STREAM_NOERROR;

	return WriteBytes;
}

UINT32 i3FileStream::SetPosition( INT32 pos, INT32 MoveType)
{
	if( m_pPackFile != NULL)
	{
		m_Pos = m_pPackFile->SetPosition( pos, MoveType);
	}
	else
	{
		#ifdef I3_USE_FILE_HANDLE
			switch( MoveType) 
			{
				case STREAM_BEGIN :
					m_Pos = SetFilePointer( m_pFile, m_InternalOffset + pos, NULL, FILE_BEGIN);
					break;

				case STREAM_CURRENT :
					m_Pos = SetFilePointer( m_pFile, pos, NULL, FILE_CURRENT);
					break;

				case STREAM_END :
					m_Pos = SetFilePointer( m_pFile, pos, NULL, FILE_END);
					break;
			}

		#else
			switch( MoveType) 
			{
				case STREAM_BEGIN :		__FSEEK( m_pFile, m_InternalOffset + pos, __SEEK_SET);		break;
				case STREAM_CURRENT :	__FSEEK( m_pFile, pos, __SEEK_CUR);		break;
				case STREAM_END :
					__FSEEK( m_pFile, pos, __SEEK_END);	
					break;
			}
			
			m_InternalPos = __FTELL( m_pFile) - m_InternalOffset;
		#endif
	}

	m_Pos -= m_InternalOffset;
	
	return m_Pos;
}


/*
UINT32 i3FileStream::_GetPosition(void)
{
	if( m_pPackFile != NULL)
	{
		return m_pPackFile->GetPosition();
	}
	else
	{
		#ifdef I3_USE_FILE_HANDLE
			return SetFilePointer( m_pFile, 0, NULL, FILE_CURRENT) - m_InternalOffset;
			
		#else
			I3ASSERT( m_pFile != __INVALID_FP);
			return __FTELL( m_pFile) - m_InternalOffset;
		#endif
	}
}
*/

UINT32 i3FileStream::GetPosition(void)
{
	return m_Pos;
}

i3FileStream * i3FileStream::FromHandle( HANDLE Handle)
{
	i3FileStream *pNewStream;

	I3ASSERT( GetFileType( Handle) == FILE_TYPE_DISK);
	
	pNewStream = i3FileStream::NewObject();
	pNewStream->m_pFile = Handle;


	pNewStream->m_Pos = SetFilePointer( Handle, 0, NULL, FILE_CURRENT);
	pNewStream->m_Size = SetFilePointer( Handle, 0, NULL, FILE_END);
	SetFilePointer( Handle, pNewStream->m_Pos, NULL, FILE_BEGIN);

	//#elif defined(_i3_PS2_)
	//	// not implemented yet
	//	pNewStream->m_Pos = 0;
	//	pNewStream->m_Size = 0;

	//#else
	//	// not implemented yet
	//	pNewStream->m_Pos = 0;
	//	pNewStream->m_Size = 0;

	//#endif		

	pNewStream->m_Access = STREAM_READ | STREAM_WRITE;

	return pNewStream;
}

UINT32 i3FileStream::CalculateSize(void)
{
	#ifdef I3_USE_FILE_HANDLE

		return GetFileSize( m_pFile, NULL); 

	#else
		UINT32 Pos;
		
		__FSEEK( m_pFile, 0, __SEEK_END);
		
		Pos = __FTELL( m_pFile);
		
		__FSEEK( m_pFile, m_Pos, __SEEK_SET);
		
		return Pos;		
	#endif
}

bool i3FileStream::isFile( const char * pszPath)
{
	if( i3Pack::isFile( pszPath))
		return true;

	return i3System::IsFile( pszPath) == TRUE;
}


#ifdef I3_DEBUG
static char		s_szLogName[MAX_PATH] = "";
static BOOL		s_bLogEnable = FALSE;
static BOOL		s_bFirstFile = TRUE;

void i3FileStream::WriteOpenLog( const char * szFileName )
{
	if( ! s_bLogEnable ) return;

	HANDLE	m_pFile;
	char conv[MAX_PATH + 10];
	DWORD rd;

	if( s_bFirstFile )
	{
		s_bFirstFile = FALSE;
		m_pFile = CreateFile( s_szLogName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	else
	{
		m_pFile = CreateFile( s_szLogName, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if( m_pFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	SetFilePointer( m_pFile, 0, NULL, FILE_END);

	sprintf( conv, "%s\n", szFileName );
	WriteFile( m_pFile, conv, strlen(conv), &rd, NULL);

	CloseHandle( m_pFile);
}

void i3FileStream::SetLogEnable( const char * pszFile, BOOL bEnable)
{
	s_bLogEnable = bEnable;

	if( s_bLogEnable && pszFile != NULL )
	{
		strcpy( s_szLogName, pszFile);
	}
}

#endif	// I3_DEBUG
