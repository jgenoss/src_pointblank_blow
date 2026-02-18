#pragma once


namespace i3
{
	struct vec3;

	float		vec3_dot(const vec3& v1, const vec3& v2);

	void		vec3_cross(vec3& out, const vec3& v1, const vec3& v2);
	vec3		vec3_cross(const vec3& v1, const vec3& v2);

	vec3		vec3_normalize(const vec3& v);
	void		vec3_normalize(vec3& dest, const vec3& src);
	void		vec3_normalize_self(vec3& v);

	float		vec3_length_sq(const vec3& v);
	float		vec3_length(const vec3& v);

}

#include "vec3_function.inl"
