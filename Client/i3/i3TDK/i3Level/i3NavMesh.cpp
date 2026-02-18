#include "stdafx.h"
#include "i3NavMesh.h"

#include "i3LevelGlobalVariable.h"
#include "i3LevelScene.h"
#include "i3LevelWorld.h"

#include "i3NavMeshFile.h"
#include "i3NavMeshDataMgr.h"

#include "i3NavMeshPrimitiveVertex.h"
#include "i3NavMeshPrimitiveSelBox.h"
#include "i3NavMeshPrimitivePolyNormal.h"
#include "i3NavMeshPrimitivePolygon.h"
#include "i3NavMeshPrimitivePolyCut.h"

#include "i3LevelViewport.h"
#include "i3LevelFramework.h"

#include <array>

I3_CLASS_INSTANCE( i3NavMesh); //, i3Node );

i3NavMesh::~i3NavMesh()
{
	I3_SAFE_RELEASE( m_pPrimPoly );
	I3_SAFE_RELEASE( m_nmFile );
	I3_SAFE_RELEASE( m_dataMgr );
}

bool	i3NavMesh::Create()
{
	I3_SAFE_RELEASE( m_dataMgr );
	
	m_dataMgr = i3NavMeshDataMgr::new_object();
	m_dataMgr->Init( this );
	
	_CreatePrimitives();
	return true;
}

void	i3NavMesh::Update()
{
	if( UPDATE_FRUSTUM & m_UpdateFlag )
	{
		_UpdateFrustum();
	}

	if( UPDATE_REBUILD_VB & m_UpdateFlag )
	{
		if( m_pPrimPoly )
			m_pPrimPoly->ReBuild_VB();
	}

	m_UpdateFlag = 0;
}

bool	i3NavMesh::DoAutoGenration( i3World* world, const navMeshConfig& cfg )
{
	i3NavMeshDataMgr* prev = m_dataMgr;

	m_dataMgr = i3NavMeshDataMgr::new_object();
	m_dataMgr->Init( this );

	ClearSelectList();

	bool bRes = false;

	bRes = m_dataMgr->DoAutoGeneration( world, cfg );

	if( bRes )
		i3LevelUndo::ADD_NavMesh_GenerateMesh( prev, m_dataMgr );

	AddUpdateFlag( UPDATE_FRUSTUM | UPDATE_REBUILD_VB );
	I3_SAFE_RELEASE( prev );	
	return bRes;
}

bool	i3NavMesh::ClearData()
{
	ClearSelectList();

	if( m_dataMgr )
	{
		if( 0 == m_dataMgr->GetVertexCount() && 0 == m_dataMgr->GetPolygonCount() )
		{
			return false;
		}
	}

	i3NavMeshDataMgr* prev = m_dataMgr;

	m_dataMgr = i3NavMeshDataMgr::new_object();
	m_dataMgr->Init( this );

	i3LevelUndo::ADD_NavMesh_GenerateMesh( prev, m_dataMgr );

	AddUpdateFlag( UPDATE_FRUSTUM | UPDATE_REBUILD_VB );
	I3_SAFE_RELEASE( prev );
	return true;
}

void	i3NavMesh::_CreatePrimitives()
{
	i3AttrSet* pAttrSet = i3AttrSet::new_object_ref();

	{
		i3ZTestEnableAttr* pAttr = i3ZTestEnableAttr::new_object_ref();
		pAttr->Set( TRUE );
		pAttrSet->AddAttr( pAttr );
	}

	{
		i3LightingEnableAttr* pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set( FALSE );
		pAttrSet->AddAttr( pAttr);
	}

	{
		i3FaceCullModeAttr* pAttr = i3FaceCullModeAttr::new_object_ref();

		pAttr->Set( I3G_FACE_CULL_NONE );
		pAttrSet->AddAttr( pAttr);
	}

	{	
		i3NavMeshPrimitive* pPrim = nullptr;
		i3Geometry* ptr = i3Geometry::new_object_ref();

		{
			pPrim = i3NavMeshPrimitiveVertex::new_object_ref();
			pPrim->Init( ptr );
		}

		{
			pPrim = i3NavMeshPrimitivePolyCut::new_object_ref();
			pPrim->Init( ptr );
		}

		{
			pPrim = i3NavMeshPrimitiveSelBox::new_object_ref();
			pPrim->Init( ptr );
		}

		{
			pPrim = i3NavMeshPrimitivePolyNormal::new_object_ref();
			pPrim->Init( ptr );
		}

		{
			m_pPrimPoly = i3NavMeshPrimitivePolygon::new_object();			// 이것 혼자 NewObject...(2012.09.03.수빈)
			m_pPrimPoly->Init( ptr );
		}


		pAttrSet->AddChild( ptr );
	}

	AddChild( pAttrSet );
}

void	i3NavMesh::UpdateMouseMove( const POINT& pt, bool bUpdateVertex, bool bUpdatePoly )
{
	m_MouseOverVertex = -1;
	m_MouseOverPoly = -1;

	if( nullptr == m_dataMgr )
	{
		return;
	}

	if( bUpdateVertex )
		m_dataMgr->GetPickVertex( pt, m_MouseOverVertex );

	if( bUpdatePoly )
		m_dataMgr->GetPickPolygon( pt, m_MouseOverPoly );
}

void	i3NavMesh::UpdateCameraMove()
{
	AddUpdateFlag( UPDATE_FRUSTUM );
}

