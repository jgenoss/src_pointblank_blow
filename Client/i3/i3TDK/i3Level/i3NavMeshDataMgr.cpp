#include "stdafx.h"
#include "i3NavMeshDataMgr.h"

#include "i3NavMesh.h"
#include "i3NavMeshGenerator.h"

#include "i3LevelViewport.h"
#include "i3LevelFramework.h"

#include <array>

I3_CLASS_INSTANCE( i3NavMeshDataMgr);//, i3ElementBase );

i3NavMeshDataMgr::i3NavMeshDataMgr()
{
	m_poly.reserve( 4096 );
	m_vertex.reserve( 4096 );
}

i3NavMeshDataMgr::~i3NavMeshDataMgr()
{
	I3_SAFE_RELEASE( m_pGenerator );
}

bool	i3NavMeshDataMgr::UsingVertexInPolygon( int nVertex ) const
{
	if( true == IsRemovedVertex( nVertex ) )
	{
		return false;
	}

	return m_UsingVertex.end() != m_UsingVertex.find( nVertex );

	/*int idx = 0;
	for each( const navMeshPoly& p in m_poly )
	{
		if( true == IsRemovedPolygon( idx ) )
		{
			++idx;
			continue;
		}

		for( int i = 0; i < 3; ++i )
		{
			if( p.n[i] == nVertex )
			{
				return true;
			}
		}

		++idx;
	}*/

	//return false;
}

const VEC3D*	i3NavMeshDataMgr::GetVec3D( int idx ) const
{
	if( idx < 0 || static_cast<int>(m_vertex.size()) <= idx )
	{
		return nullptr;
	}

	return &(m_vertex[ idx ].vec3D);
}

VEC3D*		i3NavMeshDataMgr::GetVec3D( int idx )
{
	if( idx < 0 || static_cast<int>(m_vertex.size()) <= idx )
	{
		return nullptr;
	}

	return &(m_vertex[ idx ].vec3D);
}

void	i3NavMeshDataMgr::GetUsingPoly( int nVertex, i3::vector< int >& usingList ) const
{
	bool bUsing;
	int idx = 0;

	for each( const navMeshPoly& p in m_poly )
	{
		bUsing = false;

		if( true == IsRemovedPolygon( idx ) )
		{
			++idx;
			continue;
		}

		for( int i = 0; i < 3; ++i )
		{
			if( p.n[i] == nVertex )
			{
				bUsing = true;
				break;
			}
		}

		if( bUsing )
			usingList.push_back( idx );

		++idx;
	}
}

bool	i3NavMeshDataMgr::AddVertex( const VEC3D& vec )	
{
	m_VertexKeyList.insert( i3::make_pair( navMeshVertex( vec ), static_cast<int>( m_VertexKeyList.size() ) ) );
	m_vertex.push_back( vec );
	return true;
}


bool	i3NavMeshDataMgr::RemoveVertex( int idx, bool bCheckUnUse )
{
	if( idx < 0 || static_cast<int>(m_vertex.size()) <= idx )
	{
		return false;
	}

	if( true == bCheckUnUse )
	{
		if( true == UsingVertexInPolygon( idx ) )
		{
			return false;
		}
	}

	_SetUseVertex( idx, false );
	//m_RemoveVertex.insert( idx );
	return true;
}

bool	i3NavMeshDataMgr::RemoveVertexList( const i3::vector< int >& vecList, bool bCheckUnUse )
{
	if( true == vecList.empty() )
	{
		return false;
	}

	if( true == bCheckUnUse )
	{
		for each( int idx in vecList )
		{
			if( UsingVertexInPolygon( idx ) )
			{
				return false;
			}
		}
	}	

	for each( int idx in vecList )
	{
		//m_RemoveVertex.insert( idx );
		_SetUseVertex( idx, false );
	}

	return true;
}

void	i3NavMeshDataMgr::_CalculateCollideeLine( i3CollideeLine& line, const POINT& pt )
{
	i3LevelFramework * pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	if( nullptr == pFramework )
	{
		return;
	}

	VEC3D pos1, pos2;

	MATRIX tmx;
	REAL32 dx, dy;	
	INT32 w, h;

	w = pFramework->getViewport()->GetViewWidth();
	h = pFramework->getViewport()->GetViewHeight();

	pFramework->GetViewProjectionMatrix( &tmx );
	i3Matrix::Inverse( &tmx, nullptr, &tmx);

	dx = ((REAL32) pt.x / w * 2.0f) - 1.0f;
	dy = ((REAL32) pt.y / h * 2.0f) - 1.0f;

	i3Vector::Set( &pos1, dx, -dy, 0.0f);
	i3Vector::Set( &pos2, dx, -dy, 1.0f);

	i3Vector::TransformCoord( &pos1, &pos1, &tmx);
	i3Vector::TransformCoord( &pos2, &pos2, &tmx);

	line.SetStart( &pos1);
	line.SetEnd( &pos2);
}

