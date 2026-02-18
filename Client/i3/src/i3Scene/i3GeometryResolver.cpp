#include "i3SceneDef.h"
#include "i3GeometryResolver.h"

I3_CLASS_INSTANCE( i3GeometryResolver);


i3GeometryResolver::~i3GeometryResolver(void)
{
	setGeometryAttr( nullptr);
}

void i3GeometryResolver::setGeometryAttr( i3GeometryAttr * pAttr)
{
	if( m_pVA != nullptr)
		m_pVA->Unlock();

	if( m_pIA != nullptr)
		m_pIA->Unlock();

	I3_REF_CHANGE( m_pGeoAttr, pAttr);

	if( m_pGeoAttr != nullptr)
	{
		I3_REF_CHANGE( m_pVA, m_pGeoAttr->GetVertexArray());
		I3_REF_CHANGE( m_pIA, m_pGeoAttr->GetIndexArray());

		m_idxStart = pAttr->GetStartIndex();

		switch( m_pGeoAttr->GetPrimitiveType())
		{
			case I3G_PRIM_TRILIST :
			case I3G_PRIM_TRISTRIP :
			case I3G_PRIM_TRIFAN :
				m_bLinePrim = false;
				break;

			default :
				m_bLinePrim = true;
				break;
		}
	}
	else
	{
		I3_SAFE_RELEASE( m_pVA);
		I3_SAFE_RELEASE( m_pIA);

		m_idxStart = 0;
		m_bLinePrim = false;
	}

	if( m_pVA != nullptr)
		m_pVA->Lock( 0);

	if( m_pIA != nullptr)
		m_pIA->Lock( 0);
}

INT32 i3GeometryResolver::getPrimitiveCount(void)
{
	INT32 i;
	UINT32 accm;

	if( m_pGeoAttr == nullptr)
		return 0;

	if( m_pGeoAttr->IsPrimitiveLength())
	{
		if( m_pGeoAttr->GetPrimitiveCount() <= 1)
		{
			if( m_pIA == nullptr)
			{
				return m_pVA->GetCount() - 2;
			}
			else
			{
				return m_pIA->GetCount() - 2;
			}
		}
		else
		{
			accm = 0;

			for( i = 0; i < (INT32) m_pGeoAttr->GetPrimitiveCount(); i++)
			{
				accm += m_pGeoAttr->GetPrimitiveLength( i);
			}

			return accm;
		}
	}

	return m_pGeoAttr->GetPrimitiveCount();
}

inline void _getStripIndex( INT32 idxTri, INT32 startidx, INT32 * pIdx)
{
	if( idxTri & 1)
	{
		// 홀
		pIdx[0] = startidx + (idxTri);
		pIdx[1] = startidx + (idxTri + 2);
		pIdx[2] = startidx + (idxTri + 1);
	}
	else
	{
		// 짝
		pIdx[0] = startidx + (idxTri);
		pIdx[1] = startidx + (idxTri + 1);
		pIdx[2] = startidx + (idxTri + 2);
	}
}

void i3GeometryResolver::getVertexIndices( INT32 idx, INT32 * pIndices)
{
	INT32 idxVtx;

	switch( getPrimitiveType())
	{
		case I3G_PRIM_LINELIST :
			idxVtx = m_idxStart + (idx * 2);

			if( m_pIA != nullptr)
			{
				pIndices[0] = m_pIA->GetIndex( idxVtx);
				pIndices[1] = m_pIA->GetIndex( idxVtx + 1);
			}
			else
			{
				pIndices[0] = idxVtx;
				pIndices[1] = idxVtx + 1;
			}
			break;

		case I3G_PRIM_LINESTRIP	:
			{
				I3ASSERT_0;
			}
			break;

		case I3G_PRIM_TRILIST	:
			idxVtx = m_idxStart + (idx * 3);

			if( m_pIA != nullptr)
			{
				pIndices[0] = m_pIA->GetIndex( idxVtx);
				pIndices[1] = m_pIA->GetIndex( idxVtx + 1);
				pIndices[2] = m_pIA->GetIndex( idxVtx + 2);
			}
			else
			{
				pIndices[0] = idxVtx;
				pIndices[1] = idxVtx + 1;
				pIndices[2] = idxVtx + 2;
			}
			break;

		case I3G_PRIM_TRISTRIP	:
		case I3G_PRIM_TRIFAN	:
			{
				idxVtx = m_idxStart + (idx * 3);

				INT32 accm = m_idxStart;

				if( m_pGeoAttr->IsPrimitiveLength())
				{
					// 해당 Index의 Triangle이 속해있는 Drawing Group을 찾는다.
					for( INT32 i = 0; i < (INT32) m_pGeoAttr->GetPrimitiveCount(); i++)
					{
						if( (accm + (INT32) m_pGeoAttr->GetPrimitiveLength( i)) > idx)
							break;

						accm += m_pGeoAttr->GetPrimitiveLength( i);
					}

				}

				_getStripIndex( idx, accm, pIndices);
			}
			break;
	}
}

void i3GeometryResolver::getPositions( INT32 idx, VEC3D * pPos)
{
	INT32 vidx[3];

	getVertexIndices( idx, vidx);

	m_pVA->GetPosition( vidx[0], pPos);
	m_pVA->GetPosition( vidx[1], pPos + 1);

	if( m_bLinePrim == false)
	{	
		m_pVA->GetPosition( vidx[2], pPos + 2);
	}
}

void i3GeometryResolver::getNormals( INT32 idx, VEC3D * pNormal)
{
	INT32 vidx[3];

	getVertexIndices( idx, vidx);

	m_pVA->GetNormal( vidx[0], pNormal);
	m_pVA->GetNormal( vidx[1], pNormal + 1);

	if( m_bLinePrim == false)
		m_pVA->GetNormal( vidx[2], pNormal + 2);
}

void i3GeometryResolver::getTexCoords( INT32 idx, VEC2D * pUV)
{
	INT32 vidx[3];

	getVertexIndices( idx, vidx);

	m_pVA->GetTextureCoord( 0, vidx[0], pUV);
	m_pVA->GetTextureCoord( 0, vidx[1], pUV + 1);

	if( m_bLinePrim == false)
		m_pVA->GetTextureCoord( 0, vidx[2], pUV + 2);
}
