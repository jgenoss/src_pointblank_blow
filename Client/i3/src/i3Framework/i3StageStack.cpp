#include "i3FrameworkPCH.h"
#include "i3StageStack.h"

//I3_CLASS_INSTANCE( i3StageStack, i3ElementBase);
I3_CLASS_INSTANCE( i3StageStack);

i3StageStack::i3StageStack( UINT32 allocUnit) : m_AllocUnit(allocUnit)
{
	
}

i3StageStack::~i3StageStack(void)
{
	Reset();
}

void i3StageStack::AdjustBufferSize( UINT32 newsz)
{
	m_MaxCount = (newsz + (m_AllocUnit - 1)) / m_AllocUnit;
	m_MaxCount *= m_AllocUnit;

	/*
	INT32 hMem;
	if( m_pBuffer == nullptr)	hMem = i3MemAlloc( m_MaxCount * sizeof(INT32));
	else					hMem = i3MemReAlloc( m_pBuffer, m_MaxCount * sizeof(INT32));
	m_pBuffer = (INT32 *) i3MemLock( hMem);
	*/

	if( m_pBuffer == nullptr)	m_pBuffer = (INT32 *)i3MemAlloc( m_MaxCount * sizeof(INT32));
	else					m_pBuffer = (INT32 *)i3MemReAlloc( m_pBuffer, m_MaxCount * sizeof(INT32));	
	I3ASSERT(m_pBuffer != nullptr); 
}

void i3StageStack::Push( INT32 val)
{
	if( (m_Count + 1) > m_MaxCount)
	{
		AdjustBufferSize( m_MaxCount + 1);
	}

	*(m_pBuffer + m_Count) = val;

	m_Count++;
}

bool i3StageStack::Pop( INT32 * pVal)
{
	if( m_Count <= 0)
		return false;

	m_Count --;

	if( pVal != nullptr)
		*pVal = *(m_pBuffer + m_Count);

	return true;
}

void i3StageStack::Reset(void)
{
	I3MEM_SAFE_FREE_POINTER(m_pBuffer);

	m_Count = 0;
	m_MaxCount = 0;
}