float	i3NavMesh::GetVertexScale() const
{
	if( m_dataMgr )
		return m_dataMgr->GetVertexScale();


	return 0.2f; 
}

void	i3NavMesh::SetVertexScale( float scale )		
{
	if( m_dataMgr )
		m_dataMgr->SetVertexScale( scale );
}

bool	i3NavMesh::SelectVertex( const POINT& pt, SELECT_NAVMESH_TYPE selMode )
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	switch( selMode )
	{
	case	SEL_NAVMESH_EXCLUSIVE:
			m_SelectedVertexList.clear();
			break;
	}

	int nPickVertex = -1;

	if( false == m_dataMgr->GetPickVertex( pt, nPickVertex ) )
	{
		i3Level::Update( nullptr, I3_TDK_UPDATE_SELECT, nullptr, i3LevelElement3D::static_meta() );
		return false;
	}

	_SelectVertex( nPickVertex, selMode );
	i3Level::Update( nullptr, I3_TDK_UPDATE_SELECT, nullptr, i3LevelElement3D::static_meta() );
	return true;
}

void	i3NavMesh::SetVertexApplyPoint()
{
	if( 0.0f == m_vertexMove.x && 0.0f == m_vertexMove.y && 0.0f == m_vertexMove.z  )
	{
		return;
	}

	i3LevelViewport * pView = i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework = pView->getFramework();

	switch( pFramework->GetLevelEditMode() )
	{
	case	LEM_NAVMESH_POINT:
			MoveVertex( m_SelectedVertexList, m_vertexMove, true );
			break;

	case	LEM_NAVMESH_POLYGON:
			{
				i3::set< int > moveList;
				for each( int idx in m_SelectedPolyList )
				{
					const navMeshPoly* poly = m_dataMgr->GetPolygon( idx );

					if( nullptr == poly )
					{
						continue;
					}

					for( int i = 0; i < 3; ++i )
					{
						moveList.insert( poly->n[i] );
					}
				}

				MoveVertex( moveList, m_vertexMove, true );
			}
			break;
	}
	
	i3Vector::Set( &m_vertexMove, 0.0f, 0.0f, 0.f );
	i3Level::Update( nullptr, I3_TDK_UPDATE_MOVE, nullptr, i3LevelElement3D::static_meta() );
}

bool	i3NavMesh::SelectBandVertex( const POINT& pt1, const POINT& pt2, SELECT_NAVMESH_TYPE selMode )
{
	switch( selMode )
	{
	case	SEL_NAVMESH_EXCLUSIVE:
			m_SelectedVertexList.clear();
			break;
	}

	i3Frustum frustum;

	MATRIX mvp;

	VEC3D tmin, tmax;
	i3::pack::RECT src, dest;

	i3LevelViewport * pView = i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework = pView->getFramework();

	pFramework->GetViewProjectionMatrix( &mvp );

	{
		frustum.Set( &mvp );

		INT32 hw, hh;
		REAL32 rw, rh;
		REAL32 x1, y1, x2, y2;

		// 화면 한가운데를 ( 0, 0)으로 간주한 Clipping-Space로의 좌표 변환용.
		hw = pView->GetViewWidth() >> 1;
		hh = pView->GetViewHeight() >> 1;

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		x1	=	(pt1.x - hw) * rw;
		y1	=	(pt1.y - hh) * rh;
		x2	=	(pt2.x - hw) * rw;
		y2	=	(pt2.y - hh) * rh;

		src.left = MIN( x1, x2);
		src.top = MIN( y1, y2);
		src.right = MAX( x1, x2);
		src.bottom = MAX( y1, y2);
	}

	float vscale = GetVertexScale();

	bool bPick = false;
	int idx = 0;

	for each( navMeshVertex v in m_dataMgr->GetVertexList() )
	{
		if( true == m_dataMgr->IsRemovedVertex( idx ) )
		{
			++idx;
			continue;
		}

		if( false == frustum.TestSphere( &v.vec3D, vscale ) )
		{
			++idx;
			continue;
		}

		i3Vector::Copy( &tmin, &v.vec3D );
		i3Vector::Copy( &tmax, &v.vec3D );

		tmin.x -= (vscale * 0.5f);
		tmin.y -= (vscale * 0.5f);
		tmin.z -= (vscale * 0.5f);

		tmax.x += (vscale * 0.5f);
		tmax.y += (vscale * 0.5f);
		tmax.z += (vscale * 0.5f);

		bool bHit;

		{
			VEC3D bb[8];
			i3BoundBox bbox;
			INT32 j;

			bbox.SetMinMax( &tmin, &tmax);
			bbox.GetCornerVertices( bb );

			i3Matrix::TransformPoints( bb, bb, 8, &mvp );

			i3Vector::Copy( &tmin,  &bb[0]);
			i3Vector::Copy( &tmax,  &bb[0]);
			for( j = 1; j < 8; j++)
			{
				i3Vector::Minimize( &tmin, &tmin, &bb[j]);
				i3Vector::Maximize( &tmax, &tmax, &bb[j]);
			}
		}

		dest.left	= MIN( i3Vector::GetX( &tmin), i3Vector::GetX( &tmax));
		dest.top	= MIN( i3Vector::GetY( &tmin), i3Vector::GetY( &tmax));
		dest.right	= MAX( i3Vector::GetX( &tmin), i3Vector::GetX( &tmax));
		dest.bottom	= MAX( i3Vector::GetY( &tmin), i3Vector::GetY( &tmax));

		if( i3Math::RectInRect( &dest, &src))
			bHit = true;
		else if( i3Math::RectInRect( &src, &dest))
			bHit = true;
		else
			bHit = false;

		if( bHit )
		{
			_SelectVertex( idx, selMode );
			bPick = true;
		}

		++idx;
	}

	i3Level::Update( nullptr, I3_TDK_UPDATE_SELECT, nullptr, i3LevelElement3D::static_meta() );
	return bPick;
}

