#pragma once

#include "i3NavMeshUtil.h"

class i3NavMeshFile;
class i3NavMeshDataMgr;

class i3NavMeshPrimitivePolygon;

class I3_EXPORT_TDK i3NavMesh : public i3Node
{
	I3_EXPORT_CLASS_DEFINE( i3NavMesh, i3Node );

public:
	~i3NavMesh();

public:
	enum
	{
		UPDATE_FRUSTUM			= 0x01,
		UPDATE_REBUILD_VB		= 0x02,
	};

public:
	void		AddUpdateFlag( unsigned int flag )
	{
		m_UpdateFlag |= flag;
	}

public:
	i3NavMeshDataMgr*		GetDataMgr()
	{
		return m_dataMgr; 
	}
	
	void		SetDataMgr( i3NavMeshDataMgr* ptr );
	
public:
	__inline bool			IsVisible() const						{ return m_bVisible; }
	__inline void			SetVisible( bool bVisible )				{ m_bVisible = bVisible; }

	__inline bool			IsVisibleNormal() const					{ return m_bVisibleNormal; }
	__inline void			SetVisibleNormal( bool bVisible )		{ m_bVisibleNormal = bVisible; }

	__inline VEC3D*			GetVertexMovePoint()					{ return &m_vertexMove;	}
	__inline const VEC3D&	GetPickAddVertex() const				{ return m_PickAddVertex; }

	__inline int			GetMouseOverVertex() const				{ return m_MouseOverVertex; }
	__inline int			GetMouseOverPolygon() const				{ return m_MouseOverPoly; }

	__inline bool			IsMouserOverVertex( int idx ) const		{ return m_MouseOverVertex == idx; }
	__inline bool			IsMouserOverPolygon( int idx ) const	{ return m_MouseOverPoly == idx; }

public:
	int			GetVertexCount() const;
	int			GetPolygonCount() const;

	int			GetUsingVertexCount() const;
	int			GetUsingPolygonCount() const;

	float		GetVertexScale() const;
	void		SetVertexScale( float scale );

public:
	void		SetVertexMovePosition( VEC3D* point );
	void		SetPolygonMovePosition( VEC3D* point );

	// file 
public:
	bool		SaveNM( const i3::string& _path );
	bool		LoadNM( const i3::string& _path );

	bool		SaveASE( const i3::string& _path );
	bool		LoadASE( const i3::string& _path );

	// select
public:
	bool		IsSelectedVertex( int idx ) const;
	bool		IsSelectedPolygon( int idx ) const;

	void		GetSelectedVertexList( i3::vector< int >& selList ) const;
	void		GetSelectedPolygonList( i3::vector< int >& selList ) const;

	bool		GetSelectedCenterPosition( bool bVertex, VEC3D& vCenter ) const;
	bool		GetSelectedMinMax( VEC3D& vMin, VEC3D& vMax ) const;
	
public:
	bool		SelectVertex( const POINT& pt, SELECT_NAVMESH_TYPE selMode = SEL_NAVMESH_EXCLUSIVE );
	bool		SelectBandVertex( const POINT& pt1, const POINT& pt2, SELECT_NAVMESH_TYPE selMode = SEL_NAVMESH_EXCLUSIVE );

	bool		SelectPolygon( const POINT& pt, SELECT_NAVMESH_TYPE selMode = SEL_NAVMESH_EXCLUSIVE );
	bool		SelectBandPolygon( const POINT& pt1, const POINT& pt2, SELECT_NAVMESH_TYPE selMode = SEL_NAVMESH_EXCLUSIVE );

	void		ClearSelectList();

public:
	void		UpdateMouseMove_AddVertex( const POINT& pt );

	void		UpdateMouseMove( const POINT& pt, bool bUpdateVertex = true, bool bUpdatePoly = true );
	void		UpdateCameraMove();

	// edit
public:
	bool		GetPickPosition_byWorld( const POINT &pt, VEC3D& pos );

	bool		AddVertex( const POINT& pt );
	bool		AddVertex( const VEC3D& pos, bool bUseUndo = true );
	
	bool		RemoveVertex( int idx, bool bUseUndo = true );
	bool		RemoveVertex( const i3::vector< int >& eraseList, bool bUseUndo = true );
	
	bool		CreatePolygon();
	bool		CreatePolygon( const navMeshPoly& poly, bool bUndo = true, int* pPolyIdx = nullptr );

	bool		RemoveSelectedVertex();
	bool		RemoveSelectedPolygon();

	bool		RemovePolygon( int idx, bool bUseUndo = true );
	bool		RemovePolygon( const i3::vector< int >& eraseList, bool bUseUndo = true );

	bool		MoveSelectedVertex( const VEC3D& vMove );
	bool		MoveVertex( int idx, const VEC3D& vec, bool bUseUndo = true );
	bool		MoveVertex( i3::set< int > idxList, const VEC3D& vec, bool bUseUndo = true );	

	bool		InvertSelectedPolyNormal();

	// undo redo 를 위한 함수
	void		ReUseVertex( int idx );
	void		ReUsePolygon( int idx );

	bool		ClearData();
	bool		ClearNoUseVertex();

public:
	void		SetVertexApplyPoint();

	// common
public:
	bool		Create();

	void		Update();

public:
	bool		DoAutoGenration( i3World* world, const navMeshConfig& cfg );

private:
	void		_CreatePrimitives();	
	void		_SelectVertex( int nPickVertex, SELECT_NAVMESH_TYPE selMode );
	void		_SelectPolygon( int nPickPoly, SELECT_NAVMESH_TYPE selMode );

	void		_UpdateFrustum();

	// common
private:
	int		m_MouseOverVertex = -1;
	int		m_MouseOverPoly = -1;

	i3::set< int >		m_SelectedVertexList;
	i3::set< int >		m_SelectedPolyList;

	unsigned int		m_UpdateFlag = UPDATE_FRUSTUM;

public:
	i3::set< int >		m_InFrustumVertexList;
	i3::set< int >		m_InFrustumPolygonList;

private:
	i3NavMeshPrimitivePolygon*		m_pPrimPoly = nullptr;

	// vertex add
private:
	VEC3D		m_PickAddVertex;
	VEC3D		m_vertexMove;

public:
	const navMeshCuttingInfo&	GetCuttingInfo() const
	{
		return m_cutInfo;
	}

public:
	bool	UpdateCuttingPoly( const POINT& pt );
	bool	CuttingPoly();

private:
	navMeshCuttingInfo		m_cutInfo;

	//
private:	
	i3NavMeshDataMgr*		m_dataMgr = nullptr;
	i3NavMeshFile*			m_nmFile = nullptr;

private:
	bool		m_bVisible = true;
	bool		m_bVisibleNormal = true;
};
