#include "i3MathType.h"

#include "mesh_function/quad_adj_collection.h"
#include "structure/vec3.h"
#include "structure/vec3_function.h"

namespace i3
{

	namespace
	{
		bool is_valid_polygon(const i3::uint32_t* quad_index_arr, size_t face_idx)
		{
			i3::uint32_t a0 = quad_index_arr[face_idx * 4];
			i3::uint32_t a1 = quad_index_arr[face_idx * 4 + 1];
			i3::uint32_t a2 = quad_index_arr[face_idx * 4 + 2];
			i3::uint32_t a3 = quad_index_arr[face_idx * 4 + 3];

			if (a0 == i3::uint32_t(-1) || a1 == i3::uint32_t(-1) || a2 == i3::uint32_t(-1))
				return false;
			// i3는 -1 이 허용되고, 쿼드가 아닌 삼각형임..
			if (a3 == i3::uint32_t(-1))
			{
				if (a0 == a1 || a1 == a2 || a2 == a0)
					return false;
			}
			else
			{
				if (a0 == a1 || a1 == a2 || a2 == a3 || a3 == 0)
					return false;
			}

			return true;
		}

		size_t get_num_vertex_of_polygon(const i3::uint32_t* quad_index_arr, size_t face_idx)
		{
			return (quad_index_arr[face_idx * 4 + 3] == i3::uint32_t(-1)) ? 3 : 4;
		}

		// 엣지간 인접 여부 조사 함수...
		// 라인 A를 기준으로 B의 두점이 선상에 놓여있는지 보고 판단하도록 구현...

		bool is_line_adjacent(const i3::vec3& lineA0, const i3::vec3& lineA1, const i3::vec3& lineB0, const i3::vec3& lineB1, 
			float dist_epsilon, float edge_epsilon)
		{
			vec3 lineA01 = lineA1;	lineA01 -= lineA0;
			vec3 lineB01 = lineB1;	lineB01 -= lineB0;

			float dot_lineA01_lineB01 = i3::vec3_dot(lineA01, lineB01);
			if (dot_lineA01_lineB01 >= 0.f)		// 인접면간 면벡터가 서로 반대방향이어야 함....같은 방향이면 인접면으로 취급하지 않는다..
				return false;

			const vec3* pLineB0 = &lineB0;
			const vec3* pLineB1 = &lineB1;

//			if (dot_lineA01_lineB01 >= 0.f)		/// 인접면간 면벡터가 서로 반대방향이어야 함.. 하지만 여기서는 인접면으로 취급해봄..
//			{
//				i3::swap(pLineB0, pLineB1);
//			}
			
			const float dist_eps_sq = dist_epsilon*dist_epsilon;
			float lensq_lineA01 = i3::vec3_length_sq(lineA01);

			vec3 lineA0B0 = *pLineB0;	lineA0B0 -= lineA0;
			float lensq_lineA0B0 = i3::vec3_length_sq(lineA0B0);
			float dot_lineA01_lineA0B0 = i3::vec3_dot(lineA01, lineA0B0);

			float dist_sq_B0 = lensq_lineA0B0 - (dot_lineA01_lineA0B0*dot_lineA01_lineA0B0) / lensq_lineA01;

			if (dist_sq_B0 > dist_eps_sq)			// 점 B0이 A라인에서 떨어져 있음..
				return false;

			vec3 lineA0B1 = *pLineB1;	lineA0B1 -= lineA0;
			float lensq_lineA0B1 = i3::vec3_length_sq(lineA0B1);
			float dot_lineA01_lineA0B1 = i3::vec3_dot(lineA01, lineA0B1);
			
			float dist_sq_B1 = lensq_lineA0B1 - (dot_lineA01_lineA0B1*dot_lineA01_lineA0B1) / lensq_lineA01;

			if (dist_sq_B1 > dist_eps_sq)			// 점 B1이 A라인에서 떨어져 있음..
				return false;

			float len_lineA01 = sqrtf(lensq_lineA01);
			float len_lineA01_req = 1.f / len_lineA01;

			float t0 = dot_lineA01_lineA0B0 * len_lineA01_req;
			float t1 = dot_lineA01_lineA0B1 * len_lineA01_req;

			// a0 - a1 라인에서 인접면은 b1 - b0로 맞닿을때...서로 교차하지 않으면 false임...
			return  t0 >= edge_epsilon && t1 <= len_lineA01 - edge_epsilon;

//			if (t0 >= edge_epsilon && t1 <= len_lineA01 - edge_epsilon)
//				return true;
//			if (t1 >= edge_epsilon && t0 <= len_lineA01 - edge_epsilon)
//				return true;
//			return false;
		}

	

	}


