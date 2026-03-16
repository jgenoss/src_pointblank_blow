#include "i3CommonType.h"
#include "i3RefStack.h"

I3_CLASS_INSTANCE( i3RefStack, i3List);

i3RefStack::i3RefStack(void)
{
	SetAllocationUnit( 128);
	SetOptmizeEnableWhenClear( FALSE);
}

i3RefStack::~i3RefStack(void)
{
}

void i3RefStack::Push( i3ElementBase * pObj)
{
	Add( pObj);
}

void i3RefStack::Pop( i3ElementBase ** pObj)
{
	I3ASSERT( GetCount() > 0);

	if( pObj != NULL)
	{
		*pObj = (i3ElementBase *) Items[ GetCount() - 1];
	}

	SetCount( GetCount() - 1);
}

