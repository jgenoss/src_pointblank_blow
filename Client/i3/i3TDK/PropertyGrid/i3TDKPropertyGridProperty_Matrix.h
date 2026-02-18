#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_MATRIX_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_MATRIX_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"
#include "i3TDKPropertyGridProperty_Vec4D.h"

//4x4 Matrix
class I3_EXPORT_TDK i3TDKPropertyGridProperty_Matrix : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_Matrix)

private:
	MATRIX*		m_pData;				// [initialize at constructor]
	
	bool		m_bHasRange;			// [initialize at constructor]
	REAL32		m_fMin = 0.0f;
	REAL32		m_fMax = 0.0f;

	i3TDKPropertyGridProperty_Vec4D*		m_pRow1;		// [initialize at constructor]
	i3TDKPropertyGridProperty_Vec4D*		m_pRow2;		// [initialize at constructor]
	i3TDKPropertyGridProperty_Vec4D*		m_pRow3;		// [initialize at constructor]
	i3TDKPropertyGridProperty_Vec4D*		m_pRow4;		// [initialize at constructor]
	
public:
	i3TDKPropertyGridProperty_Matrix( PROPERTYINFO* pInfo);

	virtual void UpdateFromProperty( void* pData = nullptr);
};

#endif

#endif