bool	i3NavMesh::SelectPolygon( const POINT& pt, SELECT_NAVMESH_TYPE selMode )
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	switch( selMode )
	{
	case	SEL_NAVMESH_EXCLUSIVE:
			m_SelectedPolyList.clear();
			break;
	}

	int nPickPoly = -1;
	if( false == m_dataMgr->GetPickPolygon( pt, nPickPoly ) )
	{
		i3Level::Update( nullptr, I3_TDK_UPDATE_SELECT, nullptr, i3LevelElement3D::static_meta() );
		return false;
	}

	_SelectPolygon( nPickPoly, selMode );

	i3Level::Update( nullptr, I3_TDK_UPDATE_SELECT, nullptr, i3LevelElement3D::static_meta() );
	return true;
}

bool	i3NavMesh::SelectBandPolygon( const POINT& ptArg1, const POINT& ptArg2, SELECT_NAVMESH_TYPE selMode )
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	switch( selMode )
	{
	case	SEL_NAVMESH_EXCLUSIVE:
			m_SelectedPolyList.clear();
			break;
	}

	i3::set< int > pickList;

	{
		i3::vector< navMeshPick > tmpList;

		POINT temp_pt[4];

		temp_pt[0].x = ptArg1.x;
		temp_pt[0].y = ptArg1.y;

		temp_pt[1].x = ptArg2.x;
		temp_pt[1].y = ptArg2.y;

		temp_pt[2].x = ptArg1.x;
		temp_pt[2].y = ptArg2.y;

		temp_pt[3].x = ptArg2.x;
		temp_pt[3].y = ptArg1.y;

		for( int i = 0; i < 4; ++i )
		{	
			if( false == m_dataMgr->PickPolygon( temp_pt[i], tmpList ) )
			{
				continue;
			}

			for each( navMeshPick pick in tmpList )
			{
				pickList.insert( pick.idx );
			}
		}
	}

	i3Frustum frustum;
	i3::pack::RECT src;

	MATRIX mvp;

	i3LevelViewport * pView = i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework = pView->getFramework();

	pFramework->GetViewProjectionMatrix( &mvp );

	{
		frustum.Set( &mvp );

		INT32 hw, hh;
		REAL32 rw, rh;
		REAL32 x1, y1, x2, y2;

		// 화면 한가운데를 ( 0, 0)으로 간주한 Clipping-Space로의 좌표 변환용.
		hw = pView->GetViewWidth() >> 1;
		hh = pView->GetViewHeight() >> 1;

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		x1	=	(ptArg1.x - hw) * rw;
		y1	=	(ptArg1.y - hh) * rh;
		x2	=	(ptArg2.x - hw) * rw;
		y2	=	(ptArg2.y - hh) * rh;

		src.left = MIN( x1, x2);
		src.top = MIN( y1, y2);
		src.right = MAX( x1, x2);
		src.bottom = MAX( y1, y2);
	}

	const float vscale = GetVertexScale();

	int idx = 0;
	VEC3D v[3];

	for each( navMeshPoly poly in m_dataMgr->GetPolygonList() )
	{
		if( pickList.end() != pickList.find( idx ) )
		{
			++idx;
			continue;
		}

		if( true == m_dataMgr->IsRemovedPolygon( idx ) )
		{
			++idx;
			continue;
		}

		bool bIn = false;

		for( int i = 0; i < 3; ++i )
		{
			//i3Vector::Copy( &v[i], &m_vertex[ poly.n[i] ].vec3D );
			i3Vector::Copy( &v[i], m_dataMgr->GetVec3D( poly.n[i] ) );

			if( frustum.TestSphere( &v[i], vscale ) )
			{
				bIn = true;
			}
		}

		if( false == bIn )
		{
			++idx;
			continue;
		}

		bool bHit = false;

		i3Matrix::TransformPoints( v, v, 3, &mvp );

		VEC2D pt1, pt2;

		{
			pt1.x = v[0].x;
			pt1.y = v[0].y;
			pt2.x = v[1].x;
			pt2.y = v[1].y;

			if( true == I3NAVMESH::IntersectLineToRect( &pt1, &pt2, &src ) )
			{
				bHit = true;
			}

			if( false == bHit )
			{
				pt1.x = v[0].x;
				pt1.y = v[0].y;
				pt2.x = v[2].x;
				pt2.y = v[2].y;

				if( true == I3NAVMESH::IntersectLineToRect( &pt1, &pt2, &src ) )
				{
					bHit = true;
				}
			}

			if( false == bHit )
			{
				pt1.x = v[1].x;
				pt1.y = v[1].y;
				pt2.x = v[2].x;
				pt2.y = v[2].y;

				if( true == I3NAVMESH::IntersectLineToRect( &pt1, &pt2, &src ) )
				{
					bHit = true;
				}
			}
		}

		if( false == bHit )
		{
			for( int i = 0; i < 3; ++i )
			{
				pt1.x = v[i].x;
				pt1.y = v[i].y;

				if( true == i3Math::PtInRect( &src, &pt1 ) )
				{
					bHit = true;
					break;
				}
			}
		}

		if( true == bHit )
			pickList.insert( idx );

		++idx;
	}

	for each( int nPickPoly in pickList )
	{
		switch( selMode )
		{
		case	SEL_NAVMESH_EXCLUSIVE:
		case	SEL_NAVMESH_INCLUSIVE:
				m_SelectedPolyList.insert( nPickPoly );
				break;

		case	SEL_NAVMESH_ALTERNATIVE:
				{
					i3::set< int >::iterator itr = m_SelectedPolyList.find( nPickPoly );

					if( itr != m_SelectedPolyList.end() )
						m_SelectedPolyList.erase( itr );
					else
						m_SelectedPolyList.insert( nPickPoly );
				}
				break;

		case	SEL_NAVMESH_UNSELECT:
				m_SelectedPolyList.erase( nPickPoly );
				break;
		}
	}

	i3Level::Update( nullptr, I3_TDK_UPDATE_SELECT, nullptr, i3LevelElement3D::static_meta() );
	return true;
}

