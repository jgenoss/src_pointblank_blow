#if !defined( __UINT128_H)
#define __UINT128_H

#include "i3CommonType.h"

class I3_EXPORT_BASE UINT128
{
protected:
	UINT64	v[2];

public:
	UINT128(void)
	{
	}

	UINT128( UINT32 a, UINT32 b, UINT32 c, UINT32 d)
	{
		v[0] = ((UINT64)c << 32) | ((UINT64)d & 0xFFFFFFFF);
		v[1] = ((UINT64)a << 32) | ((UINT64)b & 0xFFFFFFFF);
	}

	UINT128( UINT64 a, UINT64 b)
	{
		v[1] = a;
		v[0] = b;
	}

	UINT128 & operator = ( const UINT128 & value)
	{
		v[0] = value.v[0];
		v[1] = value.v[1];

		return *this;
	}

	UINT128 & operator = (const UINT64 & value)
	{
		v[0] = value;
		v[1] = 0;

		return *this;
	}

	UINT128 & operator = (const INT64 & value)
	{
		v[0] = (UINT64) value;
		v[1] = 0;

		return *this;
	}

	UINT128 & operator = (const UINT32 & value)
	{
		v[0] = value;
		v[1] = 0;

		return *this;
	}

	UINT128 & operator = (const INT32 & value)
	{
		v[0] = (UINT64) value;
		v[1] = 0;

		return *this;
	}

	const UINT64 operator [] (INT32 index) const
	{
		return v[index];
	}
};

inline bool operator == (const UINT128 & a, const UINT128 & b)
{
	return (a[1] == b[1]) && (a[0] == b[0]);
}

inline bool operator != (const UINT128 & a, const UINT128 & b)
{
	return (a[1] != b[1]) || (a[0] != b[0]);
}

inline bool operator > (const UINT128 & a, const UINT128 & b)
{
	return (a[1] > b[1]) || ((a[1] == b[1]) && (a[0] > b[0]));
}

inline bool operator >= (const UINT128 & a, const UINT128 & b)
{
	return (a[1] >= b[1]) || ((a[1] == b[1]) && (a[0] >= b[0]));
}

inline bool operator < (const UINT128 & a, const UINT128 & b)
{
	return (a[1] < b[1]) || ((a[1] == b[1]) && (a[0] < b[0]));
}

inline bool operator <= (const UINT128 & a, const UINT128 & b)
{
	return (a[1] <= b[1]) || ((a[1] == b[1]) && (a[0] <= b[0]));
}


#endif