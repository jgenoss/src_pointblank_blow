#include "pch.h"
#include "S2ContainerList.h"

S2ContainerList::S2ContainerList()
{
	m_eState		= S2_CONTAINER_STATE_NONE;
	m_pui8Buffer	= NULL;
	m_pui16Buffer	= NULL;
	m_pui32Buffer	= NULL;
}

S2ContainerList::~S2ContainerList()
{
	Destroy();
}

bool S2ContainerList::_EmptyBuffer()
{
	// 각 버퍼는 자리수보다 1 작은 값까지 커버 할 수 있습니다.
	if( 0xFF > m_ui32Count )
	{
		m_ui32StartIdx		= 0xFF;
	}
	else if( 0xFFFF > m_ui32Count )
	{
		m_ui32StartIdx		= 0xFFFF;
	}
	else if( 0xFFFFFFFF > m_ui32Count )
	{
		m_ui32StartIdx		= 0xFFFFFFFF;
	}
	else
	{
		return false;
	}

	return true;
}

bool S2ContainerList::_FillBuffer()
{
	m_ui32StartIdx			= 0;
	
	// 각 버퍼는 자리수보다 1 작은 값까지 커버 할 수 있습니다.
	if( 0xFF > m_ui32Count )
	{
		for( UINT32 i = 0 ; i < m_ui32Count-1 ; i++ )
		{
			m_pui8Buffer[i] = (UINT8)(i+1);
		}
		m_pui8Buffer[ m_ui32Count-1 ] = 0xFF;
	}
	else if( 0xFFFF > m_ui32Count )
	{
		for( UINT32 i = 0 ; i < m_ui32Count-1 ; i++ )
		{
			m_pui16Buffer[i] = (UINT16)(i+1);
		}
		m_pui16Buffer[ m_ui32Count-1 ] = 0xFFFF;
	}
	else if( 0xFFFFFFFF > m_ui32Count )
	{
		for( UINT32 i = 0 ; i < m_ui32Count-1 ; i++ )
		{
			m_pui32Buffer[i] = i+1;
		}
		m_pui32Buffer[ m_ui32Count-1 ] = 0xFFFFFFFF;
	}
	else
	{
		return false;
	}
	
	return true;
}

bool S2ContainerList::Create( UINT32 ui32Count, S2_CONTAINER_TYPE eType )
{
	m_ui32Count				= ui32Count;
	m_eType					= eType;

	if( 0xFF > m_ui32Count )
	{
		m_eState	= S2_CONTAINER_STATE_8;
		
		m_pui8Buffer = new UINT8[m_ui32Count];
		if( NULL == m_pui8Buffer )						return false;
	}
	else if( 0xFFFF > m_ui32Count )
	{
		m_eState	= S2_CONTAINER_STATE_16;

		m_pui16Buffer = new UINT16[m_ui32Count];
		if( NULL == m_pui16Buffer )						return false;
	}
	else if( 0xFFFFFFFF > m_ui32Count )
	{
		m_eState	= S2_CONTAINER_STATE_32;

		m_pui32Buffer = new UINT32[m_ui32Count];
		if( NULL == m_pui32Buffer )						return false;
	}
	else
	{
		return false;
	}

	return ResetBuffer();
}

void S2ContainerList::Destroy()
{
	I3_SAFE_DELETE_ARRAY( m_pui8Buffer );
	I3_SAFE_DELETE_ARRAY( m_pui16Buffer );
	I3_SAFE_DELETE_ARRAY( m_pui32Buffer );
}

bool S2ContainerList::ResetBuffer()
{
	switch( m_eType )
	{
	case S2_CONTAINER_TYPE_USE:
		{
			return _EmptyBuffer();
		}
		break;
	case S2_CONTAINER_TYPE_FREE:
		{
			return _FillBuffer();
		}
		break;
	}
	return false;
}

bool S2ContainerList::PopContainerList( UINT32* pui32Idx )
{
	switch( m_eState )
	{
	case S2_CONTAINER_STATE_8:
		{
			if( 0xFF == m_ui32StartIdx )		return false;

			*pui32Idx		= m_ui32StartIdx;
			m_ui32StartIdx	= m_pui8Buffer[ m_ui32StartIdx ];
		}
		break;
	case S2_CONTAINER_STATE_16:
		{
			if( 0xFFFF == m_ui32StartIdx )		return false;

			*pui32Idx		= m_ui32StartIdx;
			m_ui32StartIdx	= m_pui16Buffer[ m_ui32StartIdx ];
		}
		break;
	case S2_CONTAINER_STATE_32:
		{
			if( 0xFFFFFFFF == m_ui32StartIdx )	return false;

			*pui32Idx		= m_ui32StartIdx;
			m_ui32StartIdx	= m_pui32Buffer[ m_ui32StartIdx ];
		}
		break;
	default:
		{
			return false;
		}
		break;
	}
	
	return true;
}

void S2ContainerList::PushContainerList( UINT32 ui32Idx )
{
	switch( m_eState )
	{
	case S2_CONTAINER_STATE_8:
		{
			m_pui8Buffer[ ui32Idx ]		= (UINT8)m_ui32StartIdx;
			m_ui32StartIdx				= ui32Idx;
		}
		break;
	case S2_CONTAINER_STATE_16:
		{
			m_pui16Buffer[ ui32Idx ]	= (UINT16)m_ui32StartIdx;
			m_ui32StartIdx				= ui32Idx;
		}
		break;
	case S2_CONTAINER_STATE_32:
		{
			m_pui32Buffer[ ui32Idx ]	= m_ui32StartIdx;
			m_ui32StartIdx				= ui32Idx;
		}
		break;
	}
}