	quad_adj_collection* create_quad_adj_collection(const i3::uint32_t* quad_index_arr, size_t num_quad_face,
		const vec3* vert_arr, size_t num_vert, float dist_epsilon, float edge_epsilon)
	{
		i3::svector<quad_adj> vec_quad_adj(4 * num_quad_face);		// 미리 확보..
		
		for (size_t face = 0; face < num_quad_face; ++face)
		{
			if (is_valid_polygon(quad_index_arr, face) == false)
				continue;

			const size_t num_edge = get_num_vertex_of_polygon(quad_index_arr, face);

			for (i3::uint32_t edge = 0; edge < num_edge; ++edge)		// 엣지 하나씩..
			{
				i3::uint32_t a_edge_pos0 = quad_index_arr[face * 4 + edge];
				i3::uint32_t a_edge_pos1 = quad_index_arr[face * 4 + ((edge + 1) % num_edge)];
			
				const vec3& va0 = vert_arr[a_edge_pos0];
				const vec3& va1 = vert_arr[a_edge_pos1];
				
				quad_adj& a_q_adj = vec_quad_adj[face * 4 + edge];

				quad_adj::adj_edge_pair_t data_for_b(face, edge);

				// 나머지 모든 변과 조사해야됨... (계산량이 많긴 할것 같음)

				for (size_t face2 = face + 1; face2 < num_quad_face; ++face2)
				{
					if (is_valid_polygon(quad_index_arr, face2) == false)
						continue;

					const size_t num_edge2 = get_num_vertex_of_polygon(quad_index_arr, face2);

					for (i3::uint32_t edge2 = 0; edge2 < num_edge2; ++edge2)		// 엣지 하나씩..
					{
						i3::uint32_t b_edge_pos0 = quad_index_arr[face2 * 4 + edge2];
						i3::uint32_t b_edge_pos1 = quad_index_arr[face2 * 4 + ((edge2 + 1) % num_edge2)];
					
						const vec3& vb0 = vert_arr[b_edge_pos0];
						const vec3& vb1 = vert_arr[b_edge_pos1];

						quad_adj& b_q_adj = vec_quad_adj[face2 * 4 + edge2];

						// 4개의 점이 각각 무한직선 안에 존재하고 교차상태인지 점검해야함..무한직선안에 존재하지 않으면...
						// 인접면이 아니고.....무한직선 안에 있더라도 서로 교차하지 않으면 인접면이 아니다... 
						// 인접면이 확인되면 양쪽에 모두 필요정보를 다 넣어주어야한다...(중복제거같은것 없이 모두 넣는다..)
						if (is_line_adjacent(va0, va1, vb0, vb1, dist_epsilon, edge_epsilon))
						{
							quad_adj::adj_edge_pair_t data_for_a(face2, edge2);
							a_q_adj.vec_adj_edge_pair.push_back(data_for_a);
							b_q_adj.vec_adj_edge_pair.push_back(data_for_b);
						}
					}
				}
			}

		}

		return new quad_adj_collection(vec_quad_adj);
	}

	quad_adj_collection::quad_adj_collection(i3::svector<quad_adj>& vec_quad_adj_for_swap)
	{
		i3::swap(m_quad_adj_list, vec_quad_adj_for_swap);
	}

	quad_adj_collection::~quad_adj_collection()
	{

	}



	void safe_delete_quad_adj_collection(quad_adj_collection*& c)
	{
		delete c;  c = nullptr;
	}
	
}