#if !defined( __I3_FIXED_POINT_H)
#define __I3_FIXED_POINT_H

#include "i3Debug.h"
/*
class FIXED32
{
protected:
	INT32 m_Value;

public:
	BEGIN_INLINE
	FIXED32(void)
	{
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32( INT32 val)		{	m_Value = (val << 16);	}
	END_INLINE

	BEGIN_INLINE
	FIXED32( UINT32 val)
	{
		m_Value = (val << 16);
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32( REAL32 val)
	{
		m_Value = (INT32)((val * 65536.0f) + 0.5f);
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32 &	operator = (FIXED32 & val)
	{
		m_Value = val.m_Value;

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32 &	operator = ( INT32 val)
	{
		m_Value = val << 16;

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32 &	operator = ( REAL32 val)
	{
		m_Value = (INT32)((val * 65536.0f) + 0.5f);

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	operator INT32(void)
	{
		return (m_Value >> 16);
	}
	END_INLINE

	BEGIN_INLINE
	operator REAL32(void)
	{
		return ((REAL32)m_Value) * 0.152587890625E-4f;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32 operator + (FIXED32 & val)
	{
		m_Value += val.m_Value;

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32	operator - (FIXED32 & val)
	{
		m_Value -= val.m_Value;

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32 operator * (FIXED32 & val)
	{
		m_Value = (m_Value >> 8) * (val.m_Value >> 8);

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32 operator / (FIXED32 & val)
	{
		m_Value = ((m_Value << 8) / (val.m_Value >> 8)); 

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32 operator % (FIXED32 &val)
	{
		register INT32 f2 = val.m_Value >> 8;
		m_Value = m_Value - ( (((m_Value << 8) / f2) >> 8) * f2);	// 정도를 유지하기 위한 방법

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32 & operator ++ (void)
	{
		m_Value += 65536;

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32	operator ++ (int)
	{
		FIXED32 val = *this;

		m_Value += 65536;

		return val;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32 & operator -- (void)
	{
		m_Value -= 65536;

		return *this;
	}
	END_INLINE

	BEGIN_INLINE
	FIXED32	operator -- (int)
	{
		FIXED32 val = *this;

		m_Value -= 65536;

		return val;
	}
	END_INLINE
};
*/

namespace i3FXD
{
	inline REAL32	ToReal( FIXED32 fixed)			{ return ((REAL32)fixed) * 0.152587890625E-4f;}
	inline FIXED32	FromReal( REAL32 real)			
	{ 
		I3ASSERT((real <= 32757.0f) && (real >= -32768.0f));

		return (FIXED32)((real * 65536.0f) + 0.5f); 
	}

	inline INT32	ToInt( FIXED32 fxd)			{ return (INT32)(fxd >> 16); }
	inline FIXED32	FromInt( INT32 val)				
	{ 
		// I3ASSERT((val <= 32757) && (val >= -32768));

		return val << 16; 
	}

	inline FIXED32	Add( FIXED32 f1, FIXED32 f2)	{ return f1 + f2; }
	inline FIXED32	Sub( FIXED32 f1, FIXED32 f2)	{ return f1 - f2; }
	inline FIXED32	Mul( FIXED32 f1, FIXED32 f2)	
	{ 
		//I3ASSERT( f1 < 0x01000000);
		//I3ASSERT( f2 < 0x01000000);
		//I3ASSERT( (f1 & 0xFFFFFF00) != 0);
		//I3ASSERT( (f2 & 0xFFFFFF00) != 0);

		return (f1 >> 8) * (f2 >> 8); 
		//return (FIXED32)((((INT64) f1) * f2) >> 16);
	}

	inline FIXED32	Div( FIXED32 f1, FIXED32 f2)	
	{ 
		//I3ASSERT( f1 < 0x00800000);
		//I3ASSERT( (f2 & 0xFFFFFF00) != 0);

		return (FIXED32)((f1 << 8) / (f2 >> 8)); 
	}

	inline FIXED32	Mod( FIXED32 f1, FIXED32 f2)	
	{ 
		INT32 temp1;
		FIXED32 temp2;

		temp1 = f1 / f2;
		temp2 = f2 * temp1;

		return f1 - temp2;
	}
};

#endif