void	i3NavMeshDataMgr::_UpdatePolyNormal( int idx )
{
	if( idx < 0 || static_cast<int>(m_poly.size()) <= idx )
	{
		return;
	}

	navMeshPoly& poly = m_poly.at( idx );

	VEC3D v[3];
	for( int i = 0; i < 3; ++i )
	{
		i3Vector::Copy( &v[i], GetVec3D( poly.n[i] ) );

		// calculate min, max
		{
			if( 0 == i )
			{
				i3Vector::Copy( &poly.vmin, &v[i] );
				i3Vector::Copy( &poly.vmax, &v[i] );
			}
			else
			{
				i3Vector::Minimize( &poly.vmin, &poly.vmin, &v[i] );
				i3Vector::Maximize( &poly.vmax, &poly.vmax, &v[i] );
			}
		}
	}

	// calculate normal
	{
		VEC3D e0, e1;
		i3Vector::Sub( &e0, &v[1], &v[0] );
		i3Vector::Sub( &e1, &v[2], &v[0] );
		i3Vector::Cross( &poly.normal, &e0, &e1 );
		i3Vector::Normalize( &poly.normal, &poly.normal );
	}

	// calculate axis angle
	{
		VEC3D vup;
		i3Vector::Set( &vup, 0.0f, 1.0f, 0.0f );
		i3Vector::Cross( &poly.axis, &vup, &poly.normal );
		poly.angle = ::acos( (vup.x * poly.normal.x + vup.y * poly.normal.y + vup.z * poly.normal.z) );
	}

	// calculate center
	{
		i3Vector::Set( &poly.center, 0.0f, 0.0f, 0.0f );

		for( int i = 0; i < 3; ++i )
			i3Vector::Add( &poly.center, &poly.center, &v[i] );

		poly.center.x /= 3;
		poly.center.y /= 3;
		poly.center.z /= 3;
	}
}

void	i3NavMeshDataMgr::CleanUp()
{
	m_UsingVertex.clear();

	m_vertex.clear();	
	m_poly.clear();

	m_VertexKeyList.clear();
	m_PolyKeyList.clear();
}

bool	i3NavMeshDataMgr::_CalculateNavMeshData( rcPolyMesh* pmesh )
{
	CleanUp();

	if( nullptr == pmesh )
	{
		return false;
	}

	i3::vector< navMeshVertex > vertexList;
	vertexList.reserve( pmesh->npolys *3 );

	const int nvp = pmesh->nvp;
	const float cs = pmesh->cs;
	const float ch = pmesh->ch;
	const float* orig = pmesh->bmin;

	for (int i = 0; i < pmesh->npolys; ++i)
	{
		const unsigned short* p = &pmesh->polys[i*nvp*2];

		unsigned short vi[3];
		for (int j = 2; j < nvp; ++j)
		{
			if (p[j] == RC_MESH_NULL_IDX) break;

			vi[0] = p[0];
			vi[1] = p[j-1];
			vi[2] = p[j];

			for (int k = 0; k < 3; ++k)
			{
				const unsigned short* v = &pmesh->verts[vi[k]*3];
				const float x = orig[0] + v[0]*cs;
				const float y = orig[1] + v[1]*ch;
				const float z = orig[2] + v[2]*cs;

				navMeshVertex vertex;

				vertex.x = x;
				vertex.y = y;
				vertex.z = z;

				vertexList.push_back( vertex );
			}
		}
	}

	return SetNavMeshData( vertexList );
}

