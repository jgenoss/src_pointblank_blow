#include "stdafx.h"
#include "i3TDKPropertyGridProperty_Group.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_Group, i3TDKPropertyGridProperty)

i3TDKPropertyGridProperty_Group::i3TDKPropertyGridProperty_Group( PROPERTYINFO* pInfo)
:	i3TDKPropertyGridProperty( ToMFCString(pInfo->m_pszName))
{
}

void i3TDKPropertyGridProperty_Group::UpdateFromProperty( void* pData)
{

}

#endif