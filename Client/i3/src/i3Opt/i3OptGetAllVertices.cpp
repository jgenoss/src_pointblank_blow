#include "i3OptPCH.h"
#include "i3OptGetAllVertices.h"

I3_CLASS_INSTANCE( i3OptGetAllVertices);

i3OptGetAllVertices::i3OptGetAllVertices(void)
{
	m_Class = CLASS_GEOMETRY;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

i3OptGetAllVertices::~i3OptGetAllVertices(void)
{
	I3_SAFE_RELEASE( m_pVA);
}

bool i3OptGetAllVertices::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( !i3::same_of<i3Geometry* >(pNode))
		return true;

	// Parent를 따라가며, Volatile Node가 아닌지 확인한다.
	{
		i3Node * pTemp = pNode;

		while( pTemp != nullptr)
		{
			if( pTemp->IsFlag( I3_NODEFLAG_VOLATILE))
				return true;

			pTemp = pTemp->GetParent();
		}
	}

	i3Geometry * pGeo = (i3Geometry *) pNode;
	INT32 i, j;
	i3GeometryAttr * pGeoAttr;
	VEC3D pos;

	for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
	{
		pGeoAttr = (i3GeometryAttr *) pGeo->GetGeometryAttr( i);
		i3VertexArray * pVA = pGeoAttr->GetVertexArray();
	
		if( m_pVA == nullptr)
		{
			// Vertex의 개수 계산
			m_Count += pVA->GetCount();
		}
		else
		{
			pVA->Lock( );

			for( j = 0; j < (INT32) pVA->GetCount(); j++)
			{
				pVA->GetPosition( j, &pos);

				i3Vector::TransformCoord( &pos, &pos, m_pStack->GetTop());

				m_pVA->SetVector( m_pVA->GetVectorCount(), &pos);
				m_pVA->_forceSetVectorCount( m_pVA->GetVectorCount() + 1);
			}

			pVA->Unlock();
		}
	}

	return true;
}

void i3OptGetAllVertices::Trace( i3Node * pNode)
{
	m_Count = 0;
	SetMatrixStackEnable(true);

	I3_SAFE_RELEASE( m_pVA);

	i3SceneOptimizer::Trace( pNode);		// 개수를 구한다.

	if( m_Count <= 0)
		return;

	m_pVA = i3Vector3Array::new_object();
	m_pVA->SetVectorCount( m_Count);
	m_pVA->_forceSetVectorCount( 0);

	i3SceneOptimizer::Trace( pNode);
}
