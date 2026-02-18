#pragma once

namespace i3
{
	enum intersect_side;
	class frustum_plane;
	class aabb;
	struct plane;
	struct int3;
	struct vec3;

	namespace intersect
	{
		intersect_side frustum_aabb(const frustum_plane& fr, const aabb& a);
		intersect_side frustum_triangle(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2);

		bool is_frustum_aabb_negative_side(const frustum_plane& fr, const aabb& a);
		bool is_frustum_triangle_negative_side(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2);


		// 다음은 노드/옥트리 등에 쓰이는 flag와 coherency_idx를 섞어서 쓰는 형태이다..
		// branch함수의 경우 자식존재하는 부모 노드에 쓰이고, leaf함수는 말단 노드에 쓰면 된다.

		// 여기서 플래그는 비트플래그이고, 각 비트가 plane하나에 매칭...
		// 1. flag값은 최초에는 모두 채워진 상태이며, 노드를 타고들어갈때, 평면에 걸치지 않은 경우(POSITIVE,NEGATIVE)의 경우
		//    해당 비트가 지워진다..  (지워진쪽 plane은 자식노드에서 더이상 계산에 참여하지 않는다..)
		// 2. coherency_idx는 이전 프레임에서 NEGATIVE판정이 된 plane_idx를 보관한 것으로 우선적으로 먼저 검사하여
		//    다시 NEGATIVE이면 빠르게 false를 리턴하도록 만들어진 것이다.


		intersect_side frustum_aabb_branch(const frustum_plane& fr, const aabb& a,
			unsigned int& inout_flag, unsigned int& inout_coherency);
		intersect_side frustum_aabb_branch(const frustum_plane& fr, const aabb& a, unsigned int& inout_flag);

		intersect_side frustum_aabb_leaf(const frustum_plane& fr, const aabb& a,
			unsigned int in_flag, unsigned int& inout_coherency);
		intersect_side frustum_aabb_leaf(const frustum_plane& fr, const aabb& a, unsigned int in_flag);

		intersect_side frustum_triangle_leaf(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2, unsigned int in_flag);
		intersect_side frustum_quad_leaf(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, unsigned int in_flag);

		bool is_frustum_aabb_leaf_negative_side(const frustum_plane& fr, const aabb& a, unsigned int in_flag, unsigned int& inout_coherency);
		bool is_frustum_aabb_leaf_negative_side(const frustum_plane& fr, const aabb& a, unsigned int in_flag);
		bool is_frustum_triangle_leaf_negative_side(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2, unsigned int in_flag);
		bool is_frustum_quad_leaf_negative_side(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, unsigned int in_flag);

	}
}

#include "frustum_intersection.inl"