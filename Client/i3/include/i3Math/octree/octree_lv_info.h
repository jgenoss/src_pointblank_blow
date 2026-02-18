#pragma once

//#if 0

#pragma warning(push)
#pragma warning(disable : 4201)

namespace i3
{
	struct word_aabb;

	union __declspec(align(8)) octree_lv_info
	{
		struct { WORD lv, x, y, z; };
		UINT64	info64;
	};

	inline bool		operator==( const octree_lv_info& l, const octree_lv_info& r) { return l.info64 == r.info64; }
	inline bool		operator!=( const octree_lv_info& l, const octree_lv_info& r) { return l.info64 != r.info64; }

	octree_lv_info calc_lv_info(const word_aabb& wa);
	octree_lv_info calc_lv_info(WORD minX, WORD minY, WORD minZ, WORD maxX, WORD maxY, WORD maxZ);
}


#pragma warning(pop)

//#endif