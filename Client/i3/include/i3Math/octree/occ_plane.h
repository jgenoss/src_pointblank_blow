#pragma once

#include "../structure/box_plane_np_lookup.h"

namespace i3
{
	// 프러스텀
	struct int3;
	struct vec3;
	struct plane;
	class aabb;
	class occ_quad;
	enum intersect_side;

	class occ_plane
	{
	public:
		// 0번은 near 1번부터 3~4번까지 쿼드 사이드 평면..
		occ_plane() : m_num_plane(0) {}
		occ_plane(const vec3& cam_pos, const occ_quad* quad);

		void				apply(const vec3& cam_pos, const occ_quad* quad);

		size_t				get_num_plane() const { return m_num_plane; }
		const plane&		get_plane(size_t plane_idx) const { return m_plane[plane_idx]; }
		DWORD				get_lookup_idx(size_t plane_idx) const { return m_lookup[plane_idx]; }
		const int3&			get_n_vert(size_t plane_idx) const { return i3::get_box_plane_n_vert(m_lookup[plane_idx]); }
		const int3&			get_p_vert(size_t plane_idx) const { return i3::get_box_plane_p_vert(m_lookup[plane_idx]); }
		
	private:
		i3::plane	m_plane[5];
		DWORD		m_lookup[5];
		DWORD		m_num_plane;
	};

	namespace intersect
	{
		intersect_side occ_aabb(const occ_plane& op, const aabb& a);

		intersect_side occ_quad(const occ_plane& op, const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3);

		intersect_side occ_triangle(const occ_plane& op, const vec3& v0, const vec3& v1, const vec3& v2 );


		// out_intersect_flag는 intersect_both판정일때 최하위비트부터 교차면에 대해 비트를 하나씩 올리게 됨...
//		intersect_side	occ_aabb(const occ_plane& op, const aabb& a, unsigned int& out_intersect_flag);

		// in_skip_edge_flag에 세워진 엣지 평면 쪽은 계산하지 않고 무시하며(negative로 간주)  continue를 건다..
		intersect_side occ_aabb(const occ_plane& op, const aabb& a, unsigned int in_skip_plane_flag,
			unsigned int& out_intersect_plane_flag, bool is_front_face); 
					
		intersect_side occ_quad(const occ_plane& op, const vec3& v0,
			const vec3& v1, const vec3& v2, const vec3& v3, unsigned int in_skip_plane_flag,
			unsigned int& out_intersect_plane_flag, bool is_front_face);

		intersect_side occ_triangle(const occ_plane& op, const vec3& v0,
			const vec3& v1, const vec3& v2, unsigned int in_skip_plane_flag,
			unsigned int& out_intersect_plane_flag, bool is_front_face);

	}
}
