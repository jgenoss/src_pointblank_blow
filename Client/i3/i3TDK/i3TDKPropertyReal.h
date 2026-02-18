#if !defined( __I3_TDK_PROPERTY_REAL_H)
#define __I3_TDK_PROPERTY_REAL_H

#include "i3Base.h"
#include "i3TDKProperty.h"
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3TDKPropertyReal : public i3TDKProperty
{
	I3_CLASS_DEFINE( i3TDKPropertyReal, i3TDKProperty);
protected:
	REAL32		m_Value = 0.0f;

public:
	i3TDKPropertyReal(void);
	virtual ~i3TDKPropertyReal(void) {}

	void		SetValue( REAL32 value)					{	m_Value = value; }
	REAL32		GetValue(void)							{ return m_Value; }

	// 문자열 형으로 자신의 값을 표현
	virtual void Get( char * pszValueStr);

	// 문자열 형으로 주어진 사용자의 값을 자신의 값에 적용시킨다.
	virtual void Set( const char * pszValueStr);
	
	virtual void	CopyFrom( i3TDKPropertyContainer * pOther, BOOL bRef = TRUE);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif