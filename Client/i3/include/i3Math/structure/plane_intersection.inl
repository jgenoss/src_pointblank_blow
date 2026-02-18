
#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

#include "vec3.h"
#include "aabb.h"
#include "intersect_side.h"
#include "plane.h"
#include "plane_function.h"

namespace i3
{
	namespace intersect
	{
		inline intersect_side aabb_plane(const aabb& a, const plane& pl)
		{
			vec3 center;		a.calc_center(center);
			vec3 extent(a.max());	extent -= center;
			return i3::intersect::aabb_plane_center_extent(center, extent, pl);
		}

		inline intersect_side aabb_plane_min_max(const vec3& v_min, const vec3& v_max, const plane& pl)
		{
			vec3 center(v_min);	center += v_max;	center *= 0.5f;
			vec3 extent(v_max);	extent -= center;
			return i3::intersect::aabb_plane_center_extent(center, extent, pl);
		}

		inline bool is_aabb_plane_negative_side(const aabb& a, const plane& pl)
		{
			vec3 center;		a.calc_center(center);
			vec3 extent(a.max());	extent -= center;
			return i3::intersect::is_aabb_plane_negative_side_center_extent(center, extent, pl);
		}

		inline bool is_aabb_plane_negative_side_center_extent(const vec3& center, const vec3& extent, const plane& pl)
		{
			return (calc_plane_distance(pl, center) < -plane_normal_abs_dot_vec3(pl, extent));
		}
	}
}

#pragma pop_macro("max")
#pragma pop_macro("min")