bool	i3NavMeshDataMgr::PickPolygon( const POINT& pt, i3::vector< navMeshPick >& pickList )
{
	pickList.clear();

	i3CollideeLine line;
	_CalculateCollideeLine( line, pt );

	int idx = 0;	

	VEC3D v[3];
	//VEC3D norm;

	navMeshPick pick;

	for each( const navMeshPoly& poly in m_poly )
	{
		if( true == IsRemovedPolygon( idx ) )
		{
			++idx;
			continue;
		}

		for( int i = 0; i < 3; ++i )
		{
			i3Vector::Copy( &v[i], &m_vertex[ poly.n[i] ].vec3D );
		}

		if( FALSE == i3Math::TestTriPoint( &v[0], &v[2], &v[1], line.GetStart(), line.GetDir() ) )
		{
			if( FALSE == i3Math::TestTriPoint( &v[0], &v[1], &v[2], line.GetStart(), line.GetDir() ) )
			{
				++idx;
				continue;
			}
		}
		

		// calculate normal
		{
			/*VEC3D e0, e1;
			i3Vector::Sub( &e0, &v[1], &v[0] );
			i3Vector::Sub( &e1, &v[2], &v[0] );
			i3Vector::Cross( &norm, &e0, &e1 );
			i3Vector::Normalize( &norm, &norm );*/
		}

		if( TRUE == IntersectTriLine( line.GetStart(), line.GetEnd(), (VEC3D*) &poly.normal, &v[0], &pick.pc, &pick.t ) )
		{
			pick.idx = idx;
			pickList.push_back( pick );
		}

		++idx;
	}

	return false == pickList.empty();
}

bool	i3NavMeshDataMgr::GetPickPolygon( const POINT& pt, int& nPickPoly )
{
	nPickPoly = -1;

	i3::vector< navMeshPick > pickList;
	if( false == PickPolygon( pt, pickList ) )
	{
		return false;
	}

	bool bFirst = true;	
	float tmin = 0.0f;

	for each( const navMeshPick& pick in pickList )
	{
		if( true == bFirst )
		{
			tmin		= pick.t;
			nPickPoly	= pick.idx;

			bFirst = false;
		}
		else
		{
			if( pick.t < tmin )
			{
				tmin		= pick.t;
				nPickPoly	= pick.idx;
			}
		}
	}

	return true;
}

bool	i3NavMeshDataMgr::GetPickPolygon( const POINT& pt, navMeshPick& pick )
{
	pick.idx = -1;	

	i3::vector< navMeshPick > pickList;
	if( false == PickPolygon( pt, pickList ) )
	{
		return false;
	}

	bool bFirst = true;

	for each( const navMeshPick& pick_data in pickList )
	{
		if( true == bFirst )
		{			
			pick	= pick_data;
			bFirst	= false;
		}
		else
		{
			if( pick_data.t < pick.t )
			{
				pick = pick_data;
			}
		}
	}

	return true;
}

bool	i3NavMeshDataMgr::PickVertex( const POINT& pt, i3::vector< navMeshPick >& pickList )
{
	pickList.clear();

	const float radius = (m_fVScale * 0.5f);

	i3CollideeLine line;
	_CalculateCollideeLine( line, pt );	

	navMeshPick pick;
	int idx = 0;

	for each( const navMeshVertex& v in m_vertex )
	{
		if( true == IsRemovedVertex( idx ) )
		{
			++idx;
			continue;
		}

		if( TRUE == i3Math::IntersectLineToSphere( line.GetStart(), line.GetEnd(),(VEC3D*) &v.vec3D, radius * radius, &pick.pc, &pick.t ) )
		{
			pick.idx = idx;
			pickList.push_back( pick );
		}

		++idx;
	}

	return false == pickList.empty();
}

bool	i3NavMeshDataMgr::GetPickVertex( const POINT& pt, int &nPickVertex )
{
	nPickVertex = -1;

	i3::vector< navMeshPick > pickList;
	if( false == PickVertex( pt, pickList ) )
	{
		return false;
	}

	bool bFirst = true;
	REAL32 tmin = 0;

	for each( const navMeshPick& pick in pickList )
	{
		if( true == bFirst )
		{
			tmin		= pick.t;
			nPickVertex	= pick.idx;

			bFirst = false;
		}
		else
		{
			if( pick.t < tmin )
			{
				tmin		= pick.t;
				nPickVertex	= pick.idx;
			}
		}
	}

	return true;
}


