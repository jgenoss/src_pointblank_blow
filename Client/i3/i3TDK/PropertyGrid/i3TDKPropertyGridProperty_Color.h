#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_COLOR_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_COLOR_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_Color : public CMFCPropertyGridColorProperty
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_Color)

private:
	COLOR*			m_pData;			// Color [initialize at constructor]
	
protected:

public:
	i3TDKPropertyGridProperty_Color( PROPERTYINFO* pInfo);

	virtual void	UpdateFromProperty( void* pData = nullptr);
	virtual void	UpdateToProperty( void);
};

#endif

#endif