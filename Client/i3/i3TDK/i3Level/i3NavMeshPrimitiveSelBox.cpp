#include "stdafx.h"
#include "i3NavMeshPrimitiveSelBox.h"

#include "i3LevelGlobalVariable.h"
#include "i3NavMesh.h"
#include "i3NavMeshDataMgr.h"

I3_CLASS_INSTANCE( i3NavMeshPrimitiveSelBox); //, i3NavMeshPrimitive );

void	i3NavMeshPrimitiveSelBox::OnBuildMesh()
{
	_BuildGeometry_SelBox();
}

void	i3NavMeshPrimitiveSelBox::OnRender()
{
	i3NavMesh* pNav				= i3Level::GetNavMesh();
	i3NavMeshDataMgr* dataMgr	= pNav ? pNav->GetDataMgr() : nullptr;

	if( nullptr == dataMgr || nullptr == m_ctx )
	{
		return;
	}

	i3::vector< int > selList;	
	pNav->GetSelectedVertexList( selList );

	int nOver = pNav->GetMouseOverVertex();

	if( -1 != nOver )
	{
		if( false == pNav->IsSelectedVertex( nOver ) )
		{
			selList.push_back( pNav->GetMouseOverVertex() );
		}
	}

	const i3::vector< navMeshVertex >& vertexList = dataMgr->GetVertexList();

	if( vertexList.empty() || selList.empty() )
	{
		return;
	}

	COLORREAL cr;
	i3Color::Set( &cr, 1.0f, 1.0f, 1.0f, 1.0f );

	VEC3D v;
	
	i3::vector< int > drawList;
	drawList.reserve( selList.size() );

	// draw list
	{
		i3LevelViewport * pView			= i3LevelViewport::GetCurrentFocusViewport();
		i3LevelFramework * pFramework	= pView ? pView->getFramework() : nullptr;

		I3ASSERT_RETURN(pFramework);

		VEC3D vCameraPos;
		MATRIX* pCamMat = pFramework->GetCurrentCamera()->GetTransform();
		i3Vector::Copy( &vCameraPos, i3Matrix::GetPos(pCamMat) );

		VEC3D vSub;
		for each( int idx in selList )
		{
			if( pNav->m_InFrustumVertexList.end() == pNav->m_InFrustumVertexList.find( idx ) )
			{
				continue;
			}

			if( nullptr == dataMgr->GetVec3D( idx ) )
			{
				continue;
			}

			i3Vector::Sub( &vSub, &vCameraPos, dataMgr->GetVec3D( idx ) );
			if( 50.0f <= i3Vector::Length( &vSub ) )
			{
				continue;
			}

			drawList.push_back( idx );
		}
	}

	for each( int idx in drawList )
	{
		i3Vector::Copy( &v, (VEC3D*) &vertexList.at( idx ) );

		/*if( false == frustum.TestSphere( &v, pNav->GetVertexScale() ) )
		{
			continue;
		}*/

		const bool bSelected = pNav->IsSelectedVertex( idx );

		if( true == bSelected )
			i3Vector::Add( &v, &v, pNav->GetVertexMovePoint() );


		// 렌더 스테이트
		{
			MATRIX mat;

			mat.m00 *= pNav->GetVertexScale();
			mat.m11 *= pNav->GetVertexScale();
			mat.m22 *= pNav->GetVertexScale();

			mat.m30 = v.x;
			mat.m31 = v.y;
			mat.m32 = v.z;

			m_ctx->SetWorldMatrix( &mat );
			m_ctx->SetColor( &cr );
			m_ctx->SetMaterial( &cr, nullptr, nullptr, 0.0f, I3G_MATERIAL_DIFFUSE );

			//m_ctx->SetZTestEnable( FALSE );
			//m_ctx->SetZWriteEnable( FALSE );
		}

		// render
		RenderElement();
	}
}