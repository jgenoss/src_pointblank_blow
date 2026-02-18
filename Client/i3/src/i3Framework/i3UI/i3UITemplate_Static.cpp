#include "i3FrameworkPCH.h"
#include "i3UITemplate_Static.h"

const char * s_pszStaticInfoString[I3UI_STATIC_SHAPE_COUNT] = 
{
	"0: Normal",
	"4: Disable"
};

I3_CLASS_INSTANCE( i3UITemplate_Static, i3UITemplate);

i3UITemplate_Static::i3UITemplate_Static()
{
}

i3UITemplate_Static::~i3UITemplate_Static()
{
}

const char * i3UITemplate_Static::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_STATIC_SHAPE_COUNT);

	return s_pszStaticInfoString[nIndex];
}

const char * i3UITemplate_Static::getComment(void)
{
	return "Static";
}
