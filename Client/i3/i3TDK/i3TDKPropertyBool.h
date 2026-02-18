#if !defined( __I3_TDK_PROPERTY_BOOL_H)
#define __I3_TDK_PROPERTY_BOOL_H

#include "i3Base.h"
#include "i3TDKProperty.h"
// 익스포트추가->다시 제거. (12.09.19.수빈)

class i3TDKPropertyBool : public i3TDKProperty
{
	I3_CLASS_DEFINE( i3TDKPropertyBool, i3TDKProperty);
protected:
	BOOL		m_Value = FALSE;

public:
	i3TDKPropertyBool(void);
	virtual ~i3TDKPropertyBool(void) {}

	void		SetValue( BOOL value)					{	m_Value = value; }
	BOOL		GetValue(void)							{ return m_Value; }

	// 문자열 형으로 자신의 값을 표현
	virtual void Get( char * pszValueStr, const INT32 nValueStrMax);

	// 문자열 형으로 주어진 사용자의 값을 자신의 값에 적용시킨다.
	virtual void Set( const char * pszValueStr);

	virtual void	GetValueList( i3::vector<i3::rc_string>& List);
	virtual void	CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef = TRUE);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif