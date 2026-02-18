#include "i3MathType.h"

#include "structure/frustum_plane.h"
#include "structure/mat4.h"
#include "structure/plane_function.h"
#include "structure/box_plane_np_lookup.h"
#include "structure/vec3_function.h"
#include "structure/mat4_function.h"

namespace i3
{
	frustum_plane::frustum_plane(const i3::mat4& mat_view, const i3::mat4& mat_proj)
	{
		i3::mat4 matViewProj;
		i3::matrix_multiply(mat_view, mat_proj, matViewProj);
		frustum_plane::apply(matViewProj);
	}
	
	void	frustum_plane::apply(const i3::mat4& mat_view_proj)
	{

		m_plane[PLANE_LEFT].a = mat_view_proj._14 + mat_view_proj._11;
		m_plane[PLANE_LEFT].b = mat_view_proj._24 + mat_view_proj._21;
		m_plane[PLANE_LEFT].c = mat_view_proj._34 + mat_view_proj._31;
		m_plane[PLANE_LEFT].d = mat_view_proj._44 + mat_view_proj._41;

		m_plane[PLANE_RIGHT].a = mat_view_proj._14 - mat_view_proj._11;
		m_plane[PLANE_RIGHT].b = mat_view_proj._24 - mat_view_proj._21;
		m_plane[PLANE_RIGHT].c = mat_view_proj._34 - mat_view_proj._31;
		m_plane[PLANE_RIGHT].d = mat_view_proj._44 - mat_view_proj._41;

		m_plane[PLANE_BOTTOM].a = mat_view_proj._14 + mat_view_proj._12;
		m_plane[PLANE_BOTTOM].b = mat_view_proj._24 + mat_view_proj._22;
		m_plane[PLANE_BOTTOM].c = mat_view_proj._34 + mat_view_proj._32;
		m_plane[PLANE_BOTTOM].d = mat_view_proj._44 + mat_view_proj._42;

		m_plane[PLANE_TOP].a = mat_view_proj._14 - mat_view_proj._12;
		m_plane[PLANE_TOP].b = mat_view_proj._24 - mat_view_proj._22;
		m_plane[PLANE_TOP].c = mat_view_proj._34 - mat_view_proj._32;
		m_plane[PLANE_TOP].d = mat_view_proj._44 - mat_view_proj._42;

		// www2.ravensoft.com/users/ggribb/plane%2520extraction.pdf 에 따르면 DirectX근단면의 경우..
		m_plane[PLANE_NEAR].a = mat_view_proj._13;
		m_plane[PLANE_NEAR].b = mat_view_proj._23;
		m_plane[PLANE_NEAR].c = mat_view_proj._33;
		m_plane[PLANE_NEAR].d = mat_view_proj._43;

		m_plane[PLANE_FAR].a = mat_view_proj._14 - mat_view_proj._13;
		m_plane[PLANE_FAR].b = mat_view_proj._24 - mat_view_proj._23;
		m_plane[PLANE_FAR].c = mat_view_proj._34 - mat_view_proj._33;
		m_plane[PLANE_FAR].d = mat_view_proj._44 - mat_view_proj._43;

		// 각 평면들의 노말은 프러스텀의 안쪽을 향한다.
		for (size_t side = 0; side < NUM_PLANE; ++side)
		{
			i3::plane_normal_normalize_self(m_plane[side]);
			m_lookup[side] = i3::calc_box_plane_lookup_idx(m_plane[side]);
		}
		//
	}

	void	frustum_plane::apply(const vec3& cam_pos, const vec3& dir_left_top, const vec3& dir_right_top,
		const vec3& dir_left_bottom, const vec3& dir_right_bottom,
		const plane& near_plane, const plane& far_plane)
	{
		vec3 normal;
		i3::vec3_cross(normal, dir_left_top, dir_left_bottom);
		i3::vec3_normalize_self(normal);
		i3::make_plane_from_point_normal(m_plane[PLANE_LEFT], cam_pos, normal);

		i3::vec3_cross(normal, dir_right_bottom, dir_right_top);
		i3::vec3_normalize_self(normal);
		i3::make_plane_from_point_normal(m_plane[PLANE_RIGHT], cam_pos, normal);

		i3::vec3_cross(normal, dir_left_bottom, dir_right_bottom);
		i3::vec3_normalize_self(normal);
		i3::make_plane_from_point_normal(m_plane[PLANE_BOTTOM], cam_pos, normal);

		i3::vec3_cross(normal, dir_right_top, dir_left_top);
		i3::vec3_normalize_self(normal);
		i3::make_plane_from_point_normal(m_plane[PLANE_TOP], cam_pos, normal);

		m_plane[PLANE_NEAR] = near_plane;
		m_plane[PLANE_FAR] = far_plane;

		// 각 평면들의 노말은 프러스텀의 안쪽을 향한다.
		for (size_t side = 0; side < NUM_PLANE; ++side)
		{
			m_lookup[side] = i3::calc_box_plane_lookup_idx(m_plane[side]);		// 평면 노멀라이즈작업은 이미 처리됨..
		}

	}