bool	i3NavMeshDataMgr::DoAutoGeneration( i3World* world, const navMeshConfig& cfg, fnAutoGenLoadingRatio pFn )
{
	CleanUp();

	if( pFn ) 
		(*pFn)( 0.0f, "Loading..." );

	bool bRes = false;

	if( nullptr == m_pGenerator )
		m_pGenerator = i3NavMeshGenerator::new_object();
	
	bRes = m_pGenerator->DoGenerate( world, cfg, pFn );
	
	if( pFn ) 
		(*pFn)( 0.9f, "Set Nvigation Mesh Data..." );

	if( bRes )
		bRes = _CalculateNavMeshData( m_pGenerator->GetPolyMesh() );

	if( pFn ) 
		(*pFn)( 1.0f, "Set Nvigation Mesh Data..." );

	return bRes;
}

bool	i3NavMeshDataMgr::SetNavMeshData( const i3::vector< navMeshVertex >& vertexList )
{
	CleanUp();

	if( true == vertexList.empty() )
	{
		return false;
	}

	I3ASSERT( 0 == (vertexList.size() % 3) );

	navMeshVertex vertex;
	navMeshPoly poly;

	const int nPolyCnt = static_cast<int>(vertexList.size() /3);

	for( int i = 0; i < nPolyCnt; ++i )
	{
		for( int  k = 0; k < 3; ++k )
		{
			vertex = vertexList[ i*3 +k ];
		
			int _id = -1;

			map_vertexkey::iterator itr = m_VertexKeyList.find( vertex );

			if( itr == m_VertexKeyList.end() )
			{	
				_id = static_cast<int>( m_VertexKeyList.size() );
				m_VertexKeyList.insert( i3::make_pair( vertex, _id ) );
				m_vertex.push_back( vertex );
			}
			else
			{
				_id = itr->second;
			}

			poly.n[ k ] = _id;
		}

		map_polykey::iterator itr = m_PolyKeyList.find( poly );

		if( itr == m_PolyKeyList.end() )
		{
			int idx = m_poly.size();

			m_PolyKeyList.insert( i3::make_pair( poly, idx ) );
			m_poly.push_back( poly );

			//
			_AddUsingVertex( poly );

			// update poly normal
			_UpdatePolyNormal( idx );
		}
	}

	return true;
}

bool	i3NavMeshDataMgr::CreatePolygon( navMeshPoly poly )
{
	VEC3D v[3], norm;

	if( m_PolyKeyList.find( poly ) != m_PolyKeyList.end() )
	{
		return false;
	}

	for( int i = 0; i < 3; ++i )
	{
		i3Vector::Copy( &v[i], &m_vertex[ poly.n[i] ].vec3D );
	}

	// calculate normal
	{
		VEC3D e0, e1;
		i3Vector::Sub( &e0, &v[1], &v[0] );
		i3Vector::Sub( &e1, &v[2], &v[0] );
		i3Vector::Cross( &norm, &e0, &e1 );
		i3Vector::Normalize( &norm, &norm );
	}

	if( norm.y < 0.0f )
	{
		int temp = poly.n[1];
		poly.n[1] = poly.n[2];
		poly.n[2] = temp;

		norm.y = (-norm.y);
	}

	//
	{
		i3CollideeLine line;

		VEC3D test_v[3];
		VEC3D test_norm;
		VEC3D vcenter, vdir, pc;

		REAL32 t;

		for( int i = 0; i < 3; ++i )
		{
			vcenter.vec[i] = ((v[0].vec[i] + v[1].vec[i] + v[2].vec[i]) /3);
		}

		int idx = 0;

		for each( navMeshPoly test_poly in m_poly )
		{
			if( true == IsRemovedPolygon( idx ) )
			{
				++idx;
				continue;
			}

			for( int i = 0; i < 3; ++i )
			{
				i3Vector::Copy( &test_v[i], &m_vertex[ test_poly.n[i] ].vec3D );
			}

			// calculate normal
			{
				VEC3D e0, e1;
				i3Vector::Sub( &e0, &test_v[1], &test_v[0] );
				i3Vector::Sub( &e1, &test_v[2], &test_v[0] );
				i3Vector::Cross( &test_norm, &e0, &e1 );
				i3Vector::Normalize( &test_norm, &test_norm );
			}		

			i3Vector::Set( &vdir, 0, -1, 0 );
			if( TRUE == i3Math::TestTriPoint( &test_v[0], &test_v[2], &test_v[1], &vcenter, &vdir  ) )
			{
				line.SetStart( &vcenter );
				line.SetDir( &vdir );

				if( TRUE == I3NAVMESH::IntersectLineToTri( line.GetStart(), line.GetEnd(), &test_norm, &test_v[0], &pc, &t ) )
				{
					return false;
				}
			}

			i3Vector::Set( &vdir, 0, 1, 0 );
			if( TRUE == i3Math::TestTriPoint( &test_v[0], &test_v[2], &test_v[1], &vcenter, &vdir  ) )
			{
				line.SetStart( &vcenter );
				line.SetDir( &vdir );

				if( TRUE == I3NAVMESH::IntersectLineToTri( line.GetStart(), line.GetEnd(), &test_norm, &test_v[0], &pc, &t ) )
				{
					return false;
				}
			}

			++idx;
		}
	}


	//  add poly
	{
		int idx = m_poly.size();

		m_PolyKeyList.insert( i3::make_pair( poly, idx ) );
		m_poly.push_back( poly );

		//
		_AddUsingVertex( poly );

		// update poly normal
		_UpdatePolyNormal( idx );
	}

	return true;
}

