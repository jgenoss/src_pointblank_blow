#include "i3MathType.h"

//#if 0

#include "structure/matrix_multiply.h"
#include "structure/mat4.h"
#include <xmmintrin.h>
//
// 발췌 및 수정 출처 : http://fhtr.blogspot.kr/2010/02/4x4-float-matrix-multiplication-using.html
//

namespace i3
{

	struct vec4_sse
	{
		__m128 xmm;

		vec4_sse (const __m128& v) : xmm (v) {}
		vec4_sse (float v) { xmm = _mm_set1_ps(v); }
		vec4_sse (float x, float y, float z, float w) { xmm = _mm_set_ps(w,z,y,x); }	// 메모리배치가 LE이므로 거꾸로 들어가게 된다.
		
		vec4_sse (const float *v) { xmm = _mm_load_ps(v); }
		vec4_sse operator* (const vec4_sse &v) const { return vec4_sse(_mm_mul_ps(xmm, v.xmm)); }
		vec4_sse operator+ (const vec4_sse &v) const { return vec4_sse(_mm_add_ps(xmm, v.xmm)); }
		vec4_sse operator- (const vec4_sse &v) const { return vec4_sse(_mm_sub_ps(xmm, v.xmm)); }
		vec4_sse operator/ (const vec4_sse &v) const { return vec4_sse(_mm_div_ps(xmm, v.xmm)); }
		
		void operator=(float v) { xmm = _mm_set1_ps(v); }
		void operator*= (const vec4_sse &v)	{ xmm = _mm_mul_ps(xmm, v.xmm); }
		void operator+= (const vec4_sse &v) { xmm = _mm_add_ps(xmm, v.xmm); }
		void operator-= (const vec4_sse &v) { xmm = _mm_sub_ps(xmm, v.xmm); }
		void operator/= (const vec4_sse &v) { xmm = _mm_div_ps(xmm, v.xmm); }

		void operator>> (float *v) { _mm_store_ps(v, xmm); }
	};
	
	inline void vec4_sse_multiply( const vec4_sse& lhs, const vec4_sse& rhs, vec4_sse& out)
	{
		out.xmm = _mm_mul_ps(lhs.xmm, rhs.xmm);
	}
	
	inline void vec4_sse_add(const vec4_sse& lhs, const vec4_sse& rhs, vec4_sse& out)
	{
		out.xmm = _mm_add_ps(lhs.xmm, rhs.xmm);
	}

	inline void vec4_sse_sub(const vec4_sse& lhs, const vec4_sse& rhs, vec4_sse& out)
	{
		out.xmm = _mm_sub_ps(lhs.xmm, rhs.xmm);
	}
	
	inline void vec4_sse_div(const vec4_sse& lhs, const vec4_sse& rhs, vec4_sse& out)
	{
		out.xmm = _mm_div_ps(lhs.xmm, rhs.xmm);
	}

	void matrix_multiply(const mat4a& lhs, const mat4a& rhs, mat4& out)
	{
		const vec4_sse* vec_rhs = reinterpret_cast<const vec4_sse*>(&rhs);
		
		mat4a res;
		vec4_sse* res128 = reinterpret_cast<vec4_sse*>(&res);
		
		for ( int i = 0 ; i < 4 ; ++i )
		{
			vec4_sse& res = res128[i];

			res = lhs.m[i][0];		res *= vec_rhs[0];
			res += vec4_sse(lhs.m[i][1]) * vec_rhs[1];
			res += vec4_sse(lhs.m[i][2]) * vec_rhs[2];
			res += vec4_sse(lhs.m[i][3]) * vec_rhs[3];
		}

		out = res;
	}

	void matrix_multiply(const mat4& lhs, const mat4& rhs, mat4& out)
	{
		BYTE aligned_flag = 0;
		aligned_flag |=	( is_aligned_16byte(&lhs) ) ? 0x01 : 0;
		aligned_flag |= ( is_aligned_16byte(&rhs) ) ? 0x02 : 0;
	
		if (aligned_flag == 0x03)	// 입력인수 모두 정렬됨..
		{
			matrix_multiply(static_cast<const mat4a&>(lhs), 
							static_cast<const mat4a&>(rhs), out);
			return;
		}

		// 그 외에는 그냥 일반 계산..
		mat4 res;

		for (int i = 0; i< 4; ++i)
			for (int j = 0 ; j < 4 ; ++j)
				res.m[i][j] =	lhs.m[i][0] * rhs.m[0][j] + lhs.m[i][1] * rhs.m[1][j] + 
								lhs.m[i][2] * rhs.m[2][j] + lhs.m[i][3] * rhs.m[3][j];
		out = res;
	}


}

//#endif