void	i3NavMesh::_SelectVertex( int nPickVertex, SELECT_NAVMESH_TYPE selMode )
{
	switch( selMode )
	{
	case	SEL_NAVMESH_EXCLUSIVE:
	case	SEL_NAVMESH_INCLUSIVE:
			m_SelectedVertexList.insert( nPickVertex );
			break;

	case	SEL_NAVMESH_ALTERNATIVE:
			{
				i3::set< int >::iterator itr = m_SelectedVertexList.find( nPickVertex );

				if( itr != m_SelectedVertexList.end() )
					m_SelectedVertexList.erase( itr );
				else
					m_SelectedVertexList.insert( nPickVertex );
			}
			break;

	case	SEL_NAVMESH_UNSELECT:
			m_SelectedVertexList.erase( nPickVertex );
			break;
	}
}

bool	i3NavMesh::IsSelectedVertex( int idx ) const
{
	return m_SelectedVertexList.end() != m_SelectedVertexList.find( idx );
}

bool	i3NavMesh::IsSelectedPolygon( int idx ) const
{
	return m_SelectedPolyList.end() != m_SelectedPolyList.find( idx );
}

void	i3NavMesh::GetSelectedVertexList( i3::vector< int >& selList ) const
{
	selList.clear();
	selList.reserve( m_SelectedVertexList.size() );

	for each( int idx in m_SelectedVertexList )
	{
		selList.push_back( idx );
	}
}

void	i3NavMesh::GetSelectedPolygonList( i3::vector< int >& selList ) const
{
	selList.clear();
	selList.reserve( m_SelectedVertexList.size() );

	for each( int idx in m_SelectedPolyList )
	{
		selList.push_back( idx );
	}
}

bool	i3NavMesh::MoveVertex( int idx, const VEC3D& vec, bool bUseUndo )
{
	if (m_dataMgr)
	{
		m_dataMgr->MoveVertex(idx, vec);

		if (m_pPrimPoly)
			m_pPrimPoly->SetVB(idx, *m_dataMgr->GetVec3D(idx));
	}

	if( bUseUndo )
		i3LevelUndo::ADD_NavMesh_MoveVertex( idx, vec );
	
	return true;
}

bool	i3NavMesh::MoveVertex( i3::set< int > idxList, const VEC3D& vec, bool bUseUndo )
{
	for each( int idx in idxList )
	{
		if (m_dataMgr)
		{
			m_dataMgr->MoveVertex(idx, vec);

			if (m_pPrimPoly)
				m_pPrimPoly->SetVB(idx, *m_dataMgr->GetVec3D(idx));
		}
	}

	if( bUseUndo )
		i3LevelUndo::ADD_NavMesh_MoveVertex( idxList, vec );

	return true;
}

bool	i3NavMesh::MoveSelectedVertex( const VEC3D& vMove )
{
	if( 0.0f == vMove.x && 0.0f == vMove.y && 0.0f == vMove.z  )
	{
		return false;
	}

	i3LevelViewport * pView = i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework = pView->getFramework();

	switch( pFramework->GetLevelEditMode() )
	{
	case	LEM_NAVMESH_POINT:
			MoveVertex( m_SelectedVertexList, vMove, true );
			break;

	case	LEM_NAVMESH_POLYGON:
			{
				i3::set< int > moveList;
				for each( int idx in m_SelectedPolyList )
				{
					const navMeshPoly* poly = m_dataMgr->GetPolygon( idx );

					if( nullptr == poly )
					{
						continue;
					}

					for( int i = 0; i < 3; ++i )
					{
						moveList.insert( poly->n[i] );
					}
				}

				MoveVertex( moveList, vMove, true );
			}
			break;
	}

	i3Level::Update( nullptr, I3_TDK_UPDATE_MOVE, nullptr, i3LevelElement3D::static_meta() );
	return true;
}

bool	i3NavMesh::AddVertex( const POINT& pt )
{
	VEC3D pos;

	if( false == GetPickPosition_byWorld( pt, pos ) )
	{
		return false;
	}

	return AddVertex( pos, true );
}

