#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_GROUP_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_GROUP_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_Group : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty_Group)

	i3TDKPropertyGridProperty_Group( PROPERTYINFO* pInfo);
	
	virtual void UpdateFromProperty( void* pData = nullptr);
};

#endif

#endif