#pragma once

#include "../structure/vec3.h"
//#include <stdint.h>

namespace i3
{
		
	bool weld_index_buffer(const i3::uint32_t* index_arr, size_t num_face,
		const vec3* vert_arr, size_t num_vert, i3::uint32_t* out_index_arr,
		i3::uint32_t* out_vert_remap, size_t& out_num_vert,
		const i3::uint32_t* in_point_reps = nullptr);

	void remap_vertices(const vec3* in_vert_arr, size_t num_new_vert,
		const i3::uint32_t* vert_remap, vec3* out_new_vert_arr);

}