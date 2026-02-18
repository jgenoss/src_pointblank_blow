#include "i3CommonType.h"
#include "i3RefStack.h"
#include "i3Debug.h"

i3RefStack::i3RefStack(void)
{
	m_list.reserve(128);
}


void i3RefStack::Pop( i3ElementBase ** pObj)
{
	I3ASSERT( m_list.size() > 0);
	if( pObj != nullptr) *pObj = m_list.back();
	m_list.pop_back();
}

