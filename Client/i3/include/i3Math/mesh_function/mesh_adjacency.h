#pragma once

#include "../structure/vec3.h"
//#include <stdint.h>

namespace i3
{
	// 기본버전 : 먼저 정점값비교를 통한 실제로 중첩 버텍스 제거된 point_rep 계산 후, 인접 폴리건 인덱스를 구하는 함수..
	bool generate_adjacency(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon, i3::uint32_t* out_adj);
	bool generate_adjacency(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon, i3::uint32_t* out_adj);


	// 기본버전에서 point_rep 정보까지 리턴받아서 처리..
	bool generate_adjacency_and_pointreps(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon,
		i3::uint32_t* out_adj, i3::uint32_t* out_rep);
	bool generate_adjacency_and_pointreps(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon,
		i3::uint32_t* out_adj, i3::uint32_t* out_rep);


	bool generate_pointreps(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon, i3::uint32_t* out_rep);
	bool generate_pointreps(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon, i3::uint32_t* out_rep);
	
	// point_rep 정보를 미리 알고 있는 경우, 인접 폴리건 인덱스를 구하는 함수..
	bool generate_adjacency_with_pointreps(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, const i3::uint32_t* pointreps, i3::uint32_t* out_adj);
	bool generate_adjacency_with_pointreps(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, const i3::uint32_t* pointreps, i3::uint32_t* out_adj);

	// point_rep계산과 정보를 모두 처리하지 않고, 인접폴리건 인덱스만 구하는 함수..
	bool generate_adjacency_with_no_pointreps(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, i3::uint32_t* out_adj);
	bool generate_adjacency_with_no_pointreps(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, i3::uint32_t* out_adj);

	// 
	void extract_pointreps_from_adjacency(const i3::uint16_t* index_arr, size_t num_face, size_t num_vert, const i3::uint32_t* in_adj, i3::uint32_t* out_rep);
	void extract_pointreps_from_adjacency(const i3::uint32_t* index_arr, size_t num_face, size_t num_vert, const i3::uint32_t* in_adj, i3::uint32_t* out_rep);
	

	// 생성 헬퍼들...
	i3::uint32_t*		new_pointreps(size_t num_vert);
	void			delete_pointreps(i3::uint32_t* pointreps);

	i3::uint32_t*		new_adjacency(size_t num_face);
	void			delete_adjacency(i3::uint32_t* adj);


}