
#include "vec3.h"
#include <math.h>

namespace i3
{
	namespace detail
	{
		const float vec3_normalize_epsilon = 1e-06f;
	}

	inline float vec3_dot(const vec3& v1, const vec3& v2)
	{
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}

	inline void	 vec3_cross(vec3& out, const vec3& v1, const vec3& v2)
	{
		out = vec3(	v1.y * v2.z - v1.z * v2.y,
					v1.z * v2.x - v1.x * v2.z,
					v1.x * v2.y - v1.y * v2.x);
	}

	inline vec3 vec3_cross(const vec3& v1, const vec3& v2)
	{
		return vec3(	v1.y * v2.z - v1.z * v2.y,
					v1.z * v2.x - v1.x * v2.z,
					v1.x * v2.y - v1.y * v2.x);
	}
	
	inline vec3 vec3_normalize(const vec3& v)
	{
		float l = vec3_length(v);

		if (l <= detail::vec3_normalize_epsilon)
			return vec3(0.f, 0.f, 0.f);
		
		l = 1.f / l;
		return vec3(v.x * l, v.y * l, v.z * l);
	}

	inline void	 vec3_normalize(vec3& dest, const vec3& src)
	{
		float l = vec3_length(src);
		
		if (l <= detail::vec3_normalize_epsilon)
		{
			dest.x = 0.f;	dest.y = 0.f; dest.z = 0.f;
		}
		else
		{
			dest = src;	dest *= 1.f / l;
		}
	}

	inline void	 vec3_normalize_self(vec3& v)
	{
		float l = vec3_length(v);

		if (l <= detail::vec3_normalize_epsilon)
		{
			v.x = 0.f;	v.y = 0.f; v.z = 0.f;
		}
		else
		{
			v *= 1.f / l;
		}
	}

	inline float vec3_length_sq(const vec3& v)
	{
		return v.x*v.x + v.y*v.y + v.z*v.z;
	}

	inline float vec3_length(const vec3& v)
	{
		return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	}

}