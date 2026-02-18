#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_FONT_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_FONT_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_Font : public CMFCPropertyGridFontProperty
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_Font)

private:
	LPLOGFONTA	m_pData;				// [initialize at constructor]
	COLOR*		m_pUserData;			// [initialize at constructor]
	
protected:

public:
	i3TDKPropertyGridProperty_Font( PROPERTYINFO* pInfo);

	virtual void	UpdateFromProperty( void* pData = nullptr, void* pUserData = nullptr);
	virtual void	UpdateToProperty( void);
};

#endif

#endif