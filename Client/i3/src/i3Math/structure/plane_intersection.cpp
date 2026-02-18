#include "i3MathType.h"

#include "structure/plane_intersection.h"
#include "structure/plane_function.h"
#include "structure/int_structure.h"
#include "structure/box_plane_np_lookup.h"

#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

namespace i3
{
	namespace intersect
	{
		intersect_side  aabb_plane_center_extent(const vec3& center, const vec3& extent, const plane& pl)
		{
			float fMaxDist = i3::plane_normal_abs_dot_vec3(pl, extent);
			float fDist = i3::calc_plane_distance(pl, center);
			if (fDist < -fMaxDist) return i3::intersect_side_negative;
			if (fDist > fMaxDist) return i3::intersect_side_positive;
			return i3::intersect_side_both;
		}

		// n, p vertex인덱스로 좀더 빠르게 판정 가능하다고 한다.

		intersect_side aabb_plane_np_lookup(const aabb& a, const plane& pl,
			const int3& n_vert, const int3& p_vert)
		{
			// p_vert 비교...(포지티브)
			if (pl.a * a.calc_vertex_x(p_vert[0]) + pl.b * a.calc_vertex_y(p_vert[1])
				+ pl.c * a.calc_vertex_z(p_vert[2]) + pl.d < 0.f)
				return intersect_side_negative;

			// n_vert 비교...(네거티브)
			if (pl.a * a.calc_vertex_x(n_vert[0]) + pl.b * a.calc_vertex_y(n_vert[1])
				+ pl.c * a.calc_vertex_z(n_vert[2]) + pl.d < 0.f)
				return intersect_side_both;

			return intersect_side_positive;
		}
		



		
		bool is_aabb_plane_negative_side(const aabb& a, const plane& pl, const int3& p_vert)
		{
			// p_vert 한 회비교로 간단이 마칠수 있음...
			return (pl.a * a.calc_vertex_x(p_vert[0]) + pl.b * a.calc_vertex_y(p_vert[1])
				+ pl.c * a.calc_vertex_z(p_vert[2]) + pl.d < 0.f);
		}

		intersect_side triangle_plane(const vec3& v0, const vec3& v1, const vec3& v2, const plane& pl)
		{
			float d0 = i3::plane_dot_vec3(pl, v0);
			float d1 = i3::plane_dot_vec3(pl, v1);
			if ((d0 > 0.f && d1 < 0.f) || (d0 < 0.f && d1 > 0.f)) return intersect_side_both;

			float d2 = i3::plane_dot_vec3(pl, v2);
			if (d0 < 0.f && d2 < 0.f) return intersect_side_negative;
			if (d0 > 0.f && d2 > 0.f) return intersect_side_positive;
			return intersect_side_both;
		}

		intersect_side quad_plane(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, const plane& pl)
		{
			float d0 = i3::plane_dot_vec3(pl, v0);
			float d1 = i3::plane_dot_vec3(pl, v1);
			if ((d0 > 0.f && d1 < 0.f) || (d0 < 0.f && d1 > 0.f)) return intersect_side_both;	// 부호가 다른 것이 발견되면 무조건 교차됨.

			float d2 = i3::plane_dot_vec3(pl, v2);
			if ((d0 > 0.f && d2 < 0.f) || (d0 < 0.f && d2 > 0.f)) return intersect_side_both;

			float d3 = i3::plane_dot_vec3(pl, v3);
			
			if (d0 < 0.f && d3 < 0.f) return intersect_side_negative;
			if (d0 > 0.f && d3 > 0.f) return intersect_side_positive;
			return intersect_side_both;
		}
		
		bool is_triangle_plane_negative_side(const vec3& v0, const vec3& v1, const vec3& v2, const plane& pl)
		{
			if (i3::plane_dot_vec3(pl, v0) >= 0.f) return false;
			if (i3::plane_dot_vec3(pl, v1) >= 0.f) return false;
			if (i3::plane_dot_vec3(pl, v2) >= 0.f) return false;
			return true;
		}

		bool is_quad_plane_negative_side(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, const plane& pl)
		{
			if (i3::plane_dot_vec3(pl, v0) >= 0.f) return false;		// 하나라도 양수 방향에 있으면 무조건 교차나 positive이다..
			if (i3::plane_dot_vec3(pl, v1) >= 0.f) return false;
			if (i3::plane_dot_vec3(pl, v2) >= 0.f) return false;
			if (i3::plane_dot_vec3(pl, v3) >= 0.f) return false;		
			return true;
		}
		