bool	i3NavMesh::AddVertex( const VEC3D& pos, bool bUseUndo )
{
	if( false == m_dataMgr->AddVertex( pos ) )
	{
		return false;
	}

	if( true == bUseUndo )
	{
		i3LevelUndo::ADD_NavMesh_AddVertex( GetVertexCount() -1 );
	}

	AddUpdateFlag( UPDATE_FRUSTUM | UPDATE_REBUILD_VB );

	return true;
}

bool	i3NavMesh::RemoveVertex( int idx, bool bUseUndo )
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	if( false == m_dataMgr->RemoveVertex( idx ) )
	{
		return false;
	}

	if( true == bUseUndo )
		i3LevelUndo::ADD_NavMesh_RemoveVertex( idx );

	AddUpdateFlag( UPDATE_FRUSTUM );
	return true;
}

bool	i3NavMesh::RemoveVertex( const i3::vector< int >& eraseList, bool bUseUndo )
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	if( false == m_dataMgr->RemoveVertexList( eraseList ) )
	{
		return false;
	}

	if( true == bUseUndo )
		i3LevelUndo::ADD_NavMesh_RemoveVertex( eraseList );

	AddUpdateFlag( UPDATE_FRUSTUM );
	return true;
}

bool	i3NavMesh::RemoveSelectedVertex()
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	if( m_SelectedVertexList.empty() )
	{
		return true;
	}

	bool bRes = false;


	i3::vector< int > selList;
	for each( int n in m_SelectedVertexList )
	{
		selList.push_back( n );
	}

	bRes = RemoveVertex( selList, true );

	if( bRes )
		m_SelectedVertexList.clear();

	return bRes;
}


bool	i3NavMesh::CreatePolygon()
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	if( 3 != static_cast<int>(m_SelectedVertexList.size()) )
	{
		return false;
	}

	navMeshPoly poly;

	int idx = 0;
	for each( int n in m_SelectedVertexList )
	{
		poly.n[ idx++ ] = n;
	}

	return	CreatePolygon( poly );
}

bool	i3NavMesh::CreatePolygon( const navMeshPoly& poly, bool bUndo, int* pPolyIdx )
{
	int nRemovedPoly;

	if( true == m_dataMgr->Find_RemovedPolygonIdx( poly, nRemovedPoly ) )
	{
		m_dataMgr->ReUsePolygon( nRemovedPoly );

		if( pPolyIdx )
			*pPolyIdx = nRemovedPoly;

		if( bUndo )
			i3LevelUndo::ADD_NavMesh_CreatePolygon( nRemovedPoly );

		AddUpdateFlag( UPDATE_FRUSTUM );
		return true;
	}

	if( true == m_dataMgr->CreatePolygon( poly ) )
	{
		if( pPolyIdx )
			*pPolyIdx = (m_dataMgr->GetPolygonCount() -1) ;

		if( bUndo )
			i3LevelUndo::ADD_NavMesh_CreatePolygon( m_dataMgr->GetPolygonCount() -1 );

		AddUpdateFlag( UPDATE_FRUSTUM );
		return true;
	}

	return false;
}

bool	i3NavMesh::RemoveSelectedPolygon()
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	if( true == m_SelectedPolyList.empty() )
	{
		return false;
	}

	for each( int idx in m_SelectedPolyList )
	{
		m_dataMgr->RemovePolygon( idx );
	}

	i3LevelUndo::ADD_NavMesh_RemovePolygon( m_SelectedPolyList );	
	m_SelectedPolyList.clear();
	
	AddUpdateFlag( UPDATE_FRUSTUM );
	return true;
}

void	i3NavMesh::ClearSelectList()
{
	m_cutInfo.reset();

	m_MouseOverVertex = -1;
	m_MouseOverPoly = -1;

	m_SelectedVertexList.clear();
	m_SelectedPolyList.clear();

	AddUpdateFlag( UPDATE_FRUSTUM );
}

bool	i3NavMesh::GetPickPosition_byWorld( const POINT &pt, VEC3D& pos )
{
	i3LevelViewport* pView = i3LevelViewport::GetCurrentFocusViewport();

	if( nullptr == pView )
	{
		return false;
	}

	i3LevelFramework* pFramework = pView->getFramework();

	if( pFramework )
		return pFramework->GetPickedPos( pt, &pos );

	return false;
}

int		i3NavMesh::GetVertexCount() const
{
	if( m_dataMgr )
		return m_dataMgr->GetVertexCount();

	return 0;
}

int		i3NavMesh::GetPolygonCount() const
{
	if( m_dataMgr )
		return m_dataMgr->GetPolygonCount();

	return 0;
}

int		i3NavMesh::GetUsingVertexCount() const
{
	if( m_dataMgr )
		return m_dataMgr->GetUsingVertexCount();

	return 0;
}

int		i3NavMesh::GetUsingPolygonCount() const
{
	if( m_dataMgr )
		return m_dataMgr->GetUsingPolygonCount();

	return 0;
}

bool	i3NavMesh::SaveNM( const i3::string& _path )
{
	if( nullptr == m_nmFile )
		m_nmFile = i3NavMeshFile::new_object();

	return m_nmFile->SaveNM( _path, m_dataMgr );
}

bool	i3NavMesh::LoadASE( const i3::string& _path )
{
	if( nullptr == m_nmFile )
		m_nmFile = i3NavMeshFile::new_object();

	ClearSelectList();

	bool bRes = false;
	bRes = m_nmFile->LoadASE( _path, m_dataMgr );

	if( bRes )
		i3LevelUndo::ResetSystem();

	AddUpdateFlag( UPDATE_FRUSTUM | UPDATE_REBUILD_VB );
	return bRes;
}

