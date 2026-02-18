#include "i3CommonType.h"
#include "i3MemoryBuffer.h"
#include "i3Macro.h"
#include "i3Stream.h"
#include "i3ResourceFile.h"

I3_CLASS_INSTANCE( i3MemoryBuffer);


i3MemoryBuffer::~i3MemoryBuffer(void)
{
	if( m_bOwnBuffer)
	{
		I3MEM_SAFE_FREE_POINTER( m_pPointer );
	}
}

bool i3MemoryBuffer::Create( UINT32 sz)
{
	m_Size = sz;

	if( m_pPointer == nullptr)
	{
		m_pPointer = i3MemAlloc( sz);
	}
	else
	{
		m_pPointer = i3MemReAlloc( m_pPointer, sz);
	}
	
	I3ASSERT( m_pPointer != nullptr);

	return true;
}

bool i3MemoryBuffer::Bind( void * pPointer, UINT32 sz)
{
	m_pPointer = pPointer;

	m_Size = sz;
	m_bOwnBuffer = false;

	return true;
}

UINT32 i3MemoryBuffer::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	Rc = pStream->Write( &m_Size, sizeof(m_Size));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3MemoryBuffer::OnSave()", "Could not write size data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( m_Size > 0)
	{
		void * pBuff = getBuffer();

		Rc = pStream->Write( pBuff, m_Size);

		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MemoryBuffer::OnSave()", "Could not write buffer data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3MemoryBuffer::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	Rc = pStream->Read( &m_Size, sizeof(m_Size));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3MemoryBuffer::OnLoad()", "Could not read size data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( m_Size > 0)
	{
		void * pBuff;

		Create( m_Size);

		pBuff = getBuffer();

		Rc = pStream->Read( pBuff, m_Size);

		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MemoryBuffer::OnLoad()", "Could not read buffer data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	#if defined( I3_DEBUG)
	SetName( pResFile->getFileName());
	#endif

	return Result;
}

#if defined( I3_DEBUG)
void i3MemoryBuffer::_getInfoString( char * pszStr, const INT32 nStrMax )
{
	i3::snprintf( pszStr, nStrMax, "%s, %d", GetNameString(), m_Size);
}
#endif