		intersect_side	aabb_planes_branch(const aabb& a, const plane* array_plane, unsigned int& inout_flag)
		{
			vec3 c_a = a.calc_center();
			vec3 e_a = a.max();			e_a -= c_a;
			
			intersect_side result = intersect_side_positive;
			unsigned int in_flag = inout_flag;
			inout_flag = 0;						// 리턴할 값은 리셋...

			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					intersect_side state = i3::intersect::aabb_plane_center_extent(c_a, e_a, array_plane[i]);

					if (state == intersect_side_negative)
						return intersect_side_negative;

					if (state == intersect_side_both)
					{
						inout_flag |= flag;
						result = intersect_side_both;
					}
				}
			}
			return result;
		}

		intersect_side	aabb_planes_leaf(const aabb& a, const plane* array_plane, unsigned int in_flag)
		{
			vec3 c_a = a.calc_center();
			vec3 e_a = a.max();			e_a -= c_a;

			intersect_side result = intersect_side_positive;

			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					intersect_side state = i3::intersect::aabb_plane_center_extent(c_a, e_a, array_plane[i]);

					if (state == intersect_side_negative)
						return intersect_side_negative;

					if (state == intersect_side_both)
						result = intersect_side_both;
				}
			}
			return result;
		}

		intersect_side	aabb_planes_branch(const aabb& a, const plane* array_plane, const DWORD* lookup_array, unsigned int& inout_flag)
		{
			intersect_side result = intersect_side_positive;
			unsigned int in_flag = inout_flag;
			inout_flag = 0;						// 리턴할 값은 리셋...

			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					DWORD lookup = lookup_array[i];
					intersect_side state = i3::intersect::aabb_plane_np_lookup(a, array_plane[i],
						i3::get_box_plane_n_vert(lookup), i3::get_box_plane_p_vert(lookup) );

					if (state == intersect_side_negative)
						return intersect_side_negative;

					if (state == intersect_side_both)
					{
						inout_flag |= flag;
						result = intersect_side_both;
					}
				}
			}

			return result;
		}

		intersect_side	aabb_planes_leaf(const aabb& a, const plane* array_plane, const DWORD* lookup_array, unsigned int in_flag)
		{
			intersect_side result = intersect_side_positive;

			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					DWORD lookup = lookup_array[i];
					intersect_side state = i3::intersect::aabb_plane_np_lookup(a, array_plane[i],
						i3::get_box_plane_n_vert(lookup), i3::get_box_plane_p_vert(lookup) );

					if (state == intersect_side_negative)
						return intersect_side_negative;

					if (state == intersect_side_both)
						result = intersect_side_both;
				}
			}
			return result;
		}

		intersect_side  triangle_planes_leaf(const vec3& v0, const vec3& v1, const vec3& v2, const plane* array_plane, unsigned int in_flag)
		{
			intersect_side result = intersect_side_positive;

			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					intersect_side state = i3::intersect::triangle_plane(v0, v1, v2, array_plane[i]);

					if (state == intersect_side_negative)
						return intersect_side_negative;

					if (state == intersect_side_both)
						result = intersect_side_both;
				}
			}
			return result;
		}

		intersect_side  quad_planes_leaf(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, const plane* array_plane, unsigned int in_flag)
		{
			intersect_side result = intersect_side_positive;

			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					intersect_side state = i3::intersect::quad_plane(v0, v1, v2, v3, array_plane[i]);

					if (state == intersect_side_negative)
						return intersect_side_negative;

					if (state == intersect_side_both)
						result = intersect_side_both;
				}
			}
			return result;
		}
		
		bool is_aabb_planes_leaf_negative_side(const aabb& a, const plane* array_plane, unsigned int in_flag)
		{
			vec3 c_a = a.calc_center();
			vec3 e_a = a.max();			e_a -= c_a;

			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					if (i3::intersect::is_aabb_plane_negative_side_center_extent(c_a, e_a, array_plane[i]))
						return true;
				}
			}
			return false;
		}

		bool is_aabb_planes_leaf_negative_side(const aabb& a, const plane* array_plane, const DWORD* lookup_array, unsigned int in_flag)
		{
			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					if (i3::intersect::is_aabb_plane_negative_side(a, array_plane[i],
						i3::get_box_plane_p_vert(lookup_array[i])))
						return true;
				}
			}

			return false;
		}

		bool is_triangle_planes_leaf_negative_side(const vec3& v0, const vec3& v1, const vec3& v2, const plane* array_plane, unsigned int in_flag)
		{
			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					if (i3::intersect::is_triangle_plane_negative_side(v0, v1, v2, array_plane[i]))
						return true;
				}
			}
			return false;
		}

		bool is_quad_planes_leaf_negative_side(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3, const plane* array_plane, unsigned int in_flag)
		{
			unsigned int i = 0;
			for (unsigned int flag = 1; flag <= in_flag; flag <<= 1, ++i)
			{
				if (in_flag &  flag)
				{
					if (i3::intersect::is_quad_plane_negative_side(v0, v1, v2, v3, array_plane[i]))
						return true;
				}
			}
			return false;
		}


	}
}

#pragma pop_macro("max")
#pragma pop_macro("min")