bool	i3NavMeshDataMgr::RemovePolygon( int idx )
{
	if( idx < 0 || static_cast<int>(m_poly.size()) <= idx )
	{
		return false;
	}

	_SetUsePolygon( idx, false );

	//m_RemovePoly.insert( idx );

	////
	//_RemoveUsingVertex( m_poly[ idx ] );
	return true;
}

bool	i3NavMeshDataMgr::InvertPolygonNormal( int idx )
{
	if( idx < 0 || static_cast<int>(m_poly.size()) <= idx )
	{
		return false;
	}

	int ntemp = m_poly[ idx ].n[1];
	m_poly[ idx ].n[1] = m_poly[ idx ].n[2];
	m_poly[ idx ].n[2] = ntemp;

	_UpdatePolyNormal( idx );
	return true;
}

bool	i3NavMeshDataMgr::MoveVertex( int idx, const VEC3D& vec )
{
	if( idx < 0 || static_cast<int>(m_vertex.size()) <= idx )
	{
		return false;
	}

	navMeshVertex& v = m_vertex[idx];
	i3Vector::Add( &v.vec3D, &v.vec3D, (VEC3D*) &vec );

	// update poly normal
	{
		i3::vector< int > usingList;
		GetUsingPoly( idx, usingList );

		for each( int n in usingList )
		{
			_UpdatePolyNormal( n );
		}
	}	

	return true;
}

bool	i3NavMeshDataMgr::Find_RemovedPolygonIdx( const navMeshPoly& p, int& idx )
{
	idx = -1;

	map_polykey::iterator itr;

	itr = m_PolyKeyList.find( p );
	if( m_PolyKeyList.end() == itr )
	{
		return false;
	}

	/*if( m_RemovePoly.end() == m_RemovePoly.find( itr->second ) )
	{
		return false;
	}*/

	if( false == IsRemovedPolygon( itr->second ) )
	{
		return false;
	}

	idx = itr->second;
	return true;
}

navMeshVertex*	i3NavMeshDataMgr::GetVertex( int idx )
{
	if( idx < 0 || static_cast<int>(m_vertex.size()) <= idx )
	{
		return nullptr;
	}

	return &m_vertex[ idx ];
}

navMeshPoly*	i3NavMeshDataMgr::GetPolygon( int idx )
{
	if( idx < 0 || static_cast<int>(m_poly.size()) <= idx )
	{
		return nullptr;
	}

	return &m_poly[ idx ];
}

const navMeshVertex*	i3NavMeshDataMgr::GetVertex( int idx ) const
{
	if( idx < 0 || static_cast<int>(m_vertex.size()) <= idx )
	{
		return nullptr;
	}

	return &m_vertex[ idx ];
}

const navMeshPoly*	i3NavMeshDataMgr::GetPolygon( int idx ) const
{
	if( idx < 0 || static_cast<int>(m_poly.size()) <= idx )
	{
		return nullptr;
	}

	return &m_poly[ idx ];
}


int		i3NavMeshDataMgr::GetUsingVertexCount() const
{
	return static_cast< int >(m_UsingVertex.size());
}

int		i3NavMeshDataMgr::GetUsingPolygonCount() const
{
	int nRet = 0;

	const int nCnt = GetPolygonCount();
	for( int i = 0; i < nCnt; ++i )
	{
		if( false == IsRemovedPolygon( i ) )
		{
			++nRet;
		}

	}

	return nRet;
}

void	i3NavMeshDataMgr::ReUseVertex( int idx )
{
	_SetUseVertex( idx, true );
}

