#include "i3MathType.h"

#include "octree/occ_plane.h"

#include "structure/intersect_side.h"
#include "octree/occ_quad_set.h"
#include "structure/plane_function.h"
#include "structure/plane_intersection.h"

namespace i3
{

	occ_plane::occ_plane(const vec3& cam_pos, const occ_quad* quad)
	{
		occ_plane::apply(cam_pos, quad);
	}

	void	occ_plane::apply(const vec3& cam_pos, const occ_quad* quad)
	{
		// 먼저 near_plane은 그대로 대입처리한다...
		m_plane[0] = quad->get_quad_plane();					// near_plane;

		// 정점을 0->1->2->3 순서로 보고, 삼각형 평면을 하나씩 구해 넣으면 된다..
		// 카메라 포지션을 시작으로 변 순서대로 밀어넣으면 될듯...
		const i3::vec3& v0 = quad->get_quad_vertex_pos(0);
		const i3::vec3& v1 = quad->get_quad_vertex_pos(1);
		const i3::vec3& v2 = quad->get_quad_vertex_pos(2);

		if (quad->is_quad_triangle())
		{
			m_num_plane = 4;
			i3::make_plane_from_points(m_plane[1], cam_pos, v0, v1);
			i3::make_plane_from_points(m_plane[2], cam_pos, v1, v2);
			i3::make_plane_from_points(m_plane[3], cam_pos, v2, v0);
		}
		else
		{
			m_num_plane = 5;
			const i3::vec3& v3 = quad->get_quad_vertex_pos(3);
			i3::make_plane_from_points(m_plane[1], cam_pos, v0, v1);
			i3::make_plane_from_points(m_plane[2], cam_pos, v1, v2);
			i3::make_plane_from_points(m_plane[3], cam_pos, v2, v3);
			i3::make_plane_from_points(m_plane[4], cam_pos, v3, v0);
		}

		for (size_t side = 0; side < m_num_plane; ++side)
		{
			m_lookup[side] = i3::calc_box_plane_lookup_idx(m_plane[side]);		// 평면 노멀라이즈작업은 이미 처리됨..
		}
	}

	namespace intersect
	{
		/*
		intersect_side	occ_aabb(const occ_plane& op, const aabb& a, unsigned int& out_intersect_edge_flag)
		{
			intersect_side result = intersect_side_negative;
			const size_t num_plane = op.get_num_plane();

			out_intersect_edge_flag = 0;
			
			unsigned int curr_flag = 1;
			unsigned int intersect_flag = 0;

			for (size_t i = 0; i < num_plane; ++i, curr_flag <<= 1 )
			{
				intersect_side state = i3::intersect::aabb_plane_np_lookup(a, op.get_plane(i),
					op.get_n_vert(i), op.get_p_vert(i));

				if (state == intersect_side_positive)		// 어느것이든 양의 방향이면 교차없이 항상 보이는 놈임...
				{
					out_intersect_edge_flag = 0;
					return intersect_side_positive;
				}

				if (state == intersect_side_both)
				{
					intersect_flag |= curr_flag;
					result = intersect_side_both;
				}
			}

			out_intersect_edge_flag = (intersect_flag >> 1);		// 최상평면은 뺀다..

			return result;
		}
		*/

		intersect_side occ_aabb(const occ_plane& op, const aabb& a)
		{
			intersect_side result = intersect_side_negative;
			const size_t num_plane = op.get_num_plane();

			unsigned int curr_flag = 1;
		
			for (size_t i = 0; i < num_plane; ++i, curr_flag <<= 1)
			{
			
				intersect_side state = i3::intersect::aabb_plane_np_lookup(a, op.get_plane(i),
					op.get_n_vert(i), op.get_p_vert(i));

				if (state == intersect_side_positive)		// 어느것이든 양의 방향이면 교차없이 항상 보이는 놈임...
				{
					return intersect_side_positive;
				}

				if (state == intersect_side_both)
				{
					result = intersect_side_both;
				}
			}

			return result;
		}

		intersect_side occ_quad(const occ_plane& op, const vec3& v0, const vec3& v1, const vec3& v2, const vec3& v3)
		{
			intersect_side result = intersect_side_negative;
			const size_t num_plane = op.get_num_plane();

			unsigned int curr_flag = 1;
		
			for (size_t i = 0; i < num_plane; ++i, curr_flag <<= 1)
			{
		
				intersect_side state = i3::intersect::quad_plane(v0, v1, v2, v3, op.get_plane(i));

				if (state == intersect_side_positive)		// 어느것이든 양의 방향이면 교차없이 항상 보이는 놈임...
				{
					return intersect_side_positive;
				}

				if (state == intersect_side_both)
				{
					result = intersect_side_both;
				}
			}

			return result;
		}

