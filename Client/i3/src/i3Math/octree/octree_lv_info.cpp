#include "i3MathType.h"

//#if 0

#include "octree/octree_lv_info.h"
#include "octree/octree_constant.h"
#include "octree/word_aabb.h"

namespace i3
{
	octree_lv_info calc_lv_info(const word_aabb& wa)
	{
		return calc_lv_info(wa.x0, wa.y0, wa.z0, wa.x1, wa.y1, wa.z1);
	}

	octree_lv_info calc_lv_info(WORD minX, WORD minY, WORD minZ, WORD maxX, WORD maxY, WORD maxZ)
	{
		
		DWORD xPatt = DWORD(minX ^ maxX);
		DWORD yPatt = DWORD(minY ^ maxY);
		DWORD zPatt = DWORD(minZ ^ maxZ);

		DWORD Patt	= (xPatt > yPatt) ? xPatt : yPatt;
		Patt		= ( Patt > zPatt) ? Patt  : zPatt;

		DWORD dwLevel = MAX_OCTREE_DEPTH;

		if (Patt)									// Patt == 0이라면 최고 레벨...
		{
			DWORD msb;		i3::msb32( msb, Patt);	
			dwLevel = MAX_OCTREE_DEPTH - msb - 1;
		}

		octree_lv_info result;
		result.lv = WORD(dwLevel);

		DWORD shift = MAX_OCTREE_DEPTH - dwLevel;
		result.x = WORD(minX >> shift);
		result.y = WORD(minY >> shift);
		result.z = WORD(minZ >> shift);

		return result;		
	}
	
}

//#endif
