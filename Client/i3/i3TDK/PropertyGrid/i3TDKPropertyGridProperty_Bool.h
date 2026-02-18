#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_BOOL_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_BOOL_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_Bool : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_Bool)

private:
	bool*		m_pData = nullptr;
	bool		m_bNoEdit;			// [initialize at constructor]

public:
	i3TDKPropertyGridProperty_Bool( PROPERTYINFO* pInfo);
	
	virtual void	UpdateFromProperty( void* pData = nullptr);
	virtual void	UpdateToProperty( void);
};

#endif

#endif