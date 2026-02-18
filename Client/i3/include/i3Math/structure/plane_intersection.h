#pragma once

namespace i3
{
	enum intersect_side;
	class aabb;
	struct plane;
	struct vec3;
	struct int3;

	namespace intersect
	{
		intersect_side aabb_plane(const aabb& a, const plane& pl);
		intersect_side aabb_plane_min_max(const vec3& v_min, const vec3& v_max, const plane& pl);
		intersect_side aabb_plane_center_extent(const vec3& center, const vec3& extent, const plane& pl);

		intersect_side aabb_plane_np_lookup(const aabb& a, const plane& pl, 
											const int3& n_vert, const int3& p_vert);

		// 교차 여부만 판정하는 경우 아래 bool 리턴 함수가 더 좋다.
		bool is_aabb_plane_negative_side(const aabb& a, const plane& pl);
		bool is_aabb_plane_negative_side_center_extent(const vec3& center, const vec3& extent, const plane& pl);
		bool is_aabb_plane_negative_side(const aabb& a, const plane& pl, const int3& p_vert);

		intersect_side triangle_plane(const vec3& v0, const vec3& v1, const vec3& v2, const plane& pl);		// 프러스텀 검사에 이것을 사용하자...
		bool is_triangle_plane_negative_side(const vec3& v0, const vec3& v1, const vec3& v2, const plane& pl);  //이것도 좋다...

		intersect_side quad_plane(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, const plane& pl);		// 프러스텀 검사에 이것을 사용하자...
		bool is_quad_plane_negative_side(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, const plane& pl);  


		// 아래 2개 함수는 plane의 갯수 대신 flag의 최상단비트위치를 배열크기로 간주하여 계산이 들어간다.
		// 따라서, 약간의 주의가 필요함..

		intersect_side	aabb_planes_branch(const aabb& a, const plane* array_plane, unsigned int& inout_flag);
		intersect_side	aabb_planes_leaf(const aabb& a, const plane* array_plane, unsigned int in_flag);
		intersect_side	aabb_planes_branch(const aabb& a, const plane* array_plane, const DWORD* lookup_array, unsigned int& inout_flag);
		intersect_side	aabb_planes_leaf(const aabb& a, const plane* array_plane, const DWORD* lookup_array, unsigned int in_flag);

		intersect_side  triangle_planes_leaf(const vec3& v0, const vec3& v1, const vec3& v2, const plane* array_plane, unsigned int in_flag);
		intersect_side  quad_planes_leaf(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, const plane* array_plane, unsigned int in_flag);
		
		bool is_aabb_planes_leaf_negative_side(const aabb& a, const plane* array_plane, unsigned int in_flag);
		bool is_aabb_planes_leaf_negative_side(const aabb& a, const plane* array_plane, const DWORD* lookup_array, unsigned int in_flag);
		bool is_triangle_planes_leaf_negative_side(const vec3& v0, const vec3& v1, const vec3& v2, const plane* array_plane, unsigned int in_flag);
		bool is_quad_planes_leaf_negative_side(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, const plane* array_plane, unsigned int in_flag);

	}

}

#include "plane_intersection.inl"