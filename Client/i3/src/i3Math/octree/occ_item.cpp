#include "i3MathType.h"
#include "i3Math/octree/occ_item.h"
#include "i3Math/octree/occ_culler.h"
#include "i3Math/octree/occ_quad_set.h"
#include "structure/frustum_intersection.h"

namespace i3
{

	void	occludee_item_dynamic::on_fully_visible_frustum_aabb()
	{
		m_culler->_add_octree_visible_item(this);			// 오클루전 컬링쪽으로 보냄...
	}

	void	occludee_item_dynamic::on_partially_visible_frustum_aabb(const frustum_plane& fr, unsigned int flag_frustum)
	{
		m_culler->_add_octree_visible_item(this);		// // 오클루전 컬링쪽으로 보냄... 좀더 세밀한 것을 원한다면 이쪽 구현이 약간 변경되어야 함...
	}

	void	occluder_item_static::on_hidden_frustum_aabb()
	{
		//  not impl.
	}

	void	occluder_item_static::on_fully_visible_frustum_aabb()
	{
		m_occ_quad->get_culler()->_add_octree_visible_quad(m_occ_quad);		// 오클루전 컬링쪽으로 보냄...
	}

	void	occluder_item_static::on_partially_visible_frustum_aabb(const frustum_plane& fr, unsigned int flag_frustum)
	{
	
		if (m_occ_quad->is_quad_triangle())
		{
			const vec3& v0 = m_occ_quad->get_quad_vertex_pos(0);
			const vec3& v1 = m_occ_quad->get_quad_vertex_pos(1);
			const vec3& v2 = m_occ_quad->get_quad_vertex_pos(2);

			if (i3::intersect::is_frustum_triangle_leaf_negative_side(fr, v0, v1, v2, flag_frustum)) // invisible
			{
				// 안보임...  not impl. 
			}
			else  // visible
			{
				m_occ_quad->get_culler()->_add_octree_visible_quad(m_occ_quad);		// 오클루전 컬링쪽으로 보냄...
			}

		}
		else
		{
			const vec3& v0 = m_occ_quad->get_quad_vertex_pos(0);
			const vec3& v1 = m_occ_quad->get_quad_vertex_pos(1);
			const vec3& v2 = m_occ_quad->get_quad_vertex_pos(2);
			const vec3& v3 = m_occ_quad->get_quad_vertex_pos(3);

			if (i3::intersect::is_frustum_quad_leaf_negative_side(fr, v0, v1, v2, v3, flag_frustum)) // invisible
			{
				// 안보임...  not impl. 
			}
			else  // visible
			{
				m_occ_quad->get_culler()->_add_octree_visible_quad(m_occ_quad);		// 오클루전 컬링쪽으로 보냄...
			}
		}
	}
}