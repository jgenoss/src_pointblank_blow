#if !defined( __REAL16_H)
#define __REAL16_H

#include "i3CommonType.h"

class I3_EXPORT_BASE REAL16
{
protected:
	UINT16 m_Value;

protected:
	static UINT16	F2H( REAL32 val);
	static REAL32	H2F( UINT16 val);

public:
	REAL16(void)
	{
		m_Value = 0;
	}

	REAL16( REAL32 val)
	{
		m_Value = F2H( val);
	}

	REAL16 & operator = ( const REAL16 & value)
	{
		m_Value = value.m_Value;

		return *this;
	}

	REAL16 & operator = (const UINT32 & value)
	{
		m_Value = F2H( (REAL32)value);

		return *this;
	}

	REAL16 & operator = (const INT32 & value)
	{
		m_Value = F2H( (REAL32)value);

		return *this;
	}

	REAL16 & operator = (const REAL32 & value)
	{
		m_Value = F2H( value);

		return *this;
	}

	operator const REAL32() const		{ return H2F( m_Value); }

friend inline bool operator == (const REAL16 & a, const REAL16 & b)
{
	return (a.m_Value == b.m_Value);
}

friend inline bool operator != (const REAL16 & a, const REAL16 & b)
{
	return (a.m_Value != b.m_Value);
}

friend inline bool operator > (const REAL16 & a, const REAL16 & b)
{
	REAL32 ra, rb;

	ra = REAL16::H2F( a.m_Value);
	rb = REAL16::H2F( b.m_Value);
	return ra > rb;
}

friend inline bool operator >= (const REAL16 & a, const REAL16 & b)
{
	REAL32 ra, rb;

	ra = H2F( a.m_Value);
	rb = H2F( b.m_Value);

	return ra >= rb;
}

friend inline bool operator < (const REAL16 & a, const REAL16 & b)
{
	REAL32 ra, rb;

	ra = H2F( a.m_Value);
	rb = H2F( b.m_Value);

	return ra < rb;
}

friend inline bool operator <= (const REAL16 & a, const REAL16 & b)
{
	REAL32 ra, rb;

	ra = H2F( a.m_Value);
	rb = H2F( b.m_Value);

	return ra <= rb;
}
};

#endif