
#include "matrix_multiply.h"

namespace i3
{
	inline mat4::mat4(const float* f) { ::memcpy(&_11, f, sizeof(mat4)); }

	inline mat4::mat4(const mat4& m) { ::memcpy(&_11, &m, sizeof(mat4)); }

	inline mat4::mat4(float _11, float _12, float _13, float _14, 
		float _21, float _22, float _23, float _24, 
		float _31, float _32, float _33, float _34,
		float _41, float _42, float _43, float _44) : 
		_11(_11), _12(_12), _13(_13), _14(_14),
		_21(_21), _22(_22), _23(_23), _24(_24),
		_31(_31), _32(_32), _33(_33), _34(_34),
		_41(_41), _42(_42), _43(_43), _44(_44) {}

	inline 	float& mat4::operator()( UINT row, UINT col) { return m[row][col]; }
	inline 	float  mat4::operator()( UINT row, UINT col) const { return m[row][col]; }

	inline 	mat4::operator float* () { return (float*)&_11; }
	inline 	mat4::operator const float* () const { return (const float*)&_11; }

	inline mat4& mat4::operator *= ( const mat4& rhs) 
	{
		matrix_multiply(*this, rhs, *this);
		return *this;
	}

	inline mat4& mat4::operator += ( const mat4& m)
	{
		_11 += m._11; _12 += m._12; _13 += m._13; _14 += m._14;
		_21 += m._21; _22 += m._22; _23 += m._23; _24 += m._24;
		_31 += m._31; _32 += m._32; _33 += m._33; _34 += m._34;
		_41 += m._41; _42 += m._42; _43 += m._43; _44 += m._44;
		return *this;
	}

	inline mat4& mat4::operator -= ( const mat4& m)
	{
		_11 -= m._11; _12 -= m._12; _13 -= m._13; _14 -= m._14;
		_21 -= m._21; _22 -= m._22; _23 -= m._23; _24 -= m._24;
		_31 -= m._31; _32 -= m._32; _33 -= m._33; _34 -= m._34;
		_41 -= m._41; _42 -= m._42; _43 -= m._43; _44 -= m._44;
		return *this;
	}

	inline mat4& mat4::operator *= ( float f)
	{
		_11 *= f; _12 *= f; _13 *= f; _14 *= f;
		_21 *= f; _22 *= f; _23 *= f; _24 *= f;
		_31 *= f; _32 *= f; _33 *= f; _34 *= f;
		_41 *= f; _42 *= f; _43 *= f; _44 *= f;
		return *this;
	}

	inline mat4& mat4::operator /= ( float f)
	{
		float fInv = 1.0f / f;
		_11 *= fInv; _12 *= fInv; _13 *= fInv; _14 *= fInv;
		_21 *= fInv; _22 *= fInv; _23 *= fInv; _24 *= fInv;
		_31 *= fInv; _32 *= fInv; _33 *= fInv; _34 *= fInv;
		_41 *= fInv; _42 *= fInv; _43 *= fInv; _44 *= fInv;
		return *this;
	}

	inline mat4 mat4::operator + () const { return *this; }
	inline mat4 mat4::operator - () const 
	{
		return mat4(-_11, -_12, -_13, -_14,
					-_21, -_22, -_23, -_24,
					-_31, -_32, -_33, -_34,
					-_41, -_42, -_43, -_44);
	}

	inline mat4 mat4::operator * ( const mat4& m) const
	{
		mat4 out;
		matrix_multiply(*this, m, out);
		return out;
	}

	inline mat4 mat4::operator + ( const mat4& m) const
	{
		return mat4(_11 + m._11, _12 + m._12, _13 + m._13, _14 + m._14,
					_21 + m._21, _22 + m._22, _23 + m._23, _24 + m._24,
					_31 + m._31, _32 + m._32, _33 + m._33, _34 + m._34,
					_41 + m._41, _42 + m._42, _43 + m._43, _44 + m._44);
	}

	inline mat4 mat4::operator - ( const mat4& m) const
	{
		return mat4(_11 - m._11, _12 - m._12, _13 - m._13, _14 - m._14,
					_21 - m._21, _22 - m._22, _23 - m._23, _24 - m._24,
					_31 - m._31, _32 - m._32, _33 - m._33, _34 - m._34,
					_41 - m._41, _42 - m._42, _43 - m._43, _44 - m._44);
	}

	inline mat4 mat4::operator * ( float f ) const
	{
		return mat4(_11 * f, _12 * f, _13 * f, _14 * f,
					_21 * f, _22 * f, _23 * f, _24 * f,
					_31 * f, _32 * f, _33 * f, _34 * f,
					_41 * f, _42 * f, _43 * f, _44 * f);
	}

	inline mat4 mat4::operator / ( float f ) const
	{
		FLOAT inv = 1.0f / f;
		return mat4(_11 * inv, _12 * inv, _13 * inv, _14 * inv,
					_21 * inv, _22 * inv, _23 * inv, _24 * inv,
					_31 * inv, _32 * inv, _33 * inv, _34 * inv,
					_41 * inv, _42 * inv, _43 * inv, _44 * inv);
	}

	inline mat4 operator * ( float f, const mat4& m)
	{
		return mat4(f * m._11, f * m._12, f * m._13, f * m._14,
					f * m._21, f * m._22, f * m._23, f * m._24,
					f * m._31, f * m._32, f * m._33, f * m._34,
					f * m._41, f * m._42, f * m._43, f * m._44);
	}

	inline bool mat4::operator == ( const mat4& m) const
	{
		return 0 == memcmp(this, &m, sizeof(mat4));
	}

	inline bool mat4::operator != ( const mat4& m) const
	{
		return 0 != memcmp(this, &m, sizeof(mat4));
	}

////
	inline mat4a::mat4a(const float* f) : mat4(f) {}
	inline mat4a::mat4a(const mat4& m) : mat4(m) {}

	inline mat4a::mat4a(float _11, float _12, float _13, float _14, 
				float _21, float _22, float _23, float _24, 
				float _31, float _32, float _33, float _34,
				float _41, float _42, float _43, float _44) : 
				mat4(_11,_12,_13,_14,
					_21,_22,_23,_24,
					_31,_32,_33,_34,
					_41,_42,_43,_44) {}

	// i3::static_pool은 타입에 따라 최대 32비트 정렬을 보장함..
	inline void* mat4a::operator new   ( size_t s )
	{
		return i3::static_pool_malloc(s);
	}

	inline void* mat4a::operator new[] ( size_t s)
	{
		return i3::static_pool_malloc(s);
	}

	inline void mat4a::operator delete   ( void* p)   
	{
		i3::static_pool_free(p);
	}

	inline void mat4a::operator delete[] ( void* p)
	{
		i3::static_pool_free(p);
	}
	
	inline mat4a& mat4a::operator=(const mat4& rhs)
	{
		memcpy(&_11, &rhs, sizeof(mat4));
		return *this;
	}

}
