#pragma once

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_UINT32_BitFlag : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_UINT32_BitFlag)

private:
	UINT32*		m_pData;			// [initialize at constructor]
	UINT32		m_bitMask;			// [initialize at constructor]
	bool		m_bNoEdit;			// [initialize at constructor]

public:
	i3TDKPropertyGridProperty_UINT32_BitFlag( PROPERTYINFO* pInfo);

	virtual void	UpdateFromProperty( void* pData = nullptr);
	virtual void	UpdateToProperty( void);
};

#endif
