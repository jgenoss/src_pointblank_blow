#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_REAL32_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_REAL32_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_Real32 : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty_Real32)

private:
	REAL32*		m_pData;					// [initialize at constructor]

	//must be set by constructor
	REAL32		m_fMin = 0.0f;			
	REAL32		m_fMax = 0.0f;
	bool		m_bHasRange;				// [initialize at constructor]

protected:
	REAL32		_GetMinMaxData( REAL32 fData);
	
public:
	i3TDKPropertyGridProperty_Real32( PROPERTYINFO* pInfo);
	
	i3TDKPropertyGridProperty_Real32(	const CString& strName, const _variant_t& varValue,  DWORD_PTR dwData, LPCTSTR lpszDescr = nullptr, 
										bool bRange = false, REAL32 fMin = 0.f, REAL32 fMax = 0.f);
	
	virtual void UpdateFromProperty( void* pData = nullptr);
	virtual void UpdateToProperty( void);
};

#endif

#endif