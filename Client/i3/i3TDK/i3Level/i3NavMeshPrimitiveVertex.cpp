#include "stdafx.h"
#include "i3NavMeshPrimitiveVertex.h"

#include "i3LevelGlobalVariable.h"
#include "i3NavMesh.h"
#include "i3NavMeshDataMgr.h"


I3_CLASS_INSTANCE( i3NavMeshPrimitiveVertex); //, i3NavMeshPrimitive );

void	i3NavMeshPrimitiveVertex::OnBuildMesh()
{
	_BuildGeometry_Sphere();
}

void	i3NavMeshPrimitiveVertex::OnRender()
{
	i3NavMesh* pNav				= i3Level::GetNavMesh();
	i3NavMeshDataMgr* dataMgr	= pNav ? pNav->GetDataMgr() : nullptr;

	if( nullptr == dataMgr || nullptr == m_ctx )
	{
		return;
	}

	i3LevelViewport * pView			= i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework	= pView ? pView->getFramework() : nullptr;

	i3::vector< int > drawList;
	i3::set< int > selList;

	if( false == _Calculate_DrawList( pNav, dataMgr, pFramework, drawList, selList ) )
	{
		return;
	}

	const LEVEL_EDIT_MODE editMode = pFramework->GetLevelEditMode();

	VEC3D v;

	// render
	for each( int idx in drawList )
	{
		i3Vector::Copy( &v, dataMgr->GetVec3D( idx ) );

		const bool bSelected = (selList.end() != selList.find( idx ));

		if( true == bSelected )
			i3Vector::Add( &v, &v, pNav->GetVertexMovePoint() );

		if( LEM_NAVMESH_POINT == editMode || LEM_NAVMESH_POLYGON_CUT == editMode )
		{
			m_ctx->SetZTestEnable( bSelected ? FALSE : TRUE );
			m_ctx->SetZWriteEnable( bSelected ? FALSE : TRUE );

			_Render_Vertex( v, pNav->GetVertexScale(), I3G_RENDER_POLYGON, bSelected ? m_crSelect : m_crNormal );
		}		
		else
		{
			m_ctx->SetZTestEnable( TRUE );
			m_ctx->SetZWriteEnable( TRUE );

			_Render_Vertex( v, pNav->GetVertexScale(), I3G_RENDER_POLYGON, m_crNormal );
		}
	}

	// render add point
	{
		if( LEM_NAVMESH_POINT_ADD == editMode )
		{
			_Render_Vertex( pNav->GetPickAddVertex(), pNav->GetVertexScale(), I3G_RENDER_POLYGON, m_crSelect);
		}

		if( LEM_NAVMESH_POLYGON_CUT == editMode )
		{
			const navMeshCuttingInfo& info = pNav->GetCuttingInfo();

			if( navMeshCuttingInfo::CUTTING_FAIL != info._type )
			{
				_Render_Vertex( info.cutPosition, pNav->GetVertexScale(), I3G_RENDER_POLYGON, m_crSelect );
			}
				
		}
	}
}

void	i3NavMeshPrimitiveVertex::_Render_Vertex( const VEC3D& pos, float scale, I3G_RENDER_MODE _mode, COLORREAL& cr )
{
	MATRIX mat;

	mat.m00 *= scale;
	mat.m11 *= scale;
	mat.m22 *= scale;

	mat.m30 = pos.x;
	mat.m31 = pos.y;
	mat.m32 = pos.z;

	m_ctx->SetWorldMatrix( &mat );
	m_ctx->SetRenderMode( _mode );

	m_ctx->SetColor( &cr );
	m_ctx->SetMaterial( &cr, nullptr, nullptr, 0.0f, I3G_MATERIAL_DIFFUSE );

	RenderElement();
}

bool	i3NavMeshPrimitiveVertex::_Calculate_DrawList( i3NavMesh* pNav, i3NavMeshDataMgr* dataMgr, i3LevelFramework* pFramework, i3::vector< int >& drawList, i3::set< int >& selList )
{
	if( nullptr == pNav || nullptr == dataMgr || nullptr == pFramework )
	{
		return false;
	}

	const LEVEL_EDIT_MODE editMode = pFramework->GetLevelEditMode();

	if( LEM_NORMAL == editMode )
	{
		return false;
	}

	drawList.reserve( pNav->m_InFrustumVertexList.size() );

	VEC3D vCameraPos;
	VEC3D vSub;

	// draw list
	{		
		MATRIX* pCamMat = pFramework->GetCurrentCamera()->GetTransform();
		i3Vector::Copy( &vCameraPos, i3Matrix::GetPos(pCamMat) );
		
		for each( int idx in pNav->m_InFrustumVertexList )
		{
			if( nullptr == dataMgr->GetVec3D( idx ) )
			{
				continue;
			}

			i3Vector::Sub( &vSub, &vCameraPos, dataMgr->GetVec3D( idx ) );
			if( NAVMESH_PRIMITIVE_FAR <= i3Vector::Length( &vSub ) )
			{
				continue;
			}

			drawList.push_back( idx );
		}
	}

	switch( editMode )
	{
	case	LEM_NAVMESH_POINT:
	case	LEM_NAVMESH_POINT_ADD:
	case	LEM_NAVMESH_POLYGON_CUT:
			{
				i3::vector< int > vertList;
				pNav->GetSelectedVertexList( vertList );

				for each( int idx in vertList )
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
					if( NAVMESH_PRIMITIVE_FAR <= i3Vector::Length( &vSub ) )
					{
						continue;
					}

					selList.insert( idx );
				}
			}
			break;

	case	LEM_NAVMESH_POLYGON:
			{
				i3::vector< int > polyList;
				pNav->GetSelectedPolygonList( polyList );

				for each( int idx in polyList )
				{
					const navMeshPoly* poly = dataMgr->GetPolygon( idx );

					if( nullptr == poly )
					{
						continue;
					}

					for( int i = 0; i < 3; ++i )
					{
						if( pNav->m_InFrustumVertexList.end() == pNav->m_InFrustumVertexList.find( poly->n[i] ) )
						{
							continue;
						}

						if( nullptr == dataMgr->GetVec3D( poly->n[i] ) )
						{
							continue;
						}

						i3Vector::Sub( &vSub, &vCameraPos, dataMgr->GetVec3D( poly->n[i] ) );
						if( NAVMESH_PRIMITIVE_FAR <= i3Vector::Length( &vSub ) )
						{
							continue;
						}


						selList.insert( poly->n[i] );
					}
				}
			}
			break;
	}

	return (false == (drawList.empty() && selList.empty())) || LEM_NAVMESH_POINT_ADD == editMode;
}