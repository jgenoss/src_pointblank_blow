#include "i3CommonType.h"
#include "i3StreamMem.h"
#include "i3StreamFile.h"

I3_CLASS_INSTANCE( i3MemStream, i3Stream);


////////////////////////////////////////////////////////////////////////////////////////////////////
// Memory Stream
i3MemStream::i3MemStream(void)
{
	m_pHandle = NULL;	
	m_Pos = 0;
	m_bExternMem = false;
}

i3MemStream::~i3MemStream(void)
{
	Close();
}

BOOL i3MemStream::Create( const char *NewPointer, INT32 Acc, UINT32 Sz)
{
	if( m_pHandle != NULL)
		Close();

	if( NewPointer == NULL)
	{
		//m_HandleIdx = i3MemAlloc( Sz);
		m_pHandle	= (char *)i3MemAlloc(Sz);
		I3ASSERT(m_pHandle); 
	}
	else
	{
		m_pHandle = (char *)NewPointer;
		m_bExternMem = true;
	}

	if( m_pHandle == NULL) 
	{
		m_Size = 0;

		m_LastCode = STREAM_IOERROR;
		
		#if defined( I3_WINDOWS ) || defined( I3_XBOX ) || defined( I3_WINCE )
			if( GetLastError() == ERROR_NOT_ENOUGH_MEMORY) 
			{
				m_LastCode = STREAM_NOTENOUGHSPACE;
			}
			
		#else
			m_LastCode = STREAM_NOTENOUGHSPACE;
		#endif
			
		
		return FALSE;
	}

	m_Size = Sz;
	m_Pos = 0;
	m_LastCode = STREAM_NOERROR;
	m_Access = Acc;

	return TRUE;
}

BOOL i3MemStream::Open( const char *NewPointer, INT32 Acc, UINT32 Sz)
{
	return Create( NewPointer, Acc, Sz);
}

BOOL i3MemStream::Close(void)
{
	// 주의!! 스스로 할당한 메모리에 한해서, m_HandleIdx에 올바른 값이 들어감.
	if( m_bExternMem == false)
	{
		I3MEM_SAFE_FREE_POINTER( m_pHandle );
	}

	m_Pos = 0;
	m_Size = 0;
	m_LastCode = STREAM_NOTINITIALIZED;
	m_pHandle = NULL;

	return TRUE;
}

UINT32 i3MemStream::Read( void *Buf, INT32 Length)
{
	if( !(m_Access & STREAM_READ)) {
		m_LastCode = STREAM_ACCESSVIOLATION;
		return STREAM_ERR;
	}

	if( m_Pos + Length > m_Size) {
		m_LastCode = STREAM_EOS;
		return STREAM_ERR;
	}

	memcpy( Buf, (char *)m_pHandle + m_Pos, Length);

	m_Pos += Length;

	m_LastCode = STREAM_NOERROR;

	return Length;
}

UINT32 i3MemStream::Write( const void *Buf, INT32 Length)
{
	if( !(m_Access & STREAM_WRITE)) {
		m_LastCode = STREAM_ACCESSVIOLATION;
		return STREAM_ERR;
	}

	if( m_Pos + Length > m_Size) {
		m_LastCode = STREAM_EOS;
		return STREAM_ERR;
	}

	memcpy( (char *)m_pHandle + m_Pos, Buf, Length);

	m_Pos += Length;
	m_RecordSize += Length;

	m_LastCode = STREAM_NOERROR;

	return Length;
}

UINT32 i3MemStream::SetPosition( INT32 pos, INT32 MoveType)
{
	switch( MoveType) {
		case STREAM_BEGIN:
			m_Pos = pos;
			break;

		case STREAM_CURRENT :
			m_Pos += pos;
			break;

		case STREAM_END :
			m_Pos = m_Size - pos - 1;
			break;
	}

	return m_Pos;
}

UINT32 i3MemStream::GetPosition(void)
{
	return m_Pos;
}

i3MemStream * i3MemStream::CreateFromFile( const char * pszPath)
{
	i3FileStream str;
	i3MemStream * pStream;

	if( str.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		I3WARN( "Could not open %s file.", pszPath);
		return NULL;
	}

	pStream = i3MemStream::NewObject();

	pStream->Create( NULL, STREAM_READ | STREAM_WRITE, str.GetSize());

	if( pStream->CopyFrom( &str, str.GetSize()) == STREAM_ERR)
	{
		I3WARN( "Could not read data from %s file.", pszPath);
		return NULL;
	}

	str.Close();

	return pStream;
}

i3MemStream * i3MemStream::FromHandle( void *pNew)
{
	BOOL Rv;
	i3MemStream *pNewStream;

	#if defined( I3_WINDOWS ) || defined( I3_XBOX ) || defined( I3_WINCE )
	I3ASSERT( LocalFlags( pNew) != LMEM_INVALID_HANDLE);
	#endif

	pNewStream = i3MemStream::NewObject();
	I3ASSERT( pNewStream != NULL);
	
	
	UINT32 uSize;
	#if defined( I3_WINDOWS) || defined( I3_XBOX ) || defined( I3_WINCE )
		uSize = LocalSize( pNew);
	
	#elif defined( I3_PS2)
		// not implemented yet
		uSize = 0;
		
	#else
		// not implemented yet
		uSize = 0;
		
	#endif
		
	
	Rv = pNewStream->Open( (const char *)pNew, (STREAM_READ | STREAM_WRITE), uSize);
	if( Rv == FALSE) 
	{
		delete pNewStream;
		return NULL;
	}

	return pNewStream;
}
