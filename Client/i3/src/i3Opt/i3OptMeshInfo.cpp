#include "i3OptPCH.h"
#include "i3OptMeshInfo.h"
#include "i3GeometrySplitter.h"

void MESH_INFO::setNode( i3Node * pNode)
{
	// i3Geometry Leaf Node가 나올 때까지 돌면서 Transform을 확인한다.
	I3_REF_CHANGE( m_pNode, pNode);
}

void MESH_INFO::refresh(void)
{
	INT32 i;
	i3Node * pTemp = m_pNode;

	i3Matrix::Identity( &m_Transform);

	m_pGeo = nullptr;

	// World-Space에서의 BoundBox로 갱신.
	m_Bound.Reset();

	while( pTemp->GetChildCount() > 0)
	{
		if( i3::kind_of< i3Transform*>(pTemp))
		{
			i3Transform * pTrans = (i3Transform *) pTemp;

			i3Matrix::Mul( &m_Transform, pTrans->GetMatrix(), &m_Transform);
		}
		else if( i3::kind_of<i3Geometry*>(pTemp))
		{
			i3BoundBox * pBox = pTemp->GetBound();
			VEC3D pos[8];
			INT32 i;

			if( pBox == nullptr)
			{
				pBox = i3BoundBox::new_object_ref();

				pTemp->GetWrappingBound( pBox);

				pTemp->SetBound( pBox);
			}

			pBox->GetCornerVertices( pos);

			i3Matrix::TransformPoints( pos, pos, 8, &m_Transform);

			for( i = 0; i < 8; i++)
			{
				m_Bound.ExtendByVec( &pos[i]);
			}

			m_pGeo = (i3Geometry *) pTemp;
		}

		pTemp = pTemp->GetChild( 0);
	}

	if( i3::kind_of<i3Geometry*>(pTemp))
	{
		i3BoundBox * pBox = pTemp->GetBound();
		VEC3D pos[8];
		INT32 i;

		if( pBox == nullptr)
		{
			pBox = i3BoundBox::new_object_ref();

			pTemp->GetWrappingBound( pBox);

			pTemp->SetBound( pBox);
		}

		pBox->GetCornerVertices( pos);

		i3Matrix::TransformPoints( pos, pos, 8, &m_Transform);

		for( i = 0; i < 8; i++)
		{
			m_Bound.ExtendByVec( &pos[i]);
		}

		m_pGeo = (i3Geometry *) pTemp;
	}

	I3ASSERT_RETURN( m_pGeo != nullptr);

	// Triangle Count 갱신
	for( i = 0, m_TriCount = 0; i < m_pGeo->GetGeometryAttrCount(); i++)
	{
		i3RenderAttr * pAttr = m_pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr*>(pAttr))
			continue;

		i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

		m_TriCount += pGeoAttr->GetPrimitiveCount();
	}
}

MESH_INFO *		MESH_INFO::Cut( VEC4D * pPlane)
{
	INT32 i, triCount;

	if( m_pGeo == nullptr)
		return nullptr;

	i3GeometrySplitter splitter;

	MESH_INFO * pOther = nullptr;
	
	for( i = 0; i < m_pGeo->GetGeometryAttrCount(); i++)
	{
		i3RenderAttr * pAttr = m_pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr* >(pAttr))
			continue;

		i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

		splitter.Split( pGeoAttr->GetVertexArray(), &m_Transform, pPlane);

		if( splitter.getExcCount() > 0)
		{
			// 평면의 바깥쪽으로 분류된 geometry를 따로 모아 반환한다.
			if( pOther == nullptr)
			{
				pOther = new MESH_INFO;

				{
					i3Node * pClone = i3Scene::CreateClone( m_pNode, I3_COPY_INSTANCE);

					pOther->setNode( pClone);
					pOther->refresh();
				}

				pOther->m_pGeo->RemoveAllGeometryAttr();
			}

			i3GeometryAttr * pOtherGeo = i3GeometryAttr::new_object_ref();

			triCount = splitter.getExcCount();
			pOtherGeo->SetVertexArray( splitter.getExcBuffer(), I3G_PRIM_TRILIST, triCount);

			pOther->m_pGeo->AppendGeometryAttr( pOtherGeo);
		}

		// 평면의 안쪽으로 분류된 Geometry에 대한 처리.
		if( splitter.getIncCount() > 0)
		{
			// 분류된 Geometry들을 새롭게 갱신.
			triCount = splitter.getIncCount();
			pGeoAttr->SetVertexArray( splitter.getIncBuffer(), I3G_PRIM_TRILIST, triCount);
		}
		else
		{
			// 안쪽으로 분류된 Geometry가 없다!
			// 이 MESH_INFO는 사라져도 된다.
			m_pGeo->RemoveGeometryAttr( pGeoAttr);
			i--;
		}
	}

	refresh();

	if( pOther != nullptr)
	{
		pOther->refresh();
		pOther->m_idxPlace[0] = m_idxPlace[0];
		pOther->m_idxPlace[1] = m_idxPlace[1];
		pOther->m_idxPlace[2] = m_idxPlace[2];
	}

	return pOther;
}