		intersect_side occ_triangle(const occ_plane& op, const vec3& v0, const vec3& v1, const vec3& v2)
		{
			intersect_side result = intersect_side_negative;
			const size_t num_plane = op.get_num_plane();

			unsigned int curr_flag = 1;
		
			for (size_t i = 0; i < num_plane; ++i, curr_flag <<= 1)
			{

				intersect_side state = i3::intersect::triangle_plane(v0, v1, v2, op.get_plane(i));

				if (state == intersect_side_positive)		// 어느것이든 양의 방향이면 교차없이 항상 보이는 놈임...
				{
					return intersect_side_positive;
				}

				if (state == intersect_side_both)
				{
					result = intersect_side_both;
				}
			}

			return result;
		}
		

		// 교차면이 있더라도 실제 어떤면들이 교차했는지 여부는 다시 재검사 해보아야한다...해당 루틴은 occ_culler 안에 들어있다... 기본적으로 시야와 쿼드선분의 각 끝점을 잇는 2개의 선분에 대해 직교평면을 구해서...
		// 평면식 사이에 aabb가 놓여있는지 점검해보면 된다..

		intersect_side	occ_aabb(const occ_plane& op, const aabb& a, unsigned int in_skip_plane_flag, unsigned int& out_intersect_plane_flag, bool is_front_face)
		{
			intersect_side result = intersect_side_negative;
			const size_t num_plane = op.get_num_plane();

			out_intersect_plane_flag = 0;

			unsigned int curr_flag = 1;
			unsigned int intersect_flag = 0;

			const intersect_side visible_state = (is_front_face) ? intersect_side_positive : intersect_side_negative;

			for (size_t i = 0; i < num_plane; ++i, curr_flag <<= 1)
			{
				if (in_skip_plane_flag & curr_flag)	// 스킵플래그가 걸려있다면 계산을 넘긴다..(hidden 취급)
					continue;

				intersect_side state = i3::intersect::aabb_plane_np_lookup(a, op.get_plane(i),
					op.get_n_vert(i), op.get_p_vert(i));

				if (state == visible_state)		// 어느것이든 양의 방향이면 교차없이 항상 보이는 놈임...
				{
					out_intersect_plane_flag = 0;
					return intersect_side_positive;
				}

				if (state == intersect_side_both)
				{
					intersect_flag |= curr_flag;
					result = intersect_side_both;
				}
			}

			out_intersect_plane_flag = intersect_flag;

			return result;
		}

		intersect_side occ_quad(const occ_plane& op, const vec3& v0,
			const vec3& v1, const vec3& v2, const vec3& v3, unsigned int in_skip_plane_flag,
			unsigned int& out_intersect_plane_flag, bool is_front_face)
		{
			intersect_side result = intersect_side_negative;
			const size_t num_plane = op.get_num_plane();

			out_intersect_plane_flag = 0;
			unsigned int curr_flag = 1;
			unsigned int intersect_flag = 0;

			const intersect_side visible_state = (is_front_face) ? intersect_side_positive : intersect_side_negative;

			for (size_t i = 0; i < num_plane; ++i, curr_flag <<= 1)
			{
				if (in_skip_plane_flag & curr_flag)	// 스킵플래그가 걸려있다면 계산을 넘긴다..(hidden 취급)
					continue;

				intersect_side state = i3::intersect::quad_plane(v0, v1, v2, v3, op.get_plane(i));

				if (state == visible_state)		// 어느것이든 양의 방향이면 교차없이 항상 보이는 놈임...
				{
					out_intersect_plane_flag = 0;
					return intersect_side_positive;
				}

				if (state == intersect_side_both)
				{
					intersect_flag |= curr_flag;
					result = intersect_side_both;
				}
			}

			out_intersect_plane_flag = intersect_flag;

			return result;
		}

		intersect_side occ_triangle(const occ_plane& op, const vec3& v0,
			const vec3& v1, const vec3& v2, unsigned int in_skip_plane_flag,
			unsigned int& out_intersect_plane_flag, bool is_front_face)
		{
			intersect_side result = intersect_side_negative;
			const size_t num_plane = op.get_num_plane();

			out_intersect_plane_flag = 0;
			unsigned int curr_flag = 1;
			unsigned int intersect_flag = 0;

			const intersect_side visible_state = (is_front_face) ? intersect_side_positive : intersect_side_negative;

			for (size_t i = 0; i < num_plane; ++i, curr_flag <<= 1)
			{
				if (in_skip_plane_flag & curr_flag)	// 스킵플래그가 걸려있다면 계산을 넘긴다..(hidden 취급)
					continue;

				intersect_side state = i3::intersect::triangle_plane(v0, v1, v2, op.get_plane(i));

				if (state == visible_state)		// 어느것이든 양의 방향이면 교차없이 항상 보이는 놈임...
				{
					out_intersect_plane_flag = 0;
					return intersect_side_positive;
				}

				if (state == intersect_side_both)
				{
					intersect_flag |= curr_flag;
					result = intersect_side_both;
				}
			}

			out_intersect_plane_flag = intersect_flag;

			return result;
		}

	}

}

