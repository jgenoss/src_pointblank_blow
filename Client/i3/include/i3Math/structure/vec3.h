#pragma once

//#if 0

namespace i3
{

	
	struct vec3
	{
		float	x,y,z;
		
		vec3() {}		// 초기화 생략(의도적)
		vec3(float x, float y, float z) : x(x),y(y),z(z) {}
		vec3(const float* pf) : x(pf[0]), y(pf[1]), z(pf[2]) {}
		vec3(const vec3& v) : x(v.x), y(v.y), z(v.z) {}
		operator float* () { return &x; }	// 이 2개의 암시변환 연산자로 []접근이 가능함.
		operator const float* () const { return &x; }	

		vec3 operator -();
		vec3 operator +();

		vec3& operator=(const vec3& v);

		vec3& operator+=(const vec3& v);
		vec3& operator-=(const vec3& v);
		vec3& operator*=(const float f);
		vec3& operator/=(const float f);

		void  set(float x, float y, float z);
	};

	vec3 operator + ( const vec3& lhs, const vec3& rhs);
	vec3 operator - ( const vec3& lhs, const vec3& rhs);
	vec3 operator * ( const vec3& lhs, float rhs);
	vec3 operator / ( const vec3& lhs, float rhs);
	vec3 operator * ( float lhs, const vec3& rhs);




	
	
}

#include "vec3.inl"

//#endif
