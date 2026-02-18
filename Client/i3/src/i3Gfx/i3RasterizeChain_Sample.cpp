#include "i3GfxType.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3RasterizeChain_Sample.h"

I3_CLASS_INSTANCE( i3RasterizeChain_Sample);

INT32	i3RasterizeChain_Sample::GetInputSpec( I3_CHAIN_INPUT_SPEC * pSpec)
{
	pSpec->setToDefault();

	return 1;
}


