#include "i3SceneDef.h"
#include "i3AttrStack.h"

I3_CLASS_INSTANCE( i3AttrStack);

i3AttrStack::i3AttrStack( INT32 AllocUnit) : m_AllocUnit(AllocUnit)
{
	ResizeStack( AllocUnit);
}

i3AttrStack::~i3AttrStack(void)
{
	I3_SAFE_RELEASE( m_pDefaultAttr);
	I3MEM_SAFE_FREE_POINTER( m_pStack ); 
}

void i3AttrStack::ResizeStack( INT32 MaxCount)
{
	m_MaxCount = MaxCount;

	if( m_pStack == nullptr)
	{
		m_pStack = (I3ATTRSTACKINFO *) i3MemAlloc( m_MaxCount * sizeof(I3ATTRSTACKINFO) );
	}
	else
	{
		m_pStack = (I3ATTRSTACKINFO *) i3MemReAlloc( m_pStack, m_MaxCount * sizeof(I3ATTRSTACKINFO) );
	}
	I3ASSERT( m_pStack != nullptr);
}

void i3AttrStack::Reset(void)
{
	m_Count = 0;
	m_State = m_State & I3SG_ATTRSTACK_STATE_DISABLED;
	m_apNext[0] = nullptr;
	m_apNext[1] = nullptr;
}

void i3AttrStack::PopToCount( INT32 count)
{
	m_Count = count;
}