	void	frustum_plane::modify_near_far_plane(const vec3& cam_pos, float near_dist, float far_dist)
	{
		float n_p = plane_normal_dot_vec3(m_plane[PLANE_NEAR], cam_pos);
		m_plane[PLANE_NEAR].d = -n_p - near_dist;
		m_plane[PLANE_FAR].d = +n_p + far_dist;
		// 특별이, 노멀값이나 룩업테이블이 바뀌거나 하지 않는다는 점에 유의...
	}

	void	frustum_plane::transform_inv_transpose_mat_self(const i3::mat4& inv_transpose_mat)
	{
		for (size_t side = 0; side < NUM_PLANE; ++side)
			i3::plane_transform_inv_transpose_mat_self(m_plane[side], inv_transpose_mat);

		for (size_t side = 0; side < NUM_PLANE; ++side)
		{
			i3::plane_normal_normalize_self(m_plane[side]);
			m_lookup[side] = i3::calc_box_plane_lookup_idx(m_plane[side]);
		}
	}

	void	frustum_plane::transform_mat_self(const i3::mat4& m)
	{
		i3::mat4 inv_transpose;
		i3::mat4_inv(inv_transpose, m);
		i3::mat4_transpose(inv_transpose, inv_transpose);
		frustum_plane::transform_inv_transpose_mat_self(inv_transpose);
	}

	void	frustum_plane::transform_mat_self_ortho(const i3::mat4& m)
	{
		i3::mat4 inv_transpose;
		i3::mat4_inv_transpose_ortho(inv_transpose, m);
		frustum_plane::transform_inv_transpose_mat_self(inv_transpose);
	}

	void	frustum_plane::transform_mat_self_ortho_normal(const i3::mat4& m)
	{
		i3::mat4 inv_transpose;
		i3::mat4_inv_transpose_ortho_normal(inv_transpose, m);
		frustum_plane::transform_inv_transpose_mat_self(inv_transpose);
	}

	void frustum_plane::transform_inv_transpose_mat(frustum_plane& out, const i3::mat4& inv_transpose_mat) const
	{
		for (size_t side = 0; side < NUM_PLANE; ++side)
			i3::plane_transform_inv_transpose_mat(out.m_plane[side], m_plane[side], inv_transpose_mat);

		for (size_t side = 0; side < NUM_PLANE; ++side)
		{
			i3::plane_normal_normalize_self(out.m_plane[side]);
			out.m_lookup[side] = i3::calc_box_plane_lookup_idx(out.m_plane[side]);
		}
	}

	void frustum_plane::transform_mat(frustum_plane& out, const i3::mat4& m) const
	{
		i3::mat4 inv_transpose;
		i3::mat4_inv(inv_transpose, m);
		i3::mat4_transpose(inv_transpose, inv_transpose);
		frustum_plane::transform_inv_transpose_mat(out, inv_transpose);
	}

	void frustum_plane::transform_mat_ortho(frustum_plane& out, const i3::mat4& m) const
	{
		i3::mat4 inv_transpose;
		i3::mat4_inv_transpose_ortho(inv_transpose, m);
		frustum_plane::transform_inv_transpose_mat(out, inv_transpose);
	}

	void frustum_plane::transform_mat_ortho_normal(frustum_plane& out, const i3::mat4& m) const
	{
		i3::mat4 inv_transpose;
		i3::mat4_inv_transpose_ortho_normal(inv_transpose, m);
		frustum_plane::transform_inv_transpose_mat(out, inv_transpose);
	}

	i3::frustum_plane frustum_plane::transform_inv_transpose_mat(const i3::mat4& inv_transpose_mat) const
	{
		i3::frustum_plane res;
		frustum_plane::transform_inv_transpose_mat(res, inv_transpose_mat);
		return res;
	}

	i3::frustum_plane frustum_plane::transform_mat(const i3::mat4& m) const
	{
		i3::mat4 inv_transpose;
		i3::mat4_inv(inv_transpose, m);
		i3::mat4_transpose(inv_transpose, inv_transpose);
		return frustum_plane::transform_inv_transpose_mat(inv_transpose);
	}

	i3::frustum_plane frustum_plane::transform_mat_ortho(const i3::mat4& m) const
	{
		i3::mat4 inv_transpose;
		i3::mat4_inv_transpose_ortho(inv_transpose, m);
		return frustum_plane::transform_inv_transpose_mat(inv_transpose);
	}

	i3::frustum_plane frustum_plane::transform_mat_ortho_normal(const i3::mat4& m) const
	{
		i3::mat4 inv_transpose;
		i3::mat4_inv_transpose_ortho_normal(inv_transpose, m);
		return frustum_plane::transform_inv_transpose_mat(inv_transpose);
	}


}
