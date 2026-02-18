#include "i3CommonType.h"
#include "i3LinearRand.h"

I3_CLASS_INSTANCE( i3LinearRand, i3ElementBase );

i3LinearRand::i3LinearRand()
{
	m_i32Count			= 0;
	for( INT32 i = 0 ; i < DOUBLE_BUFFER_COUNT ; i++ )
	{
		m_Buffer[i].m_pBuffer	= NULL;
	}
}

i3LinearRand::~i3LinearRand()
{
	Destory();
}

BOOL i3LinearRand::Create( INT32 i32Count, BOOL bBasicSetting )
{
	m_i32Count			= i32Count;

	m_Buffer[0].m_pBuffer	= (INT32*)i3MemAlloc( sizeof(INT32) * m_i32Count );
	if( NULL == m_Buffer[0].m_pBuffer )												return FALSE;
	m_Buffer[1].m_pBuffer	= (INT32*)i3MemAlloc( sizeof(INT32) * m_i32Count );
	if( NULL == m_Buffer[1].m_pBuffer )												return FALSE;

	m_Buffer[0].m_i32Idx	= m_i32Count;
	m_Buffer[1].m_i32Idx	= 0;

	if( bBasicSetting )
	{
		for( INT32 i = 0 ; i < m_i32Count ; i++ )
		{
			m_Buffer[0].m_pBuffer[i]	= i;
		}
	}
	return TRUE;
}

void i3LinearRand::Destory()
{
	for( INT32 i = 0 ; i < DOUBLE_BUFFER_COUNT ; i++ )
	{
		I3MEM_SAFE_FREE( m_Buffer[i].m_pBuffer );
	}
}

void i3LinearRand::LockBuffer()
{
	m_bLockState			= TRUE;

	RAND_BUFFER* pCurBuffer = &m_Buffer[ _GetCurIdx() ];
	RAND_BUFFER* pNexBuffer = &m_Buffer[ _GetNexIdx() ];

	pCurBuffer->m_i32Idx	= 0;
	pNexBuffer->m_i32Idx	= 0;
}

BOOL i3LinearRand::SetData( INT32 i32Value )
{
	if( FALSE == m_bLockState )		return FALSE;

	RAND_BUFFER* pCurBuffer = &m_Buffer[ _GetCurIdx() ];

	if( pCurBuffer->m_i32Idx + 1 >= m_i32Count )		return FALSE;

	pCurBuffer->m_pBuffer[ pCurBuffer->m_i32Idx ]	= i32Value;
	pCurBuffer->m_i32Idx++;

	return TRUE;
}

void i3LinearRand::UnLockBuffer()
{
	m_bLockState			= FALSE;
}

INT32 i3LinearRand::Rand()
{
	RAND_BUFFER* pCurBuffer = &m_Buffer[ _GetCurIdx() ];
	RAND_BUFFER* pNexBuffer = &m_Buffer[ _GetNexIdx() ];
	
	INT32 i32RandIdx = 0;
	if( 1 < pCurBuffer->m_i32Idx )
	{	// 현재 버퍼 중 랜덤하게 하나의 값을 뽑음
		i32RandIdx	= rand()%pCurBuffer->m_i32Idx;
	}

	INT32 i32Value		= pCurBuffer->m_pBuffer[ i32RandIdx ];

	pNexBuffer->m_pBuffer[ pNexBuffer->m_i32Idx ] = i32Value;
	pNexBuffer->m_i32Idx++;

	pCurBuffer->m_i32Idx--;
	if( 0 < pCurBuffer->m_i32Idx )
	{
		// 현재 버퍼를 비우고 맨 마지막의 값을 앞으로 땡김		
		pCurBuffer->m_pBuffer[ i32RandIdx ] = pCurBuffer->m_pBuffer[ pCurBuffer->m_i32Idx ];		
	}
	else
	{
		_ChangeBuffer();
	}
	
	return i32Value;
}