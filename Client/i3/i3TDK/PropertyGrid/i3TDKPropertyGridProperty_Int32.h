#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_INT32_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_INT32_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_Int32 : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty_Int32)

private:
	INT32*		m_pData;				// [initialize at constructor]

	INT32		m_nMin = 0;
	INT32		m_nMax = 100;
	bool		m_bHasRange;			// [initialize at constructor]
	bool		m_bNoEdit;				// [initialize at constructor]

protected:
	INT32		_GetMinMaxData( INT32 nData);
	
public:
	i3TDKPropertyGridProperty_Int32( PROPERTYINFO* pInfo);
	virtual ~i3TDKPropertyGridProperty_Int32();
	
	virtual void	UpdateFromProperty( void* pData = nullptr);
	virtual void	UpdateToProperty( void);
};

#endif

#endif