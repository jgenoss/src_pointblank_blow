#pragma once

#include "int_structure.h"
#include "plane.h"
#include <WinDef.h>

namespace i3
{
	namespace detail
	{
		struct box_plane_np_lookup
		{
			static const int3 n_vert[8];
			static const int3 p_vert[8];
		};
	}
	
	inline DWORD calc_box_plane_lookup_idx(bool is_a_positive, bool is_b_positive, bool is_c_positive)
	{
		return (DWORD(is_a_positive) << 2) + (DWORD(is_b_positive) << 1) + (DWORD(is_c_positive));
	}

	inline DWORD calc_box_plane_lookup_idx(const plane& pl)
	{
		return calc_box_plane_lookup_idx(pl.a > 0.f, pl.b > 0.f, pl.c > 0.f);
	}

	inline const int3& get_box_plane_n_vert(DWORD lookup_idx) { return detail::box_plane_np_lookup::n_vert[lookup_idx]; }
	inline const int3& get_box_plane_p_vert(DWORD lookup_idx) { return detail::box_plane_np_lookup::p_vert[lookup_idx]; }
}
