#pragma once

#include <windef.h>
#include "plane.h"
#include "box_plane_np_lookup.h"

namespace i3
{
	struct int3;
	struct vec3;
	struct mat4;

	// n-p vert 최적화.. (리얼타임렌더링 및 교차테이블 쪽 출처의 알고리즘이 참고되었다..)

	class I3_EXPORT_MATH frustum_plane
	{
	public:
		enum plane_type { PLANE_LEFT, PLANE_RIGHT, PLANE_BOTTOM, PLANE_TOP, PLANE_NEAR, PLANE_FAR, NUM_PLANE };

		frustum_plane() { }
		frustum_plane(const i3::mat4& mat_view, const i3::mat4& mat_proj);
		frustum_plane(const i3::mat4& mat_view_proj);

		frustum_plane(const frustum_plane& rhs);
		frustum_plane&		operator=(const frustum_plane& rhs);

		void				apply(const i3::mat4& mat_view_proj);

		// 아래 함수는 반직선 4개로 상하좌우 평면을 만들고, 근원단면2개를 포함해서 새로운 종류의 프러스텀을 뽑아낼수 있게 해준다..
		void				apply(const vec3& cam_pos, const vec3& dir_left_top, const vec3& dir_right_top,
							const vec3& dir_left_bottom, const vec3& dir_right_bottom,
							const plane& near_plane, const plane& far_plane);

		// 근원단면 거리를 알면 근원단면을 재구성할수 있다. 
		void				modify_near_far_plane(const vec3& cam_pos, float near_dist, float far_dist);


		const plane&		get_plane(size_t plane_idx) const { return m_plane[plane_idx]; }
		DWORD				get_lookup_idx(size_t plane_idx) const { return m_lookup[plane_idx]; }
		const int3&			get_n_vert(size_t plane_idx) const { return i3::get_box_plane_n_vert(m_lookup[plane_idx]); }
		const int3&			get_p_vert(size_t plane_idx) const { return i3::get_box_plane_p_vert(m_lookup[plane_idx]); }

		const plane*		get_plane_array() const { return m_plane; }
		const DWORD*		get_lookup_idx_array() const { return m_lookup; }

		// 아래 함수는 행렬에 의한 프러스텀 변환을 해줌..(변환행렬을 넣기 전 역행렬 후 전치과정이 필요함)
		void				transform_inv_transpose_mat_self(const i3::mat4& inv_transpose_mat);
		// 행렬 트랜스폼의 경우 역행렬,전치과정이 들어가므로 약간의 병목을 생각해야한다.
		void				transform_mat_self(const i3::mat4& m);
		void				transform_mat_self_ortho(const i3::mat4& m);
		void				transform_mat_self_ortho_normal(const i3::mat4& m);

		void				transform_inv_transpose_mat(frustum_plane& out, const i3::mat4& inv_transpose_mat) const;
		void				transform_mat(frustum_plane& out, const i3::mat4& m) const;
		void				transform_mat_ortho(frustum_plane& out, const i3::mat4& m) const;
		void				transform_mat_ortho_normal(frustum_plane& out, const i3::mat4& m) const;

		i3::frustum_plane	transform_inv_transpose_mat(const i3::mat4& inv_transpose_mat) const;
		i3::frustum_plane	transform_mat(const i3::mat4& m) const;
		i3::frustum_plane	transform_mat_ortho(const i3::mat4& m) const;
		i3::frustum_plane	transform_mat_ortho_normal(const i3::mat4& m) const;

	private:
		plane		m_plane[NUM_PLANE];
		DWORD		m_lookup[NUM_PLANE];			// 룩업 변수 배열..AABB 교차 최적화에 도움..

	};

}

#include "frustum_plane.inl"
