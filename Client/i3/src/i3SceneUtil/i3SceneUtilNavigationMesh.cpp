#include "i3SceneUtil.h"

static i3GeometryAttr * _ConvertNavigationMesh( i3NavigationMesh * pMesh, REAL32 dY )
{
	i3GeometryAttr * pGeoAttr;
	i3Vector2Array * pPosArray;
	UINT32 i;

	pPosArray = pMesh->GetPositionArray();

	// GeometryAttr
	pGeoAttr = i3GeometryAttr::new_object_ref();

	// Vertex Array
	{
		i3VertexFormat format;
		i3VertexArray * pVA;

		format.SetHasPosition( true);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, pPosArray->GetVectorCount(), 0);

		pVA->Lock();

		for( i = 0; i < pPosArray->GetVectorCount(); i++ )
		{
			VEC3D vec3D;
			i3Vector::Convert( &vec3D, pPosArray->GetVector( i ), i3Vector::CONVERT_MODE_XZ );
			i3Vector::AddY( &vec3D, dY);

			pVA->SetPosition( i, &vec3D );
		}

		pVA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_TRILIST, pMesh->GetTriangleCount(), 0);
	}

	// Index Array
	{
		i3IndexArray * pIA;

		pIA = i3IndexArray::new_object_ref();

		pIA->Create( pMesh->GetTriangleCount() * 3, 0, pGeoAttr->GetVertexArray()->GetCount());

		pIA->Lock();

		for( i = 0; i < (INT32) pMesh->GetTriangleCount(); i++)
		{
			i3::pack::NAVMESH_TRI * pTri = pMesh->GetTriangle( i);

			pIA->SetIndex( (i * 3) + 0, pTri->nVertex[0]);
			pIA->SetIndex( (i * 3) + 1, pTri->nVertex[1]);
			pIA->SetIndex( (i * 3) + 2, pTri->nVertex[2]);
		}

		pIA->Unlock();

		pGeoAttr->SetIndexArray( pIA);
	}

	return pGeoAttr;
}

static i3GeometryAttr * _ConvertNavigationMeshWireFrame( i3NavigationMesh * pMesh, REAL32 dY )
{
	i3GeometryAttr * pGeoAttr;
	i3Vector2Array * pPosArray;
	UINT32 i;

	pPosArray = pMesh->GetPositionArray();

	// GeometryAttr
	{
		pGeoAttr = i3GeometryAttr::new_object_ref();

	}

	// Vertex Array
	{
		i3VertexFormat format;
		i3VertexArray * pVA;

		format.SetHasPosition( true);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, pPosArray->GetVectorCount(), 0);

		pVA->Lock();

		for( i = 0; i < pPosArray->GetVectorCount(); i++ )
		{
			VEC3D vec3D;
			i3Vector::Convert( &vec3D, pPosArray->GetVector( i ), i3Vector::CONVERT_MODE_XZ );
			i3Vector::AddY( &vec3D, dY);

			pVA->SetPosition( i, &vec3D );
		}

		pVA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, pMesh->GetTriangleCount() * 3, 0);
	}

	// Index Array
	{
		i3IndexArray * pIA;

		pIA = i3IndexArray::new_object_ref();

		pIA->Create( pMesh->GetTriangleCount() * 6, 0, pGeoAttr->GetVertexArray()->GetCount());

		pIA->Lock();

		for( i = 0; i < (INT32) pMesh->GetTriangleCount(); i++)
		{
			i3::pack::NAVMESH_TRI * pTri = pMesh->GetTriangle( i);

			pIA->SetIndex( (i * 6) + 0, pTri->nVertex[0]);
			pIA->SetIndex( (i * 6) + 1, pTri->nVertex[1]);

			pIA->SetIndex( (i * 6) + 2, pTri->nVertex[1]);
			pIA->SetIndex( (i * 6) + 3, pTri->nVertex[2]);

			pIA->SetIndex( (i * 6) + 4, pTri->nVertex[2]);
			pIA->SetIndex( (i * 6) + 5, pTri->nVertex[0]);
		}

		pIA->Unlock();

		pGeoAttr->SetIndexArray( pIA);
	}

	return pGeoAttr;
}

static i3GeometryAttr * _ConvertNavigationMiddlePoint( i3NavigationMesh * pMesh, REAL32 dY )
{
	
	i3GeometryAttr * pGeoAttr;
	i3Vector2Array * pPosArray;

	INT32 i;

	pPosArray = pMesh->GetPositionArray();

	// GeometryAttr
	{
		pGeoAttr = i3GeometryAttr::new_object_ref();
	}

	// Vertex Array
	{
		i3VertexFormat format;
		i3VertexArray * pVA;

		format.SetHasPosition( true);

		pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, pMesh->GetTriangleCount() * 2, 0);

		pVA->Lock();

		{
			VEC3D vecPos[3];
			VEC3D vecNorm;

			for( i = 0; i < (INT32) pMesh->GetTriangleCount(); i++)
			{
				i3::pack::NAVMESH_TRI * pTri = pMesh->GetTriangle( i);

				i3Vector::Convert( &vecPos[0], pPosArray->GetVector( pTri->nVertex[0] ), i3Vector::CONVERT_MODE_XZ );
				i3Vector::Convert( &vecPos[1], pPosArray->GetVector( pTri->nVertex[1] ), i3Vector::CONVERT_MODE_XZ );
				i3Vector::Convert( &vecPos[2], pPosArray->GetVector( pTri->nVertex[2] ), i3Vector::CONVERT_MODE_XZ );

				i3Vector::Normal( &vecNorm, &vecPos[0], &vecPos[1], &vecPos[2] );

				//i3Vector::Convert( &vecMid, &pTri->vecMiddle, i3Vector::CONVERT_MODE_XZ );
				VEC3D vecMid;
				i3Vector::Add( &vecMid, &vecPos[0], &vecPos[1] );
				i3Vector::Add( &vecMid, &vecMid, &vecPos[2] );
				i3Vector::Scale( &vecMid, &vecMid, 1.0f / 3.0f);

				i3Vector::AddY( &vecMid, dY);
				pVA->SetPosition( (i * 2), &vecMid );

				i3Vector::ScaleAdd( &vecMid, &vecNorm, 1.0f);
				pVA->SetPosition( (i * 2) + 1, &vecMid);
			}
		}

		pVA->Unlock();

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, pMesh->GetTriangleCount(), 0);
	}

	return pGeoAttr;
}

i3Node * i3SceneUtil::BuildRendableNavigationMesh( i3NavigationMesh * pNode, bool bLine, bool bNormal, REAL32 dY )
{
	i3Node * pRoot = i3Node::new_object_ref();

	i3GeometryAttr *pGeoAttr;
	i3Geometry * pGeo;

	pGeo = i3Geometry::new_object_ref();
	pRoot->AddChild( pGeo );

	// Polygon
	{
		pGeoAttr = _ConvertNavigationMesh( pNode, dY );
		pGeo->AppendGeometryAttr( pGeoAttr);
	}

	// Wire Frame
	if( bLine )
	{
		pGeoAttr = _ConvertNavigationMeshWireFrame( pNode, dY );
		pGeo->AppendGeometryAttr( pGeoAttr);
	}

	// Middle Point
	if( bNormal )
	{
		pGeoAttr = _ConvertNavigationMiddlePoint( pNode, dY );
		pGeo->AppendGeometryAttr( pGeoAttr);
	}

	return pRoot;
}
