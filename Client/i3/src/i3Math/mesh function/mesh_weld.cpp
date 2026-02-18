#include "i3MathType.h"

#include "i3Math/mesh_function/mesh_weld.h"
#include "i3Math/mesh_function/mesh_adjacency.h"

#include "i3Base/smart_ptr/scoped_array.h"
// Wine코드를 참고해서 이것저것 수정..

namespace i3
{
	bool weld_index_buffer(const i3::uint32_t* index_arr, size_t num_face,
					const vec3* vert_arr, size_t num_vert, i3::uint32_t* out_index_arr,
		i3::uint32_t* out_vert_remap, size_t& out_num_vert, const i3::uint32_t* in_point_reps)
	{
		const i3::uint32_t* pointrep_arr = in_point_reps;

		i3::scoped_array<i3::uint32_t> pointrep_allocated;

		out_num_vert = num_vert;

		if (pointrep_arr == nullptr)
		{
			pointrep_allocated.reset(new i3::uint32_t[num_vert]);
			pointrep_arr = pointrep_allocated.get();

			bool result = i3::generate_pointreps(
				index_arr, num_face, vert_arr, num_vert, 0.f, pointrep_allocated.get());

			if (!result) return false;
		}

		// 0. 인덱스에 중복된 정점은 대표정점으로 모두 정리..
		const size_t num_indices = num_face * 3;

		for (size_t i = 0; i < num_indices; ++i)
		{
			uint32_t vert_idx = index_arr[i];
			out_index_arr[i] = pointrep_arr[vert_idx];
		}

		// 1. vertex_remap 구성..
		i3::scoped_array<i3::uint32_t> vert_remap_allocated;
		i3::uint32_t* vert_remap = out_vert_remap;

		if (vert_remap == nullptr)
		{
			vert_remap_allocated.reset(new i3::uint32_t[num_vert]);
			vert_remap = vert_remap_allocated.get();
		}

		::memset(vert_remap, 0xFF, sizeof(i3::uint32_t) * num_vert);	// UNUSED로 모두 마킹 후..

		for (size_t i = 0; i < num_indices; ++i)
		{
			i3::uint32_t vert_idx = out_index_arr[i];

			if ( vert_idx == i3::uint32_t(-1))
				continue;
			if (vert_idx >= num_vert)		// 에러 상황이지만 continue;
				continue;

			vert_remap[vert_idx] = 1;		// 0아닌것으로만 마킹..
		}

		size_t num_new_vert = 0;
		for (size_t i = 0; i < num_vert; ++i)
		{
			if (vert_remap[i] != UNUSED32)
			{
				vert_remap[i] = num_new_vert;
				++num_new_vert;
			}
		}

		// 인덱스버퍼를 변경...(버텍스버퍼의 공백들이 합쳐질 예정이므로, 인덱스들이 좀더 낮은 번호들로 이동됨)
		for (size_t i = 0; i < num_indices; ++i)
		{
			i3::uint32_t vert_idx = out_index_arr[i];

			if (vert_idx == i3::uint32_t(-1))
				continue;
			if (vert_idx >= num_vert)		// 에러 상황이지만 continue;
				continue;

			out_index_arr[i] = vert_remap[vert_idx];
		}
		// 
		num_new_vert = 0;
		for (size_t i = 0; i < num_vert; ++i)
		{
			if ( vert_remap[i] != UNUSED32)
			{
				vert_remap[num_new_vert] = i;
				++num_new_vert;
			}
		}

		for (size_t i = num_new_vert; i < num_vert; ++i)
		{
			vert_remap[i] = UNUSED32;
		}

		out_num_vert = num_new_vert;
				
		return true;
	}

	void remap_vertices(const vec3* in_vert_arr, size_t num_new_vert,
		const i3::uint32_t* vert_remap, vec3* out_new_vert_arr)
	{
		for (size_t i = 0; i < num_new_vert; ++i)
		{
			out_new_vert_arr[i] = in_vert_arr[vert_remap[i]];
		}

	}

}