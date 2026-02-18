#pragma once

#include "i3NavMeshUtil.h"
#include "Recast/Recast.h"

class i3NavMesh;
class i3NavMeshGenerator;

class I3_EXPORT_TDK i3NavMeshDataMgr : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshDataMgr, i3ElementBase );

public:
	i3NavMeshDataMgr();
	~i3NavMeshDataMgr();

public:
	i3NavMeshGenerator*		GetGenerator()
	{
		return m_pGenerator;
	}

	const i3::vector< navMeshVertex >&		GetVertexList() const		{ return m_vertex; }
	const i3::vector< navMeshPoly >&		GetPolygonList() const		{ return m_poly; }

public:
	__inline int		GetVertexCount() const				{ return static_cast<int>( m_vertex.size() ); }
	__inline int		GetPolygonCount() const				{ return static_cast<int>( m_poly.size() ); }

	__inline float		GetVertexScale() const				{ return m_fVScale; }
	__inline void		SetVertexScale( float scale )		{ m_fVScale = scale; }

	bool		IsRemovedVertex( int idx ) const;
	bool		IsRemovedPolygon( int idx ) const;

	int			GetUsingVertexCount() const;
	int			GetUsingPolygonCount() const;

	void		ReUseVertex( int idx );
	void		ReUsePolygon( int idx );

protected:
	void		_SetUseVertex( int idx, bool bUse );
	void		_SetUsePolygon( int idx, bool bUse );

public:
	navMeshVertex*		GetVertex( int idx );
	navMeshPoly*		GetPolygon( int idx );

	const navMeshVertex*		GetVertex( int idx ) const;
	const navMeshPoly*			GetPolygon( int idx ) const;


	const VEC3D*	GetVec3D( int idx ) const;
	VEC3D*			GetVec3D( int idx );

	bool		UsingVertexInPolygon( int nVertex ) const;
	bool		Find_RemovedPolygonIdx( const navMeshPoly& p, int& idx );	

	void		GetUsingPoly( int nVertex, i3::vector< int >& usingList ) const;

	/************************************************************************/
	/* ASE 저장을 위해 필요없는 데이타를 뺀 순수 데이타를 리턴함		*/
	/************************************************************************/
	bool		GetASEData( i3::vector< navMeshPoly >& polyList, i3::vector< navMeshVertex >& vertexList ) const;

public:	
	bool		Init( i3NavMesh* pNav )
	{
		I3ASSERT( nullptr != pNav );

		m_pNavMesh = pNav;		
		return nullptr != m_pNavMesh;
	}

	bool		DoAutoGeneration( i3World* world, const navMeshConfig& cfg, fnAutoGenLoadingRatio pFn = nullptr );	
	bool		SetNavMeshData( const i3::vector< navMeshVertex >& vertexList );

public:
	void		CleanUp();

	bool		AddVertex( const VEC3D& vec );

	bool		RemoveVertex( int idx, bool bCheckUnUse = true );
	bool		RemoveVertexList( const i3::vector< int >& vecList, bool bCheckUnUse = true );

	bool		MoveVertex( int idx, const VEC3D& vec );

	bool		CreatePolygon( navMeshPoly poly );
	bool		RemovePolygon( int idx );

	bool		InvertPolygonNormal( int idx );

public:
	bool		GetPickVertex( const POINT& pt, int& nPickVertex );
	bool		GetPickPolygon( const POINT& pt, int& nPickPoly );
	bool		GetPickPolygon( const POINT& pt, navMeshPick& pick );

	bool		PickVertex( const POINT& pt, i3::vector< navMeshPick >& pickList );
	bool		PickPolygon( const POINT& pt, i3::vector< navMeshPick >& pickList );

public:
	//bool		CutPolyLine( const POINT& pt, VEC3D& cutPos, int& resPoly1, int& resPoly2, int& resVertex1, int& resVertex2 );
	bool		CutPolyLine( const POINT& pt, navMeshCuttingInfo& info );


private:
	bool		_CalculateNavMeshData( rcPolyMesh* pmesh );
	void		_CalculateCollideeLine( i3CollideeLine &line, const POINT &pt );

	void		_UpdatePolyNormal( int idx );

private:
	typedef i3::map< navMeshVertex, int >		map_vertexkey;
	typedef i3::map< navMeshPoly, int >			map_polykey;

private:
	float			m_fVScale = 0.2f;
	

private:
	i3::vector< navMeshVertex >		m_vertex;
	i3::vector< navMeshPoly >		m_poly;

private:
	struct _UsingVertex
	{
		int		idx;
		int		ncnt;

		_UsingVertex( int _idx ) : idx( _idx ), ncnt( 1 )
		{
		}

		bool	operator < ( const _UsingVertex& rhs ) const
		{
			return idx < rhs.idx;
		}
	};

	i3::set< _UsingVertex >		m_UsingVertex;

private:
	void		_AddUsingVertex( const navMeshPoly& poly );
	void		_RemoveUsingVertex( const navMeshPoly& poly );

	void		_AddUsingVertex( int idx );
	void		_RemoveUsingVertex( int idx );

private:
	map_vertexkey			m_VertexKeyList;	// 정점 키 인덱스
	map_polykey				m_PolyKeyList;		// 폴리곤 키 인덱스

private:
	i3NavMesh*					m_pNavMesh = nullptr;
	i3NavMeshGenerator*			m_pGenerator = nullptr;
};