bool	i3NavMesh::SaveASE( const i3::string& _path )
{
	if( nullptr == m_nmFile )
		m_nmFile = i3NavMeshFile::new_object();

	return m_nmFile->SaveASE( _path, m_dataMgr );
}

bool	i3NavMesh::LoadNM( const i3::string& _path )
{
	if( nullptr == m_nmFile )
		m_nmFile = i3NavMeshFile::new_object();

	ClearSelectList();

	bool bRes = false;
	bRes = m_nmFile->LoadNM( _path, m_dataMgr );

	if( bRes )
		i3LevelUndo::ResetSystem();

	AddUpdateFlag( UPDATE_FRUSTUM | UPDATE_REBUILD_VB );
	return bRes;
}

bool	i3NavMesh::RemovePolygon( int idx, bool bUseUndo )
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	m_dataMgr->RemovePolygon( idx );

	if( bUseUndo )
		i3LevelUndo::ADD_NavMesh_RemovePolygon( idx );

	AddUpdateFlag( UPDATE_FRUSTUM );
	return true;
}

bool	i3NavMesh::RemovePolygon( const i3::vector< int >& eraseList, bool bUseUndo )
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	for each( int idx in eraseList )
	{
		m_dataMgr->RemovePolygon( idx );
	}

	if( bUseUndo )
	{	
		i3::set< int > undoList;
		for each( int idx in eraseList )
		{
			undoList.insert( idx );
		}

		i3LevelUndo::ADD_NavMesh_RemovePolygon( undoList );
	}

	AddUpdateFlag( UPDATE_FRUSTUM );
	return true;
}


void	i3NavMesh::SetDataMgr( i3NavMeshDataMgr* ptr )
{
	I3_REF_CHANGE( m_dataMgr, ptr );

	if( m_dataMgr )
		m_dataMgr->Init( this );

	AddUpdateFlag( UPDATE_FRUSTUM | UPDATE_REBUILD_VB );
}

bool	i3NavMesh::ClearNoUseVertex()
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	i3::set< int > usingVertex;

	int idx = 0;
	for each( const navMeshPoly& poly in m_dataMgr->GetPolygonList() )
	{
		if( m_dataMgr->IsRemovedPolygon( idx ) )
		{
			++idx;
			continue;
		}

		for( int i = 0; i < 3; ++i )
		{
			usingVertex.insert( poly.n[i] );
		}

		++idx;
	}

	i3::vector< int > eraseList;
	eraseList.reserve( usingVertex.size() );

	for( int i = 0; i < GetVertexCount(); ++i )
	{
		if( true == m_dataMgr->IsRemovedVertex( i ) )
		{
			continue;
		}

		if( usingVertex.end() == usingVertex.find( i ) )
		{
			eraseList.push_back( i );
		}
	}

	if( false == m_dataMgr->RemoveVertexList( eraseList, false ) )
	{
		return false;
	}

	i3LevelUndo::ADD_NavMesh_RemoveVertex( eraseList );
	AddUpdateFlag( UPDATE_FRUSTUM );
	return true;
}

void	i3NavMesh::SetVertexMovePosition( VEC3D* point )
{
	i3Vector::Copy( &m_vertexMove, point );

	if( nullptr == m_dataMgr )
	{
		return;
	}

	VEC3D v;
	for each( int idx in m_SelectedVertexList )
	{
		i3Vector::Copy( &v, m_dataMgr->GetVec3D( idx ) );
		i3Vector::Add( &v, &v, &m_vertexMove );

		if( m_pPrimPoly )
			m_pPrimPoly->SetVB( idx, v );
	}

	i3Level::Update( nullptr, I3_TDK_UPDATE_MOVE, nullptr, i3LevelElement3D::static_meta() );
}

void	i3NavMesh::SetPolygonMovePosition( VEC3D* point )
{
	i3Vector::Copy( &m_vertexMove, point );

	if( nullptr == m_dataMgr )
	{
		return;
	}

	VEC3D v;

	for each( int idx in m_SelectedPolyList )
	{
		const navMeshPoly* poly = m_dataMgr->GetPolygon( idx );

		if( nullptr == poly )
		{
			continue;
		}

		for( int i = 0; i < 3; ++i )
		{
			i3Vector::Copy( &v, m_dataMgr->GetVec3D( poly->n[i] ) );
			i3Vector::Add( &v, &v, &m_vertexMove );

			if( m_pPrimPoly )
				m_pPrimPoly->SetVB( poly->n[i], v );
		}
	}

	i3Level::Update( nullptr, I3_TDK_UPDATE_MOVE, nullptr, i3LevelElement3D::static_meta() );
}