void	i3NavMeshDataMgr::ReUsePolygon( int idx )
{
	_SetUsePolygon( idx, true );
}

void	i3NavMeshDataMgr::_SetUseVertex( int idx, bool bUse )
{
	navMeshVertex* pv = GetVertex( idx );

	if( nullptr != pv )
		pv->bRemoved = (!bUse);
}

void	i3NavMeshDataMgr::_SetUsePolygon( int idx, bool bUse )
{
	navMeshPoly* pp = GetPolygon( idx );

	if( nullptr != pp )
	{
		pp->bRemoved = (!bUse);

		if( bUse )
			_AddUsingVertex( *pp );
		else
			_RemoveUsingVertex( *pp );
	}
}

bool	i3NavMeshDataMgr::IsRemovedVertex( int idx ) const	
{
	const navMeshVertex* pv = GetVertex( idx );

	if( nullptr == pv )
	{
		return true;
	}
	
	return pv->bRemoved;
}

bool	i3NavMeshDataMgr::IsRemovedPolygon( int idx ) const	
{
	const navMeshPoly* pp = GetPolygon( idx );

	if( nullptr == pp )
	{
		return true;
	}

	return pp->bRemoved;
}

void	i3NavMeshDataMgr::_AddUsingVertex( const navMeshPoly& poly )
{
	for( int i = 0; i < 3; ++i )
		_AddUsingVertex( poly.n[i] );
}

void	i3NavMeshDataMgr::_RemoveUsingVertex( const navMeshPoly& poly )
{
	for( int i = 0; i < 3; ++i )
		_RemoveUsingVertex( poly.n[i] );
}

void	i3NavMeshDataMgr::_AddUsingVertex( int idx )
{
	i3::set< _UsingVertex >::iterator itr;

	itr = m_UsingVertex.find( idx );
	if( itr != m_UsingVertex.end() )
	{
		itr->ncnt++;
		return;
	}

	m_UsingVertex.insert( _UsingVertex( idx ) );
}

void	i3NavMeshDataMgr::_RemoveUsingVertex( int idx )
{
	i3::set< _UsingVertex >::iterator itr;

	itr = m_UsingVertex.find( idx );
	if( itr != m_UsingVertex.end() )
	{
		itr->ncnt--;

		if( itr->ncnt <= 0 )
			m_UsingVertex.erase( idx );
	}
}

