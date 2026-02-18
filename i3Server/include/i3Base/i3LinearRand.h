#ifndef __I3_LINEAR_RAND__
#define __I3_LINEAR_RAND__

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

#define		DOUBLE_BUFFER_COUNT			2

struct RAND_BUFFER
{
	INT32					m_i32Idx;
	INT32*					m_pBuffer;
};

class I3_EXPORT_BASE i3LinearRand : public i3ElementBase
{
	I3_CLASS_DEFINE( i3LinearRand );

	BOOL					m_bLockState;

	INT32					m_i32Count;

	INT32					m_i32BufferIdx;
	RAND_BUFFER				m_Buffer[ DOUBLE_BUFFER_COUNT ];

protected:

	INT32					_GetCurIdx()				{	return m_i32BufferIdx;		}
	INT32					_GetNexIdx()				
	{
		INT32 i32Idx = m_i32BufferIdx + 1;
		if( DOUBLE_BUFFER_COUNT <= i32Idx )	i32Idx = 0;
		return i32Idx;
	}
	void					_ChangeBuffer()
	{
		m_i32BufferIdx	= _GetNexIdx();
	}

public:

	i3LinearRand();
	virtual ~i3LinearRand();

	BOOL					Create( INT32 i32Count, BOOL bBasicSetting = TRUE );			// bBasicSetting = TRUE 일 경우 0~i32Count-1 중에 rand 를 돌리는 기본 세팅을 합니다.
	void					Destory();
	
	INT32					Rand();

	void					LockBuffer();
	BOOL					SetData( INT32 i32Value );
	void					UnLockBuffer();

	// Data 를 넣기 위해서는 LockBuffer -> SetData -> UnLockBuffer  순으로 진행 되어야 합니다.
	// LockBuffer();
	// SetData( 2 );
	// SetData( 3 );
	// UnLockBuffer();
};

#endif