#include "i3OptPCH.h"
#include "i3OptTriSplitNode.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3OptTriSplitNode);

//#define		VLOG_EDGE
//#define			VLOG_ADDTRI

i3OptTriSplitNode::~i3OptTriSplitNode(void)
{
	

	for(size_t i = 0; i < m_TriList.size(); i++)
	{
		I3OPT_TRISPLIT_INFO * pTri = m_TriList[i];

		delete pTri;
	}

	m_TriList.clear();
}

void i3OptTriSplitNode::AddTri( i3VertexArray * pVA, i3IndexArray * pIA, INT32 idx0, INT32 idx1, INT32 idx2)
{
	I3OPT_TRISPLIT_INFO * pNew;

	pNew = new I3OPT_TRISPLIT_INFO;

	pNew->m_idx[0]	= idx0;
	pNew->m_idx[1]	= idx1;
	pNew->m_idx[2]	= idx2;
	pNew->m_pVA		= pVA;
	pNew->m_pIA		= pIA;
	
	pVA->GetPosition( idx0, & pNew->m_Pos[0]);
	pVA->GetPosition( idx1, & pNew->m_Pos[1]);
	pVA->GetPosition( idx2, & pNew->m_Pos[2]);

#if defined( VLOG_ADDTRI)
	i3VLog::SetColor( 1.0f, 0.0f, 0.0f, 1.0f);
	i3VLog::Line( & pNew->m_Pos[0], & pNew->m_Pos[1]);
	i3VLog::Line( & pNew->m_Pos[1], & pNew->m_Pos[2]);
	i3VLog::Line( & pNew->m_Pos[2], & pNew->m_Pos[0]);
#endif

	m_TriList.push_back( pNew);
}


i3Geometry * i3OptTriSplitNode::CreateGatheredGeometry(void)
{
	i3Geometry * pNewGeo = i3Geometry::new_object_ref();
	i3GeometryAttr * pNewGeoAttr = i3GeometryAttr::new_object_ref();
	i3VertexArray * pNewVA = nullptr;
	i3VertexArray * pVA;
	i3IndexArray * pIA, * pNewIA = nullptr;
	i3::vector<i3VertexArray*>	vaList;
	i3::vector<i3IndexArray*>	iaList;

	if( getTriCount() <= 0)
		return nullptr;

	I3OPT_TRISPLIT_INFO * pTri = getTri( 0);

	pVA = pTri->m_pVA;
	pIA = pTri->m_pIA;

	if( pIA != nullptr)
	{
		// IndexArray가 있다면 Vertex는 새롭게 생성하지 않고 재사용한다.
		pNewIA = i3IndexArray::new_object_ref();
		pNewIA->Create( getTriCount() * 3, 0, pVA->GetCount());
		pNewIA->Lock();
	}
	else
	{
		pNewVA = i3VertexArray::new_object_ref();
		pNewVA->Create( pVA->GetFormat(), getTriCount() * 3, 0);
		pNewVA->Lock();
	}

	// Vertex 복사
	for(INT32 i = 0; i < getTriCount(); i++)
	{
		pTri = getTri( i);

		if( i3::vu::index_of(vaList, pTri->m_pVA) == vaList.size() )
		{
			// 원본 VertexArray를 Lock/Unlock 시키기 위해....
			vaList.push_back( pTri->m_pVA);
			pTri->m_pVA->Lock( I3G_LOCK_READONLY);
		}

		if( pTri->m_pIA != nullptr)
		{
			if( i3::vu::index_of(iaList, pTri->m_pIA) == iaList.size() )
			{
				iaList.push_back( pTri->m_pIA);
				pTri->m_pIA->Lock( I3G_LOCK_READONLY);
			}
				
			if (pNewIA)
			{
				pNewIA->SetIndex((i * 3) + 0, pTri->m_idx[0]);
				pNewIA->SetIndex((i * 3) + 1, pTri->m_idx[1]);
				pNewIA->SetIndex((i * 3) + 2, pTri->m_idx[2]);
			}
			else
			{
				I3ASSERT(pNewIA != nullptr);
			}
		}
		else
		{
			if (pNewVA)
			{
				pNewVA->CopyVertexFrom((i * 3) + 0, pTri->m_pVA, pTri->m_idx[0]);
				pNewVA->CopyVertexFrom((i * 3) + 1, pTri->m_pVA, pTri->m_idx[1]);
				pNewVA->CopyVertexFrom((i * 3) + 2, pTri->m_pVA, pTri->m_idx[2]);
			}
			else
			{
				I3ASSERT(pNewVA != nullptr);
			}
		}
	}

	if( pNewVA != nullptr)
		pNewVA->Unlock();

	if( pNewIA != nullptr)
	{
		pNewIA->Unlock();

		pNewGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, getTriCount());
		pNewGeoAttr->SetIndexArray( pNewIA);
	}
	else
	{
		pNewGeoAttr->SetVertexArray( pNewVA, I3G_PRIM_TRILIST, getTriCount());
	}

	pNewGeo->AppendGeometryAttr( pNewGeoAttr);

	// 원본 VertexArray들에 대해 Unlock을 해주어야 한다.
	for(size_t i = 0; i < vaList.size(); i++)
	{
		pVA = vaList[i];

		pVA->Unlock();
	}

	for(size_t i = 0; i < iaList.size(); i++)
	{
		pIA = iaList[i];

		pIA->Unlock();
	}

	return pNewGeo;
}
