#include "i3CommonType.h"
#include "i3StreamFileAsync.h"
#include "i3Debug.h"
#include "i3Macro.h"
#include "i3Performance.h"
#include "i3ErrorHandler.h"
#include "i3PackFileSystem.h"
#include "i3PackFile.h"

I3_CLASS_INSTANCE( i3AsyncFileStream);

i3AsyncFileStream::i3AsyncFileStream(void)
{
	#if defined( I3_WINDOWS)
	i3mem::FillZero( &m_Ovl, sizeof(m_Ovl));
	#endif
}

bool  i3AsyncFileStream::Create(const i3::rc_string& strFileName, INT32 Acc, UINT32 Size)
{
	return i3FileStream::Create(strFileName, Acc | STREAM_ASYNC, Size);
}

bool  i3AsyncFileStream::Open(const i3::rc_string& strFileName, INT32 Acc, UINT32 Size)
{
	return i3FileStream::Open(strFileName, Acc | STREAM_ASYNC, Size);
}

bool  i3AsyncFileStream::Close(void)
{
	#if defined( I3_WINDOWS)
	if( m_hIOCP != INVALID_HANDLE_VALUE)
	{
		CloseHandle( m_hIOCP);
		m_hIOCP = INVALID_HANDLE_VALUE;
	}
	#endif

	return i3FileStream::Close();
}

UINT32 i3AsyncFileStream::Read( void * pBuf, INT32 Length)
{
	INT32 ReadBytes;

	if( !(m_Access & STREAM_READ)) 
	{
		m_LastCode = STREAM_ACCESSVIOLATION;
		return STREAM_ERR;
	}

	if( m_pPackFile != nullptr)
	{
		ReadBytes = m_pPackFile->Read( pBuf, Length);
		if( ReadBytes == STREAM_ERR)
		{
			//m_InternalPos = m_pPackFile->GetPosition();
			return STREAM_ERR;
		}
	}
	else
	{
		// DirectX or anything else...
		#if defined( I3_WINDOWS ) || defined( I3_WINCE )
			m_Ovl.Internal = 0;
			m_Ovl.InternalHigh = 0;
			m_Ovl.Offset = m_TargetPos;
			m_Ovl.OffsetHigh = 0;
			m_Ovl.hEvent = 0;

			if( ::ReadFile( m_pFile, pBuf, Length, (DWORD *)&ReadBytes, &m_Ovl) == FALSE)
				return STREAM_ERR;
		#endif
	}

	m_Pos += Length;
	if( m_Pos > m_Size)
		m_Size = m_Pos;

	m_LastCode = STREAM_NOERROR;

	return ReadBytes;
}

UINT32 i3AsyncFileStream::Write( const void * pBuf, INT32 Length)
{
	INT32 WriteBytes = 0;

	if( !(m_Access & STREAM_WRITE))
	{
		m_LastCode = STREAM_ACCESSVIOLATION;
		return STREAM_ERR;
	}

	#if defined( I3_WINDOWS ) || defined( I3_WINCE )
		m_Ovl.hEvent = 0;
		m_Ovl.Internal = 0;
		m_Ovl.InternalHigh = 0;
		m_Ovl.Offset = m_TargetPos;
		m_Ovl.OffsetHigh = 0;

		WriteFile( m_pFile, pBuf, Length, (DWORD *)&WriteBytes, &m_Ovl);
	#endif

	m_Pos += Length;
	m_RecordSize += Length;

	if( m_Pos > m_Size)
		m_Size = m_Pos;
	
	m_LastCode = STREAM_NOERROR;

	return WriteBytes;
}

UINT32 i3AsyncFileStream::SetPosition( INT32 Pos, INT32 MoveType)
{
	switch( MoveType) 
	{
		case STREAM_BEGIN :
			m_TargetPos = Pos;
			break;

		case STREAM_CURRENT :
			m_TargetPos = m_Pos + Pos;
			break;

		case STREAM_END :
			m_TargetPos = m_Size + Pos;
			break;
	}

	return m_TargetPos;
}

bool	i3AsyncFileStream::WaitCompletion()
{
	DWORD bytes;

	return ::GetOverlappedResult( m_pFile, &m_Ovl, &bytes, TRUE) == TRUE;
}

bool	i3AsyncFileStream::CancelIO(void)
{
	return ::CancelIo( m_pFile) == TRUE;
}

bool	i3AsyncFileStream::HasFinished(void)
{
	return HasOverlappedIoCompleted( &m_Ovl);
}
