#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_COMBO_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_COMBO_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

// Combo의 인덱스를 연결시켜 놓는다. 
class I3_EXPORT_TDK i3TDKPropertyGridProperty_Combo : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_Combo)

private:
	INT32*			m_pData;			// Combo index [initialize at constructor]
	const i3::vector<i3::rc_string>&	m_OptionList;		// Combo items

protected:
	INT32			_getIndex( const char* pszOption);

public:
	i3TDKPropertyGridProperty_Combo( PROPERTYINFO* pInfo);
	
	virtual void	UpdateFromProperty( void* pData = nullptr);

};

#endif

#endif