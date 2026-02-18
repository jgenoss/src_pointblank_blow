#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_VEC3D_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_VEC3D_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"
#include "i3TDKPropertyGridProperty_Real32.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_Vec3D : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty_Vec3D)

private:
	VEC3D*		m_pData;				// [initialize at constructor]
	
	bool		m_bHasRange;			// [initialize at constructor]
	REAL32		m_fMin = 0.0f;
	REAL32		m_fMax = 0.0f;

	i3TDKPropertyGridProperty_Real32*		m_pX;	// [initialize at constructor]
	i3TDKPropertyGridProperty_Real32*		m_pY;	// [initialize at constructor]
	i3TDKPropertyGridProperty_Real32*		m_pZ;	// [initialize at constructor]
	
public:
	i3TDKPropertyGridProperty_Vec3D( PROPERTYINFO* pInfo);

	virtual BOOL OnUpdateValue();			//From CMFCPropertyGridProprety
	
	virtual void UpdateFromProperty( void* pData = nullptr);		//From i3TDKPropertyGridProperty
};

#endif

#endif