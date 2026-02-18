#pragma once

#include "../structure/vec3.h"

namespace i3
{
	//
	// 메시 정보(여기서는 정점(pos only)과 인덱스목록)을 토대로 쿼드들을 추출해낸다...
	// 

	//
	// 쿼드는 4정점 기준이 되어야하고, 
	// 0. 정점목록 상태는 유지되는 것으로 간주...
	// 1. 정점 인덱스 각 4개들을 쿼드로 잘 추려서, 쿼드 1개를 구성하고, 쿼드 리스트에 등록...
	// 2. 각 쿼드는 인접한 쿼드의 쿼드인덱스를 ( 최소 0개~ 최대 4개)를 갖고 있어야함...
	// 
	// 쿼드 조건 : 폴리건에서 가장 직사각형에 근접한 폴리건이면서 평면식 동일해야함...
	//           평면노멀이 같지 않다면 그다음 긴 변을 찾아서 평면식 동일성을 점검한다..
	//           변이 인접한 폴리건의 경우 평면식동일성은 각 성분의 부호가 동일하다는 가정하에 평면노멀만 검사해도 된다..
	//			 직사각형 근접요건 : 각 변간 각도가 90도에 근접한지 여부를 보고 따진다..
	//							  가장 직사각형에 가까운 것으로 채택된 인접폴리건으로 쿼드를 결정...


	// 위와 같이 팬 아닌 스트립한 순서로 0번 뺀, 1-2변을 공유한 쿼드로 가정하고 점수 함수 작성..
	//

	float		calc_quad_quality_score(const i3::uint32_t* index_arr,
	const vec3* vert_arr, i3::uint32_t face0, i3::uint32_t face1, i3::uint32_t face0_adj_edge_idx, i3::uint32_t face1_adj_edge_idx);



	// 쿼드 인덱스 리스트를 만듬...
	// 두번째 출력인수는 쿼드 각 4 모서리의 인접쿼드인덱스를 넣어두는 adj리스트이다...(0-1,1-2,2-3,3-0으로 매핑)

	void		build_quad_index_list(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert,
		i3::vector<i3::uint32_t>& out_quad_vert_idx_list,
		i3::vector<i3::uint32_t>& out_quad_adj_list, const i3::uint32_t* in_adj = nullptr);
			
	void		build_quad_index_list(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert,
		i3::vector<i3::uint32_t>& out_quad_vert_idx_list, const i3::uint32_t* in_adj = nullptr);




}