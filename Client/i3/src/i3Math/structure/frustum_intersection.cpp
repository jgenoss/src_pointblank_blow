#include "i3MathType.h"

#include "structure/frustum_intersection.h"

#include "structure/intersect_side.h"
#include "structure/frustum_plane.h"
#include "structure/plane_intersection.h"

namespace i3
{

	namespace intersect
	{
		// 가장 기본이 되는 함수..
		intersect_side frustum_aabb(const frustum_plane& fr, const aabb& a)
		{
			intersect_side result = intersect_side_positive;

			for (size_t i = 0; i < frustum_plane::NUM_PLANE; ++i)
			{
				intersect_side state = i3::intersect::aabb_plane_np_lookup(a, fr.get_plane(i), 
					fr.get_n_vert(i), fr.get_p_vert(i) );

				if (state == intersect_side_negative)
					return intersect_side_negative;
				if (state == intersect_side_both)
					result = intersect_side_both;
			}
			return result;
		}

		intersect_side frustum_triangle(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2)
		{
			intersect_side result = intersect_side_positive;

			for (size_t i = 0; i < frustum_plane::NUM_PLANE; ++i)
			{
				intersect_side state = i3::intersect::triangle_plane(v0, v1, v2, fr.get_plane(i));
				if (state == intersect_side_negative)
					return intersect_side_negative;
				if (state == intersect_side_both)
					result = intersect_side_both;
			}
			return result;
		}

		bool is_frustum_aabb_negative_side(const frustum_plane& fr, const aabb& a)
		{
			for (size_t i = 0; i < frustum_plane::NUM_PLANE; ++i)
			{
				if (i3::intersect::is_aabb_plane_negative_side(a, fr.get_plane(i), fr.get_p_vert(i)))
					return true;
			}
			return false;
		}

		bool is_frustum_triangle_negative_side(const frustum_plane& fr, const vec3& v0, const vec3& v1, const vec3& v2)
		{
			for (size_t i = 0; i < frustum_plane::NUM_PLANE; ++i)
			{
				if (i3::intersect::is_triangle_plane_negative_side(v0, v1, v2, fr.get_plane(i))) 
					return true;
			}
			return false;
		}

		intersect_side frustum_aabb_branch(const frustum_plane& fr, const aabb& a,
											unsigned int& inout_flag, unsigned int& inout_coherency)
		{
			intersect_side result = intersect_side_positive;

			unsigned int in_flag = inout_flag;

			inout_flag = 0;						// 리턴할 값은 리셋...

			unsigned int curr_flag = (1 << inout_coherency);

			if (in_flag & curr_flag)		//
			{
				intersect_side state = i3::intersect::aabb_plane_np_lookup(a,
					fr.get_plane(inout_coherency), fr.get_n_vert(inout_coherency), fr.get_p_vert(inout_coherency) );

				if (state == intersect_side_negative) return intersect_side_negative; //전혀 보이지 않으므로, bit_flag = 0 이더라도 상관없음..
				if (state == intersect_side_both) { inout_flag |= curr_flag;	result = intersect_side_both; }//이것을 비트플래그에 더함...

				in_flag &= ~curr_flag;				// 이값을 루프조사에서만 제외...
				if (!(in_flag & 0x3F))    // 00111111(2)과 연산해서 모두 0이면... return result임..
					return result;	//
			}

			unsigned int i = 0;
			for (curr_flag = 1; curr_flag <= in_flag; curr_flag <<= 1, ++i)
			{
				if (in_flag & curr_flag)
				{
					intersect_side state = i3::intersect::aabb_plane_np_lookup(a, fr.get_plane(i), fr.get_n_vert(i), fr.get_p_vert(i) );
					if (state == intersect_side_negative) { inout_coherency = i; return intersect_side_negative; }
					if (state == intersect_side_both) { inout_flag |= curr_flag; result = intersect_side_both; }
				}
			}
			return result;
		}

	

		intersect_side frustum_aabb_leaf(const frustum_plane& fr, const aabb& a,
			unsigned int in_flag, unsigned int& inout_coherency)
		{
			intersect_side result = intersect_side_positive;
			unsigned int curr_flag = (1 << inout_coherency);

			if (in_flag & curr_flag)		//
			{
				intersect_side state = i3::intersect::aabb_plane_np_lookup(a, fr.get_plane(inout_coherency),
					fr.get_n_vert(inout_coherency), fr.get_p_vert(inout_coherency) );

				if (state == intersect_side_negative) return intersect_side_negative; //전혀 보이지 않으므로, bit_flag = 0 이더라도 상관없음..
				if (state == intersect_side_both) result = intersect_side_both;

				in_flag &= ~curr_flag;				// 이값을 루프조사에서만 제외...
				if (!(in_flag & 0x3F))    // 00111111(2)과 연산해서 모두 0이면... return result임..
					return result;	//
			}

			unsigned int i = 0;
			for (curr_flag = 1; curr_flag <= in_flag; curr_flag <<= 1, ++i)
			{
				if (in_flag & curr_flag)
				{
					intersect_side state = i3::intersect::aabb_plane_np_lookup(a, fr.get_plane(i), fr.get_n_vert(i), fr.get_p_vert(i) );
					if (state == intersect_side_negative) { inout_coherency = i; return intersect_side_negative; }
					if (state == intersect_side_both) result = intersect_side_both;
				}
			}
			return result;
		}

		bool is_frustum_aabb_leaf_negative_side(const frustum_plane& fr, const aabb& a, unsigned int in_flag, unsigned int& inout_coherency)
		{
			unsigned int curr_flag = (1 << inout_coherency);

			if (in_flag & curr_flag)		//
			{
				if (i3::intersect::is_aabb_plane_negative_side(a, fr.get_plane(inout_coherency), fr.get_p_vert(inout_coherency)))
					return true;
				in_flag &= ~curr_flag;				// 이값을 루프조사에서만 제외...
				if (!(in_flag & 0x3F))    // 00111111(2)과 연산해서 모두 0이면... return result임..
					return false;	//
			}

			unsigned int i = 0;
			for (curr_flag = 1; curr_flag <= in_flag; curr_flag <<= 1, ++i)
			{
				if (in_flag & curr_flag)
					if (i3::intersect::is_aabb_plane_negative_side(a, fr.get_plane(i), fr.get_p_vert(i)))
					{
						inout_coherency = i; return true;
					}
			}

			return false;

		}

	}



}