#pragma once

#include "i3Base/itl/map.h"
#include "i3Base/itl/set.h"

class i3LevelNaviMesh;

class I3_EXPORT_TDK i3LevelNaviMesh_GeometryAttr : public i3GeometryAttr
{
	I3_EXPORT_CLASS_DEFINE( i3LevelNaviMesh_GeometryAttr, i3GeometryAttr);

public:
	i3LevelNaviMesh_GeometryAttr();
	virtual ~i3LevelNaviMesh_GeometryAttr();

public:
	VEC3D*		GetPos()	{ return &m_pos; }
	REAL32		GetScale()	{ return m_scale; }

	void	SetPos( RT_VEC3D* v );
	void	SetScale( REAL32 scale );
	void	SetColor( COLORREAL* cr );

public:
	void	Move( RT_VEC3D* pMove )
	{
		i3Vector::Add( &m_pos, &m_pos, pMove );
	}

public:
	bool	IsPick( VEC3D* p1, VEC3D* p2, REAL32* t );

protected:
	void	_CalculateWorldMatrix( RT_MATRIX& mat );

public:
	virtual void		Apply( i3RenderContext * pContext );

protected:
	i3LevelNaviMesh*		m_pNaviMesh;

protected:
	RT_VEC3D		m_pos;
	REAL32			m_scale;

	COLORREAL		m_cr;
};

enum SEL_POINT_MODE
{
	SEL_POINT_EXCLUSIVE = 0,
	SEL_POINT_INCLUSIVE,
	SEL_POINT_ALTERNATIVE,
	SEL_POINT_UNSELECT,
};

class I3_EXPORT_TDK i3LevelNaviMesh : public i3Node
{
	I3_EXPORT_CLASS_DEFINE( i3LevelNaviMesh, i3Node );

public:
	i3LevelNaviMesh();
	virtual ~i3LevelNaviMesh();

public:
	void		ClearAllPoint()
	{
		if( m_pGeomPoint )
			m_pGeomPoint->RemoveAllGeometryAttr();

		for each( i3LevelNaviMesh_GeometryAttr* ptr in m_PointList )
		{
			ptr->Release();
		}

		m_PointList.clear();
	}

	void		GetSelectPointList( i3::vector< int >& selList );

	void		AddPoint( RT_VEC3D* pos );

	void		MoveSelectedPoint( RT_VEC3D* pMove );
	void		RemoveSelectedPoint();

	void		RemovePoint( i3LevelNaviMesh_GeometryAttr* pDel );
	void		RemovePoint( int idx );
	
	void		SelectPoint( POINT pt, SEL_POINT_MODE selMode );
	void		SelectBandPoint( POINT pt1, POINT pt2, SEL_POINT_MODE selMode );
	
	void		UnSelectPointAll();

public:
	bool	Create();

protected:
	void		_SelectedPoint( i3LevelNaviMesh_GeometryAttr* pickGeom, SEL_POINT_MODE selMode );

	void		_Add_SelectPoint( i3LevelNaviMesh_GeometryAttr* pickGeom );
	void		_Remove_SelectPoint( i3LevelNaviMesh_GeometryAttr* pickGeom );

	void		_Build_SphereMesh( UINT32 hdiv, UINT32 vdiv );
	void		_Build_SelectBoxMesh();

	void	_CleanUp();
	

protected:
	struct _SelPoint
	{
		i3LevelNaviMesh_GeometryAttr*	geom;
		i3LevelNaviMesh_GeometryAttr*	sel;

		_SelPoint() : geom( NULL ), sel( NULL )
		{
		}
	};

protected:
	typedef i3::map< i3LevelNaviMesh_GeometryAttr*, _SelPoint >		map_selpoint;
	typedef i3::vector< i3LevelNaviMesh_GeometryAttr* >				vec_point;

protected:
	map_selpoint		m_SelPointList;
	vec_point			m_PointList;

protected:
	i3VertexArray*		m_pVASphere;
	i3IndexArray*		m_pIASphere;

	i3VertexArray*		m_pVASelBox;
	i3IndexArray*		m_pIASelBox;

protected:
	i3Geometry*			m_pGeomPoint;
	i3Geometry*			m_pGeomSelBox;
};