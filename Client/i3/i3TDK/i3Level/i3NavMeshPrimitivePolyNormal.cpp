#include "stdafx.h"
#include "i3NavMeshPrimitivePolyNormal.h"

#include "i3LevelGlobalVariable.h"
#include "i3NavMeshUtil.h"
#include "i3NavMesh.h"
#include "i3NavMeshDataMgr.h"

I3_CLASS_INSTANCE( i3NavMeshPrimitivePolyNormal);//, i3NavMeshPrimitive );

void	i3NavMeshPrimitivePolyNormal::OnBuildMesh()
{
	i3VertexFormat format;
	format.SetHasPosition( true);
	format.SetHasNormal( false);

	// ¸öÅë ¼±
	{
		VEC3D vec;
		i3VertexArray * pVA;

		pVA = i3VertexArray::new_object_ref();

		pVA->Create( &format, 2, 0 );
		pVA->setRestoreType( I3G_RESTORE_MEM);

		pVA->Lock();

		i3Vector::Set( &vec, 0.0f, 0.0f, 0.0f );
		pVA->SetPosition( 0, &vec);

		i3Vector::Set( &vec, 0.0f, 1.0f, 0.0f );
		pVA->SetPosition( 1, &vec);

		pVA->Unlock();

		SetVertexArray( pVA, I3G_PRIM_LINELIST, 1, 0);
	}
}

void	i3NavMeshPrimitivePolyNormal::OnRender()
{
	i3NavMesh* pNav				= i3Level::GetNavMesh();
	i3NavMeshDataMgr* dataMgr	= pNav ? pNav->GetDataMgr() : nullptr;

	if( nullptr == dataMgr || nullptr == m_ctx )
	{
		return;
	}	

	if( false == pNav->IsVisibleNormal() )
	{
		return;
	}

	i3::vector< int > drawList;
	drawList.reserve( pNav->m_InFrustumPolygonList.size() );

	// calculate draw list
	{
		i3LevelViewport * pView			= i3LevelViewport::GetCurrentFocusViewport();
		i3LevelFramework * pFramework	= pView ? pView->getFramework() : nullptr;
		I3ASSERT_RETURN(pFramework);

		if( LEM_NORMAL == pFramework->GetLevelEditMode() )
		{
			return;
		}

		VEC3D vCameraPos;
		MATRIX* pCamMat = pFramework->GetCurrentCamera()->GetTransform();
		i3Vector::Copy( &vCameraPos, i3Matrix::GetPos(pCamMat) );

		VEC3D vSub;
		for each( int idx in pNav->m_InFrustumPolygonList )
		{
			const navMeshPoly* poly = dataMgr->GetPolygon( idx );

			if( nullptr == poly )
			{
				continue;
			}

			i3Vector::Sub( &vSub, &vCameraPos, &poly->center );
			if( NAVMESH_PRIMITIVE_FAR <= i3Vector::Length( &vSub ) )
			{
				continue;
			}

			drawList.push_back( idx );
		}
	}


	COLORREAL cr;
	i3Color::Set( &cr, 1.0f, 1.0f, 1.0f, 1.0f );

	i3BoundBox bb;

	for each( int idx in drawList )
	{
		if( dataMgr->IsRemovedPolygon( idx ) )
		{
			continue;
		}

		const navMeshPoly* poly = dataMgr->GetPolygon( idx );

		if( nullptr == poly )
		{
			continue;
		}	

		MATRIX mat;

		i3Matrix::PostRotateAxis( &mat, &poly->axis, poly->angle );

		mat.m30 = poly->center.x;
		mat.m31 = poly->center.y;
		mat.m32 = poly->center.z;

		m_ctx->SetWorldMatrix( &mat );
		RenderElement();

		++idx;
	}
}