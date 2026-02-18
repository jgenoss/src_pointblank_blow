#include "i3MathType.h"

//#if 0
#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

#include "octree/word_aabb.h"
#include "structure/vec3.h"
#include "structure/aabb.h"

namespace i3
{
	namespace
	{
		const float	aabb_to_word_aabb_epsilon = 1e-04f;		// 부정확하면 수치를 줄일 것!..
	}

	void calc_world_aabb_scale_offset(const i3::aabb& world_aabb, vec3& out_scale, vec3& out_offset)
	{
		vec3 len = world_aabb.calc_length();
		out_scale.x = 65536.f / len.x;	out_scale.y = 65536.f / len.y;	out_scale.z = 65536.f / len.z;
		out_offset = world_aabb.min();	out_offset *= -1.f;

	}

	bool convert_aabb_to_word_aabb(const i3::aabb& input, const vec3& scale, const vec3& offset,
									word_aabb& out )
	{
		out = 0;

		i3::aabb a(input);
		a += offset;					// 중심에 놓고..
		a *= scale;						// 0 ~ 65535까지 사상..

		if ( a.min().x < 0.f)		
		{	
			if ( a.min().x < -aabb_to_word_aabb_epsilon) 
				return false;	
			a.min().x = 0.f;	
		}
		else
		if ( a.min().x > 65535.f )	
		{	
			a.min().x = 65535.f;   
		}

		if ( a.min().y < 0.f )		
		{	
			if ( a.min().y < -aabb_to_word_aabb_epsilon) 
				return false;	
			a.min().y = 0.f;	
		}
		else
		if ( a.min().y > 65535.f )	
		{	
			a.min().y = 65535.f;	
		}

		if ( a.min().z < 0.f)	
		{	
			if (a.min().z < -aabb_to_word_aabb_epsilon) 
				return false;	
			a.min().z = 0.f;	
		}
		else
		if ( a.min().z > 65535.f)		
		{	
			a.min().z = 65535.f;	
		}
					/////////

		if ( a.max().x < a.min().x )	
		{	
			a.max().x = a.min().x;	
		}
		else
		if ( a.max().x > 65535.f )	
		{	
			if ( a.max().x > 65536.f + aabb_to_word_aabb_epsilon ) 
				return false;	
			a.max().x = 65535.f;	
		}

		if ( a.max().y < a.min().y )	
		{	
			a.max().y = a.min().y;	
		}
		else
		if ( a.max().y > 65535.f )	
		{	
			if ( a.max().y > 65536.f + aabb_to_word_aabb_epsilon ) 
				return false;		
			a.max().y = 65535.f;	
		}

		if ( a.max().z < a.min().z )	
		{	
			a.max().z = a.min().z;	
		}
		else
		if ( a.max().z > 65535.f )	
		{	
			if ( a.max().z > 65536.f + aabb_to_word_aabb_epsilon) 
				return false;	
			a.max().z = 65535.f;	
		}

		/////////		
		out.x0 = WORD(a.min().x);		// floor처리가 아니더라도 충분이 소수점은 truncate된다..
		out.y0 = WORD(a.min().y);		// float to int의 경우, 최신 CPU에서는 SSE최적화옵션에 의해, 중간성능이 보장됨..
		out.z0 = WORD(a.min().z);
		out.x1 = WORD(a.max().x);
		out.y1 = WORD(a.max().y);
		out.z1 = WORD(a.max().z);
		
	//	out.lv_info = calc_lv_info(out.x0, out.y0, out.z0, out.x1, out.y1, out.z1);
		
		// 중요한점..오브젝트는 region범위안에는 되도록 포함되야하는데.
		// 실제로 그렇지 않다면, 다른 방법을 생각해야한다....
		return true;
	}
	
}


#pragma pop_macro("max")
#pragma pop_macro("min")


//#endif