void	i3NavMesh::_UpdateFrustum()
{
	i3LevelViewport * pView			= i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework	= pView ? pView->getFramework() : nullptr;

	if( nullptr == pFramework || nullptr == m_dataMgr )
	{
		return;
	}

	MATRIX mvp;
	pFramework->GetViewProjectionMatrix( &mvp );

	i3Frustum frustum;
	frustum.Set( &mvp );

	// vertex list
	{
		m_InFrustumVertexList.clear();

		int idx = 0;
		for each( const navMeshVertex& vert in m_dataMgr->GetVertexList() )
		{
			if( true == m_dataMgr->IsRemovedVertex( idx ) )
			{
				++idx;
				continue;
			}

			if( true == frustum.TestSphere( (VEC3D*) &vert.vec3D, GetVertexScale() ) )
			{
				m_InFrustumVertexList.insert( idx );
			}

			++idx;
		}
	}

	// polygon list
	{
		m_InFrustumPolygonList.clear();

		//VEC3D vmin, vmax;
		i3BoundBox bb;
		int idx = 0;

		for each( const navMeshPoly& poly in m_dataMgr->GetPolygonList() )
		{
			if( true == m_dataMgr->IsRemovedPolygon( idx ) )
			{
				++idx;
				continue;
			}			

			bb.SetMinMax( (VEC3D*) &poly.vmin, (VEC3D*) &poly.vmax );
			if( 0 != frustum.TestBox( &bb ) )
			{
				m_InFrustumPolygonList.insert( idx );
			}

			++idx;
		}
	}
}


void	i3NavMesh::ReUseVertex( int idx )
{
	if( m_dataMgr )
		m_dataMgr->ReUseVertex( idx );

	AddUpdateFlag( UPDATE_FRUSTUM );
}

void	i3NavMesh::ReUsePolygon( int idx )
{
	if( m_dataMgr )
		m_dataMgr->ReUsePolygon( idx );

	AddUpdateFlag( UPDATE_FRUSTUM );
}

bool	i3NavMesh::InvertSelectedPolyNormal()
{
	for each( int idx in m_SelectedPolyList )
	{
		if( m_dataMgr )
			m_dataMgr->InvertPolygonNormal( idx );
	}

	i3LevelUndo::ADD_NavMesh_InvertPolyNormal( m_SelectedPolyList );
	return true;
}

bool	i3NavMesh::GetSelectedCenterPosition( bool bVertex, VEC3D& vCenter ) const
{
	i3Vector::Set( &vCenter, 0,0,0 );

	if( nullptr == m_dataMgr )
	{
		return false;
	}

	int nSize = 0;

	if( bVertex )
	{
		if( true == m_SelectedVertexList.empty() )
		{
			return false;
		}

		for each( int idx in m_SelectedVertexList )
		{
			i3Vector::Add( &vCenter, &vCenter, m_dataMgr->GetVec3D( idx ) );
			i3Vector::Add( &vCenter, &vCenter, &m_vertexMove );
		}			

		nSize = static_cast< int >(m_SelectedVertexList.size());
	}
	else
	{
		if( true == m_SelectedPolyList.empty() )
		{
			return false;
		}

		for each( int idx in m_SelectedPolyList )
		{
			const navMeshPoly* poly = m_dataMgr->GetPolygon( idx );

			for( int i = 0; i < 3; ++i )
			{
				i3Vector::Add( &vCenter, &vCenter, m_dataMgr->GetVec3D( poly->n[i] ) );
				i3Vector::Add( &vCenter, &vCenter, &m_vertexMove );
			}
		}	

		nSize = (m_SelectedPolyList.size() * 3);
	}

	vCenter.x /= nSize;
	vCenter.y /= nSize;
	vCenter.z /= nSize;

	return true;
}

bool	i3NavMesh::GetSelectedMinMax( VEC3D& vMin, VEC3D& vMax ) const
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	i3LevelViewport * pView = i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework = pView ? pView->getFramework() : nullptr;

	bool bVertex = true;

	if( pFramework )
		bVertex = LEM_NAVMESH_POINT ==  pFramework->GetLevelEditMode() ;

	if( true == bVertex )
	{
		if( true == m_SelectedVertexList.empty() )
		{
			return false;
		}

		int i = 0;
		for each( int idx in m_SelectedVertexList )
		{
			if( i == 0 )
			{
				i3Vector::Copy( &vMin, m_dataMgr->GetVec3D( idx ) );
				i3Vector::Copy( &vMax, m_dataMgr->GetVec3D( idx ) );
			}
			else
			{
				i3Vector::Minimize( &vMin, &vMin, m_dataMgr->GetVec3D( idx ) );
				i3Vector::Maximize( &vMax, &vMax, m_dataMgr->GetVec3D( idx ) );
			}
			++i;
		}
	}
	else
	{
		if( true == m_SelectedPolyList.empty() )
		{
			return false;
		}

		int i =0;
		for each( int idx in m_SelectedPolyList )
		{
			const navMeshPoly* poly = m_dataMgr->GetPolygon( idx );

			for( int k = 0; k < 3; ++k )
			{
				if( i == 0 )
				{
					i3Vector::Copy( &vMin, m_dataMgr->GetVec3D( poly->n[k] ) );
					i3Vector::Copy( &vMax, m_dataMgr->GetVec3D( poly->n[k] ) );
				}
				else
				{
					i3Vector::Minimize( &vMin, &vMin, m_dataMgr->GetVec3D( poly->n[k] ) );
					i3Vector::Maximize( &vMax, &vMax, m_dataMgr->GetVec3D( poly->n[k] ) );
				}
				
				++i;
			}
		}	
	}

	return true;
}

void	i3NavMesh::UpdateMouseMove_AddVertex( const POINT& pt )
{
	GetPickPosition_byWorld( pt, m_PickAddVertex );
}

