#ifndef __I3_TDK_PROPERTY_GRID_PROPERTY_STRING_H__
#define __I3_TDK_PROPERTY_GRID_PROPERTY_STRING_H__

#if defined( I3_WINDOWS)

#include "i3TDKPropertyGridProperty.h"

class I3_EXPORT_TDK i3TDKPropertyGridProperty_String : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty_String)

private:
	char*		m_pData;		// [initialize at constructor]
	INT32		m_nBufSize;		// [initialize at constructor]
	
protected:
	char*		GetStringData( void)					{	return m_pData;	}
	INT32		GetBufSize( void)						{	return m_nBufSize;	}

public:
	i3TDKPropertyGridProperty_String( PROPERTYINFO* pInfo);

	virtual void UpdateFromProperty( void* pData = nullptr);
	virtual void UpdateToProperty( void);
};

class I3_EXPORT_TDK i3TDKPropertyGridProperty_RcString : public i3TDKPropertyGridProperty
{
	DECLARE_DYNAMIC(i3TDKPropertyGridProperty_RcString)
private:
	i3::rc_string*	m_strData;			// 다른 프로퍼티와 마찬가지로 외부에 딱 fixed된 데이터가 들어오도록 포인터 처리..(유효성 보장되야함)
	
public:
	i3TDKPropertyGridProperty_RcString( PROPERTYINFO* pInfo);

	virtual void UpdateFromProperty( void* pData = nullptr);
	virtual void UpdateToProperty( void);
};


#endif

#endif