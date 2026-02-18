#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_VEC4D_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_VEC4D_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"
#include "i3TDKPropertyGridProperty_Real32.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_Vec4D : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty_Vec4D)

private:
	VEC4D*		m_pData;			// [initialize at constructor]
	
	bool		m_bHasRange;		// [initialize at constructor]
	REAL32		m_fMin = 0.0f;
	REAL32		m_fMax = 0.0f;

	i3TDKPropertyGridProperty_Real32*		m_pX;		// [initialize at constructor]
	i3TDKPropertyGridProperty_Real32*		m_pY;		// [initialize at constructor]
	i3TDKPropertyGridProperty_Real32*		m_pZ;		// [initialize at constructor]
	i3TDKPropertyGridProperty_Real32*		m_pW;		// [initialize at constructor]
	
public:
	i3TDKPropertyGridProperty_Vec4D( PROPERTYINFO* pInfo);

	virtual BOOL OnUpdateValue();

	virtual void UpdateFromProperty( void* pData = nullptr);
};

#endif

#endif