#include "i3CommonType.h"
#include "i3ListMeta.h"


static BOOL s_bRegisteri3ListMeta = FALSE;

I3_EXPORT_BASE void i3ListRegisterMetas(void)
{
	if( s_bRegisteri3ListMeta == FALSE)
	{
		i3List::RegisterMeta();
		i3StringList::RegisterMeta();
		i3BinList::RegisterMeta();
	}

	s_bRegisteri3ListMeta = TRUE;
}
