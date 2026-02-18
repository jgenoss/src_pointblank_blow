#pragma once

//#if 0
//#include "octree_lv_info.h"

#pragma warning(push)
#pragma warning(disable : 4201)

namespace i3
{
	class aabb;
	struct vec3;

	// __declspec(align(16))

	struct  word_aabb
	{
		word_aabb() : aabb64_1(0), aabb64_2(0) {}
		word_aabb(const word_aabb& a) : aabb64_1(a.aabb64_1), aabb64_2(a.aabb64_2) {}
		word_aabb& operator=(const word_aabb& a) { aabb64_1 = a.aabb64_1;	aabb64_2 = a.aabb64_2; return *this; }
		word_aabb& operator=(int) { aabb64_1 = 0; aabb64_2 = 0; return *this; }

		union
		{
			struct
			{
				WORD			x0, y0, z0;
				WORD			x1, y1, z1;
			//	octree_lv_info	lv_info;		// union제약때문에 octree_lv_info에 생성자를 두면 안됨..
			};
			
			struct 
			{
				UINT64	aabb64_1, aabb64_2;
			};
		};

	};
	
	inline bool		operator== ( const word_aabb& l, const word_aabb& r)  
	{	
		return l.aabb64_1 == r.aabb64_1 && l.aabb64_2 == r.aabb64_2; 
	}

	inline bool		operator!= ( const word_aabb& l, const word_aabb& r ) 
	{ 
		return l.aabb64_1 != r.aabb64_1 || l.aabb64_2 != r.aabb64_2; 
	}

	void calc_world_aabb_scale_offset(const i3::aabb& world_aabb, vec3& out_scale, vec3& out_offset);

	// word_aabb 계산시 level_info까지 함께 포함해서 계산해둔다...
	bool convert_aabb_to_word_aabb(const i3::aabb& input, const vec3& scale, const vec3& offset,
									word_aabb& out);

}

#pragma warning(pop)

//#endif