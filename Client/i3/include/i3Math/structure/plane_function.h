#pragma once

namespace i3
{
	struct plane;
	struct mat4;

	const vec3&		get_plane_normal(const plane& pl);
	vec3&			get_plane_normal(plane& pl);

	void		make_plane_from_point_normal(plane& pl, const vec3& p, const vec3& n);
	plane		make_plane_from_point_normal(const vec3& p, const vec3& n);

	void		make_plane_from_points(plane& pl, const vec3& v0, const vec3& v1, const vec3& v2);
	plane		make_plane_from_points(const vec3& v0, const vec3& v1, const vec3& v2);

	plane		plane_normal_normalize(const plane& pl);
	void		plane_normal_normalize(plane& dest, const plane& src);
	void		plane_normal_normalize_self(plane& pl);
	
	float		plane_normal_length_sq(const plane& pl);
	float		plane_normal_length(const plane& pl);

	float		plane_normal_abs_dot_vec3(const plane& pl, const vec3& v);
	float		plane_normal_dot_vec3(const plane& pl, const vec3& v);
	float		plane_dot_vec3(const plane& pl, const vec3& v);
	float		calc_plane_distance(const plane& pl, const vec3& v); // 위 함수와 같은 함수이다. (가독성을 위한 네이밍만 구분됨)

	// plane을 매트릭스로 변환처리...(미리 역행+전치 되어있어야함)
	void		plane_transform_inv_transpose_mat(plane& out, const plane& pl, const mat4& inv_transpose);
	plane		plane_transform_inv_transpose_mat(const plane& pl, const mat4& inv_transpose);
	void		plane_transform_inv_transpose_mat_self(plane& inout, const mat4& inv_transpose);

}

#include "plane_function.inl"
