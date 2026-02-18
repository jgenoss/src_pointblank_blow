#include "i3FrameworkPCH.h"
#include "i3VirtualTexture.h"

namespace vt_detail
{
	I3VT_TILE * getTileByIndex(const i3VirtualTextureData* pData, INT32 xidx, INT32 yidx);
}

bool i3VirtualTexture::Validate_Legacy( I3VT_TEXINFO * pTex, VEC4D * pRect)
{
	return m_pCache->Validate_Legacy( pTex, pRect);
}

I3VT_TEXINFO*	i3VirtualTexture::GetRelatedRefTexture( REAL32 x, REAL32 y)
{
	REAL32 tx, ty;
	INT32 sx, sy;

	tx = x * getWidth();
	ty = y * getHeight();

	sx = (INT32)(tx / I3VT_TILE_SIZE);
	sy = (INT32)(ty / I3VT_TILE_SIZE);

	I3VT_TILE * pTile = vt_detail::getTileByIndex(&m_Data, sx, sy);

	return pTile->getTexInfo();
}

void i3VirtualTexture::SortUsedTextureList( i3::vector<INT32>& TexIdxList)
{
	
	INT32 idx0, idx1;
	I3VT_TEXINFO * pTex0, * pTex1;

	if ( !TexIdxList.empty() )
	{
		for(size_t i = 0; i < (TexIdxList.size() - 1); i++)
		{
			for(size_t j = i + 1; j <TexIdxList.size(); j++)
			{
				idx0 = TexIdxList[i];
				idx1 = TexIdxList[j];

				pTex0 = getRefTexture( idx0);
				pTex1 = getRefTexture( idx1);

				INT32 span0 = pTex0->m_Width * pTex0->m_Height;
				INT32 span1 = pTex1->m_Width * pTex1->m_Height;

				if( span0 < span1)
				{
					TexIdxList[i] = idx1;
					TexIdxList[j] = idx0;
				}
			}
		}
	}
}