bool	i3NavMeshDataMgr::CutPolyLine( const POINT& pt, navMeshCuttingInfo& info )
{
	info.reset();

	navMeshPick pick;

	if( false == GetPickPolygon( pt, pick ) )
	{
		return false;
	}	

	const navMeshPoly* pPoly = GetPolygon( pick.idx );

	if( nullptr == pPoly )
	{
		return false;
	}

	static const int c_LineIndex[6] =  
	{
		1, 2, 
		0, 2, 
		0, 1, 
	};

	int nSelectedLine = -1;
	VEC3D vpc;

	// 피킹된 선분 검출
	{
		struct _pickLine
		{
			int		idx;
			float	t;
			VEC3D	pc;
		};

		_pickLine pkLine;

		i3CollideeLine line;
		_CalculateCollideeLine( line, pt );

		//ts::vector< ts::pair< int, float > > vecIntersect;
		i3::vector< _pickLine > vecIntersect;

		VEC3D v[3];
		VEC3D vLine[2];
		VEC3D vr1, vr2;

		float fmin = 0.f;

		for( int i = 0; i < 3; ++i )
		{
			i3Vector::Copy( &v[i], GetVec3D( pPoly->n[i] ));
		}

		for( int i = 0; i < 3; ++i )
		{
			for( int k = 0; k < 2; ++k )
			{
				const int nLineIdx = c_LineIndex[ i * 2 + k ];
				i3Vector::Copy( &vLine[k], &v[ nLineIdx ] );
			}

			if( true == I3NAVMESH::IntersectLineToLine( &vLine[0], &vLine[1], line.GetStart(), line.GetEnd(), &vr1, &vr2 ) )
			{
				VEC3D vsub;
				i3Vector::Sub( &vsub, &vr1, &vr2 );

				const float flen = i3Vector::Length( &vsub );

				if( 0.2f < flen )
				{
					continue;
				}

				pkLine.idx	= i;
				pkLine.t	= flen;
				i3Vector::Copy( &pkLine.pc, &vr1 );

				//vecIntersect.push_back( ts::make_pair( i, flen ) );
				vecIntersect.push_back(pkLine);
			}
		}

		//for each( ts::pair< int, float > p in vecIntersect )
		for each( const _pickLine& pl in vecIntersect )
		{
			if( nSelectedLine == -1 )
			{
				/*nSelectedLine = p.first;
				fmin = p.second;*/
				nSelectedLine	= pl.idx;
				fmin			= pl.t;

				i3Vector::Copy( &vpc, (VEC3D*) &pl.pc );
			}
			else
			{
				if( pl.t < fmin )
				{
					nSelectedLine	= pl.idx;
					fmin			= pl.t;
					i3Vector::Copy( &vpc, (VEC3D*) &pl.pc );
				}
			}
		}
	}

	if( 0 <= nSelectedLine && nSelectedLine < 3 )
	{
		info._type = navMeshCuttingInfo::LINE;

		std::tr1::array< int, 2 > selLine;
		std::tr1::array< int, 2 > selChecker;
		std::tr1::array< int, 3 > otherPoly;

		// 피킹된 면 결과
		{
			info.selPoly	= pick.idx;
			info.cutVertex	= pPoly->n[ nSelectedLine ];
		}

		//
		{
			VEC3D v[2];

			for( int i = 0; i < 2; ++i )
			{	
				const int idx = pPoly->n[ c_LineIndex[ nSelectedLine * 2 + i ] ];

				selLine[ i ] = idx;
				i3Vector::Copy( &v[ i ], GetVec3D( idx ) );
			}

			i3Vector::Copy( &info.cutPosition, &vpc );
		}

		// 자르려는 선분이 사용되는 다른 면 검출
		{
			int idx = 0;
			for each( const navMeshPoly& poly in m_poly )
			{
				if( true == IsRemovedPolygon( idx ) || pick.idx == idx )
				{
					++idx;
					continue;
				}

				for( int i = 0; i < 3; ++i )
				{
					otherPoly[i] = poly.n[i];
				}

				int ncheck = 0;

				for( int i = 0; i < 3; ++i )
				{
					for( int k = 0; k < 2; ++k ) 
					{
						if( otherPoly[i] == selLine[k] )
						{
							selChecker[ ncheck++ ] = i;
							break;
						}
					}
				}

				if( 2 == ncheck )
				{
					int nother = -1;
					for( int i = 0; i < 2; ++i)
					{
						if( selChecker[i] != i )
						{
							nother = otherPoly[i];
							break;
						}
					}

					if( -1 == nother )
						nother = otherPoly[2];

					if( nother != info.cutVertex )
					{
						// 다른 면
						info.otherPoly		= idx;
						info.otherCutVertex	= nother;					
						break;
					}
				}

				++idx;			
			}
		}

		info.cutLine[0] = selLine[0];
		info.cutLine[1] = selLine[1];

		return true;
	}
	else
	{
		info._type		= navMeshCuttingInfo::POLY;		
		info.selPoly	= pick.idx;
		
		i3Vector::Copy( &info.cutPosition, &pick.pc );

		return true;
	}
}

bool	i3NavMeshDataMgr::GetASEData( i3::vector< navMeshPoly >& polyList, i3::vector< navMeshVertex >& vertexList ) const
{
	const int nPolygonCnt = GetUsingPolygonCount();

	if( nPolygonCnt <= 0 )
	{
		return true;
	}

	navMeshVertex tmpVertex;
	i3::vector< navMeshVertex > tmpList;

	tmpList.reserve( nPolygonCnt *3 );

	for each( const navMeshPoly& p in m_poly )
	{
		if( p.bRemoved )
		{
			continue;
		}

		for( int i = 0; i < 3; ++i )
		{
			tmpVertex = m_vertex[ p.n[i] ];
			tmpList.push_back( tmpVertex );
		}
	}

	map_vertexkey keyList;

	navMeshPoly p;
	int npoly = 0;

	for each( const navMeshVertex v in tmpList )
	{
		int _id = -1;
		map_vertexkey::iterator itr = keyList.find( v );

		if( itr == keyList.end() )
		{
			_id = static_cast<int>( keyList.size() );
			keyList.insert( i3::make_pair( v, _id ) );
			vertexList.push_back( v );
		}
		else
		{
			_id = itr->second;
		}

		p.n[ npoly ] = _id;

		npoly++;
		if( 3 == npoly )
		{
			npoly = 0;
			polyList.push_back( p );
		}
	}

	return true;
}
