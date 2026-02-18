#pragma once

//#include <stdint.h>

#include "i3Base/itl/svector.h"

namespace i3
{
	struct vec3;

	struct quad_adj
	{
		typedef i3::pair<i3::uint32_t, i3::uint32_t>	adj_edge_pair_t;
		i3::svector<adj_edge_pair_t> vec_adj_edge_pair;
	};
	
	class quad_adj_collection
	{
	public:
		quad_adj_collection(i3::svector<quad_adj>& vec_quad_adj_for_swap);
		~quad_adj_collection();
		
		i3::uint32_t get_adjacent_quad_count(size_t quad_face, size_t edgeIdx) const { return m_quad_adj_list[quad_face*4 + edgeIdx].vec_adj_edge_pair.size(); }
		i3::uint32_t get_adjacent_quad_face(size_t quad_face, size_t edgeIdx, size_t idx) const { return m_quad_adj_list[quad_face * 4 + edgeIdx].vec_adj_edge_pair[idx].first; }
		i3::uint32_t get_adjacent_quad_edge(size_t quad_face, size_t edgeIdx, size_t idx) const { return m_quad_adj_list[quad_face * 4 + edgeIdx].vec_adj_edge_pair[idx].second; }
		
	private:
		i3::svector<quad_adj>	m_quad_adj_list;				// 쿼드*엣지(4) 당 쿼드 quad_adj 구조체 하나로 설정..
	};															// 삼각형의 경우 네번째 것은 그냥 비워둔다...


	quad_adj_collection* create_quad_adj_collection(const i3::uint32_t* quad_index_arr, size_t num_quad_face,
		const vec3* vert_arr, size_t num_vert, float dist_epsilon, float edge_epsilon);
	
	void				 safe_delete_quad_adj_collection(quad_adj_collection*& c);

}