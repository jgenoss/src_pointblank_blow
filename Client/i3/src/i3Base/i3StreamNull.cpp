#include "i3CommonType.h"
#include "i3StreamNull.h"

I3_CLASS_INSTANCE( i3NullStream);


///////////////////////////////////////////////////////////////////////////////////////////////
// Null Stream

UINT32 i3NullStream::Read( void * /*Buf*/, INT32 Length)
{
	m_Pos += Length;

	if( m_Pos > m_Size)
		m_Size = m_Pos;

	return Length;
}

UINT32 i3NullStream::Write( const void * /*Buf*/, INT32 Length)
{
	m_Pos += Length;
	m_RecordSize += Length;

	if( m_Pos > m_Size)
		m_Size = m_Pos;

	return Length;
}

UINT32 i3NullStream::SetPosition( INT32 pos, INT32 MoveType)
{
	switch( MoveType) {
		case STREAM_BEGIN:
			m_Pos = pos;
			break;

		case STREAM_CURRENT:
			m_Pos += pos;
			break;

		case STREAM_END :
			m_Pos = m_Size - pos - 1;
			break;
	}

	if( m_Pos > m_Size)
		m_Size = m_Pos;

	return m_Pos;
}

