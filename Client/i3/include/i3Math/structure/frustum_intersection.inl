
#include "plane_intersection.h"
#include "frustum_plane.h"

namespace i3
{
	

	namespace intersect
	{
		inline intersect_side frustum_aabb_branch(const frustum_plane& fr, const aabb& a, unsigned int& inout_flag)
		{
			return aabb_planes_branch(a, fr.get_plane_array(), fr.get_lookup_idx_array(), inout_flag);
		}

		inline intersect_side frustum_aabb_leaf(const frustum_plane& fr, const aabb& a, unsigned int in_flag)
		{
			return aabb_planes_leaf(a, fr.get_plane_array(), fr.get_lookup_idx_array(), in_flag);
		}

		inline intersect_side frustum_triangle_leaf(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2, unsigned int in_flag)
		{
			return triangle_planes_leaf(v0, v1, v2, fr.get_plane_array(), in_flag);
		}

		inline intersect_side frustum_quad_leaf(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, unsigned int in_flag)
		{
			return quad_planes_leaf(v0, v1, v2, v3, fr.get_plane_array(), in_flag);
		}
		
		inline bool is_frustum_aabb_leaf_negative_side(const frustum_plane& fr, const aabb& a, unsigned int in_flag)
		{
			return is_aabb_planes_leaf_negative_side(a, fr.get_plane_array(), fr.get_lookup_idx_array(), in_flag);
		}

		inline bool is_frustum_triangle_leaf_negative_side(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2, unsigned int in_flag)
		{
			return is_triangle_planes_leaf_negative_side(v0, v1, v2, fr.get_plane_array(), in_flag);
		}

		inline bool is_frustum_quad_leaf_negative_side(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, unsigned int in_flag)
		{
			return is_quad_planes_leaf_negative_side(v0, v1, v2, v3, fr.get_plane_array(), in_flag);
		}


	}
}