INT32 MESH_INFO::getOrdinalIndex(void)
{
	INT32 idxChild = 0;

	// Y 축.
	// Positive, NA -> Up side
	// Negative -> Down side
	if( m_idxPlace[1] == PLACE_NEGATIVE)				// Down Side에 있다.
		idxChild += 4;

	// X 축
	// Negative, NA -> Left side
	// Positive -> Right side
	if( m_idxPlace[0] == PLACE_POSITIVE)
		idxChild += 2;

	// Z 축
	// Negative, NA -> Front side
	// Positive -> Back side
	if( m_idxPlace[2] == PLACE_POSITIVE)
		idxChild += 1;

	return idxChild;
}

void MESH_INFO::setOrdinalIndex( INT32 idx)
{
	static PLACE_TAG s_PlaceTable[8][3] =
	{
		//	X					Y					Z
		{ PLACE_NEGATIVE,	PLACE_POSITIVE,		PLACE_NEGATIVE },		// TOP_LEFT_FRONT = 0,		// 0
		{ PLACE_NEGATIVE,	PLACE_POSITIVE,		PLACE_POSITIVE },		// TOP_LEFT_BACK,			// 1
		{ PLACE_POSITIVE,	PLACE_POSITIVE,		PLACE_NEGATIVE },		// TOP_RIGHT_FRONT,			// etc...
		{ PLACE_POSITIVE,	PLACE_POSITIVE,		PLACE_POSITIVE },		// TOP_RIGHT_BACK,
		{ PLACE_NEGATIVE,	PLACE_NEGATIVE,		PLACE_NEGATIVE },		// BOTTOM_LEFT_FRONT,
		{ PLACE_NEGATIVE,	PLACE_NEGATIVE,		PLACE_POSITIVE },		// BOTTOM_LEFT_BACK,
		{ PLACE_POSITIVE,	PLACE_NEGATIVE,		PLACE_NEGATIVE },		// BOTTOM_RIGHT_FRONT,
		{ PLACE_POSITIVE,	PLACE_NEGATIVE,		PLACE_POSITIVE },		// BOTTOM_RIGHT_BACK,
	};
	
	I3_BOUNDCHK( idx, 8);

	m_idxPlace[0] = s_PlaceTable[idx][0];
	m_idxPlace[1] = s_PlaceTable[idx][1];
	m_idxPlace[2] = s_PlaceTable[idx][2];
}

#if defined( I3_DEBUG)
void MESH_INFO::VLog( bool bSideColor)
{
	INT32 i, j;
	VEC3D pos[3];
	i3VertexArray * pVA;
	INT32 idxChild;
	REAL32 a;

	if( bSideColor)
	{
		idxChild = 0;

		// Y 축.
		// Positive, NA -> Up side
		// Negative -> Down side
		if( m_idxPlace[1] == PLACE_NEGATIVE)				// Down Side에 있다.
			idxChild += 4;

		// X 축
		// Negative, NA -> Left side
		// Positive -> Right side
		if( m_idxPlace[0] == PLACE_POSITIVE)
			idxChild += 2;

		// Z 축
		// Negative, NA -> Front side
		// Positive -> Back side
		if( m_idxPlace[2] == PLACE_POSITIVE)
			idxChild += 1;

		a = 0.5f;
		switch( idxChild)
		{
			case 0 :	i3VLog::SetColor( 1.0f, 0.0f, 0.0f, a);	break;
			case 1 :	i3VLog::SetColor( 0.0f, 0.0f, 1.0f, a);	break;
			case 2 :	i3VLog::SetColor( 0.0f, 1.0f, 0.0f, a);	break;
			case 3 :	i3VLog::SetColor( 1.0f, 1.0f, 0.0f, a);	break;

			case 7 :	i3VLog::SetColor( 0.5f, 0.0f, 0.0f, a);	break;
			case 6 :	i3VLog::SetColor( 0.0f, 0.0f, 0.5f, a);	break;
			case 5 :	i3VLog::SetColor( 0.0f, 0.5f, 0.0f, a);	break;
			case 4 :	i3VLog::SetColor( 0.5f, 0.5f, 0.0f, a);	break;
		}
	}

	for( i = 0; i < m_pGeo->GetGeometryAttrCount(); i++)
	{
		i3RenderAttr * pAttr = m_pGeo->GetGeometryAttr( i);

		if( !i3::kind_of<i3GeometryAttr*>(pAttr))
			continue;

		i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

		pVA = pGeoAttr->GetVertexArray();

		pVA->Lock( I3G_LOCK_READONLY);

		for( j = 0; j < (INT32) pGeoAttr->GetPrimitiveCount(); j ++)
		{
			pVA->GetPositions( (j * 3), 3, pos);

			i3Matrix::TransformPoints( pos, pos, 3, &m_Transform);

			i3VLog::Tri( &pos[0], &pos[1], &pos[2]);
		}

		pVA->Unlock();
	}
}
#endif
