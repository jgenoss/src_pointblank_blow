#pragma once

//#if 0
#include <windef.h>		// for UINT type

#pragma warning(push)
#pragma warning(disable : 4201)

namespace i3
{
	// 16바이트 정렬 확인 함수..
	inline bool is_aligned_16byte(const void* p) { return ( size_t(p) & 0x0F ) == 0; }


	struct mat4
	{
		union
		{
			struct { float _11,_12,_13,_14,_21,_22,_23,_24,_31,_32,_33,_34,_41,_42,_43,_44; };
			float  m[4][4];
		};
		
		mat4() {};
		mat4(const float* f);
		mat4(const mat4& m);
		mat4(float _11, float _12, float _13, float _14, 
			 float _21, float _22, float _23, float _24, 
			 float _31, float _32, float _33, float _34,
			 float _41, float _42, float _43, float _44);

		float& operator()( UINT row, UINT col);
		float  operator()( UINT row, UINT col) const;
		operator float* ();
		operator const float* () const;

		mat4& operator *= ( const mat4& m);
		mat4& operator += ( const mat4& m);
		mat4& operator -= ( const mat4& m);
		mat4& operator *= ( float f);
		mat4& operator /= ( float f);

		mat4 operator + () const;
		mat4 operator - () const;

		mat4 operator * ( const mat4& m) const;
		mat4 operator + ( const mat4& m) const;
		mat4 operator - ( const mat4& m) const;
		mat4 operator * ( float f) const;
		mat4 operator / ( float f) const;

		friend mat4 operator * ( float f, const mat4& m);

		bool operator == ( const mat4& m) const;
		bool operator != ( const mat4& m) const;
	};
	
	struct __declspec(align(16)) mat4a : mat4
	{
		mat4a() {};
		mat4a(const float* f);
		mat4a(const mat4& m);
		mat4a(float _11, float _12, float _13, float _14, 
			float _21, float _22, float _23, float _24, 
			float _31, float _32, float _33, float _34,
			float _41, float _42, float _43, float _44);

		void* operator new   ( size_t );
		void* operator new[] ( size_t );
		void operator delete   ( void* );   
		void operator delete[] ( void* );   

		mat4a& operator=(const mat4& );
	};

}

#include "mat4.inl"

#pragma warning(pop)

//#endif