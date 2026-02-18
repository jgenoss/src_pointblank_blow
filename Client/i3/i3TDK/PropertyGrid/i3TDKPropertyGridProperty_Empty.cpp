#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Empty.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Empty, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_Empty::i3TDKPropertyGridProperty_Empty()
:	i3TDKPropertyGridProperty( _T(""), _T(""), 0, 0)
{
}

void i3TDKPropertyGridProperty_Empty::UpdateFromProperty( void* pData)
{

}

#endif