bool	i3NavMesh::UpdateCuttingPoly( const POINT& pt )
{
	m_SelectedVertexList.clear();
	m_SelectedPolyList.clear();

	m_cutInfo.reset();

	if( nullptr == m_dataMgr )
	{
		return false;
	}

	if( false == m_dataMgr->CutPolyLine( pt, m_cutInfo ) )
	{
		return false;
	}

	switch( m_cutInfo._type )
	{
	case	navMeshCuttingInfo::LINE:
			{
				_SelectVertex( m_cutInfo.cutVertex, SEL_NAVMESH_EXCLUSIVE );
				_SelectPolygon( m_cutInfo.selPoly, SEL_NAVMESH_EXCLUSIVE );


				if( m_cutInfo.otherCutVertex != -1 )
				{
					_SelectVertex( m_cutInfo.otherCutVertex, SEL_NAVMESH_EXCLUSIVE );
					_SelectPolygon( m_cutInfo.otherPoly, SEL_NAVMESH_EXCLUSIVE );
				}
			}
			break;

	case	navMeshCuttingInfo::POLY:
			{
				const navMeshPoly* poly = m_dataMgr->GetPolygon( m_cutInfo.selPoly );

				if( nullptr != poly )
				{
					for( int i = 0; i < 3; ++i )
						_SelectVertex( poly->n[i], SEL_NAVMESH_EXCLUSIVE );
				}

				_SelectPolygon( m_cutInfo.selPoly, SEL_NAVMESH_EXCLUSIVE );
			}
			break;
	}

	return true;
}

bool	i3NavMesh::CuttingPoly()
{
	if( nullptr == m_dataMgr )
	{
		return false;
	}

	i3::vector< int > removePoly;
	i3::vector< int > createPoly;

	int addVertex = GetVertexCount();

	switch( m_cutInfo._type )
	{
	case	navMeshCuttingInfo::LINE:
			{
				RemovePolygon( m_cutInfo.selPoly, false );	
				removePoly.push_back( m_cutInfo.selPoly );

				if( -1 != m_cutInfo.otherCutVertex )
				{
					RemovePolygon( m_cutInfo.otherPoly, false );
					removePoly.push_back( m_cutInfo.otherPoly );
				}

				AddVertex( m_cutInfo.cutPosition, false );

				int nPolyIdx = -1;

				navMeshPoly poly;

				poly.n[0] = addVertex;
				poly.n[1] = m_cutInfo.cutLine[0];
				poly.n[2] = m_cutInfo.cutVertex;

				CreatePolygon( poly, false, &nPolyIdx );				
				createPoly.push_back( nPolyIdx );

				poly.n[1] = m_cutInfo.cutLine[1];
				poly.n[2] = m_cutInfo.cutVertex;

				CreatePolygon( poly, false, &nPolyIdx );
				createPoly.push_back( nPolyIdx );

				if( -1 != m_cutInfo.otherCutVertex )
				{
					poly.n[1] = m_cutInfo.cutLine[0];
					poly.n[2] = m_cutInfo.otherCutVertex;

					CreatePolygon( poly, false, &nPolyIdx );
					createPoly.push_back( nPolyIdx );

					poly.n[1] = m_cutInfo.cutLine[1];
					poly.n[2] = m_cutInfo.otherCutVertex;

					CreatePolygon( poly, false, &nPolyIdx );
					createPoly.push_back( nPolyIdx );
				}

				i3LevelUndo::ADD_NavMesh_PolyCut( addVertex, removePoly, createPoly );
			}
			break;

	case	navMeshCuttingInfo::POLY:
			{
				RemovePolygon( m_cutInfo.selPoly, false );
				removePoly.push_back( m_cutInfo.selPoly );

				AddVertex( m_cutInfo.cutPosition, false );

				const navMeshPoly* pPoly = m_dataMgr->GetPolygon( m_cutInfo.selPoly );


				int nPolyIdx = -1;
				navMeshPoly poly;

				poly.n[0] = addVertex;

				for( int i = 0; i < 3; ++i )
				{
					switch( i )
					{
					case	0:
							poly.n[1] = pPoly->n[1];
							poly.n[2] = pPoly->n[2];
							break;

					case	1:
							poly.n[1] = pPoly->n[0];
							poly.n[2] = pPoly->n[2];
							break;

					case	2:
							poly.n[1] = pPoly->n[0];
							poly.n[2] = pPoly->n[1];
							break;
					}

					CreatePolygon( poly, false, &nPolyIdx );
					createPoly.push_back( nPolyIdx );
				}

				i3LevelUndo::ADD_NavMesh_PolyCut( addVertex, removePoly, createPoly );
			}
			break;

	default:
		return false;
		break;
	}

	return true;
}

void	i3NavMesh::_SelectPolygon( int nPickPoly, SELECT_NAVMESH_TYPE selMode )
{
	switch( selMode )
	{
	case	SEL_NAVMESH_EXCLUSIVE:
	case	SEL_NAVMESH_INCLUSIVE:
			m_SelectedPolyList.insert( nPickPoly );
			break;

	case	SEL_NAVMESH_ALTERNATIVE:
			{
				i3::set< int >::iterator itr = m_SelectedPolyList.find( nPickPoly );

				if( itr != m_SelectedPolyList.end() )
					m_SelectedPolyList.erase( itr );
				else
					m_SelectedPolyList.insert( nPickPoly );
			}
			break;

	case	SEL_NAVMESH_UNSELECT:
			m_SelectedPolyList.erase( nPickPoly );
			break;
	}
}
