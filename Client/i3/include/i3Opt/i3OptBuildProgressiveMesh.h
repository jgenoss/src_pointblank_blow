#if !defined( __I3_OPT_BUILDPROGRESSIVE_H_)
#define __I3_OPT_BUILDPROGRESSIVE_H_

#include "i3SceneOptimizer.h"
#include "i3OptQuadric.h"
#include "i3Base/smart_ptr/pscoped_ptr.h"
#include "i3Base/smart_ptr/shared_ptr.h"
#include "i3Base/itl/vector_set.h"

struct ERRINFO
{
	INT32		min = 0, max = 0, key = 0;
	double		err = 0.0;
	INT32		deltaPivot = 0;
	i3NVector	vNew;
	void		Free(void);
};


struct Neighbor_ERRINFO_Comp
{
	bool	operator()( const ERRINFO* p1, const ERRINFO* p2) const
	{
		if ( p1->min < p2->min) return true;
		if ( p1->min == p2->min && p1->max < p2->max) return true;
		return false;
	}
};

struct ErrValue_ERRINFO_Comp
{
	bool	operator()( const ERRINFO* p1, const ERRINFO* p2) const
	{
		return ( p1->err > p2->err)	;
	}
};

//
// alloc대신 생성자로 처리해야한다..
//
struct QUADRIC 
{
	QUADRIC() {}
	~QUADRIC() {}

	i3Quadric		_Quad;
	i3NVector		_NVec;

	INT32			_mark = 0;
	bool			_isValid = false;

	i3::pscoped_ptr< i3::vector<INT32> >	plistNeighbor;
	i3::pscoped_ptr< i3::vector_multiset<ERRINFO*, Neighbor_ERRINFO_Comp> > plistEdge;

//	void			Free(void);
};

class I3_EXPORT_OPT i3OptBuildProgressiveMesh : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildProgressiveMesh, i3SceneOptimizer);

protected:
	//volatile
	i3VertexArray	*	m_pVA = nullptr;
	i3IndexArray	*	m_pIA = nullptr;
	INT32				m_PrimCount = 0;

	INT32				m_CurDim = 0;
	INT32				m_uvDim = -1;
	INT32				m_normDim = -1;

	INT32				m_Vcnt = 0;
	INT32				m_Icnt = 0;

	QUADRIC		*		m_pQuadric = nullptr;
	I_FACE		*		m_pPlane = nullptr;

	ERRINFO		*		m_pErr = nullptr;

//	List		*		m_plistPlane;						// 쓰임이 없어보여 우선 막고 나중에 필요하면 주석을 푼다..
	
	i3::shared_ptr< i3::vector<I3GEOM_VSPLIT*> > m_plistVSplit;
	i3::pscoped_ptr< i3::vector<I_FACE* > >		 m_plistFace;

	i3::vector_multiset<ERRINFO*>				 m_listErr;	
	i3::vector_multiset<ERRINFO*, ErrValue_ERRINFO_Comp>	m_listErrValue;

	i3TextureBindAttr * m_pcurTextureBind = nullptr;

public:
	i3OptBuildProgressiveMesh(void);
	virtual ~i3OptBuildProgressiveMesh(void);

protected:
	void	Clear();

	//bool	_isSymmetric( MATRIX* m);									// Transpose하여도 같은 값을 가지는 Matrix인지 판별
	//bool	_checkisValidErrorTable(void);								// TempFunc ErrorTable이 유효한지 체크
	bool	_isClosedVertex( ERRINFO * pInfo);							// Edge가 ClosedVertex만을 포함하고 있는지 체크
	bool	_isClosedVertex( INT32 vid);

	INT32		_getOppositeVert( INT32 vid, i3::vector<INT32>& listOpp);
	INT32		_addEdge( INT32 vid, ERRINFO * pEdge);
	i3::vector_multiset<ERRINFO*, Neighbor_ERRINFO_Comp>*	_getEdge( INT32 vid);
	INT32		_removeEdge( INT32 vid, INT32 viDel);

	void	_UnlinkNeighbor( INT32 vid, INT32 fid);
	void	_UnlinkNeighbor( INT32 fid);
	void	_ClearNeighbor( INT32 vid);

	INT32	_addNeighbor( INT32 vid, INT32 fid);
	i3::vector<INT32>*	_getNeighbor( INT32 vid);
	//void	_clearNeighbor(INT32 vid);

	bool	_isValidVertex( INT32 vid);
	void	_setValidVertex( INT32 vid ,bool bValid = true);
	INT32	_getVMarked( INT32 vid);
	INT32	_getFMarked( INT32 fid);
	
	void	_markVert( INT32 vid, INT32 mark);
	void	_markface( INT32 fid, INT32 mark);
	void	_markCorners( INT32 vid, INT32 mark);
	void	_markNeighbors( INT32 vid, INT32 mark);
	void	_markNeighbors_delta( INT32 vid, INT32 markDelta);

	void	_partition_marked_neighbors(INT32 vid, INT32 pivot, i3::vector<INT32>& lChange, i3::vector<INT32>& lDelete);

	void	_collect_Unmarked_corners( INT32 vid, i3::vector<INT32>& VertList);
	void	_collect_Unmarked_neighbors( INT32 vid1, i3::vector<INT32>& FaceList);
	void	_collect_vertx_star( INT32 vid, i3::vector<INT32>& Star);
	void	_collect_edge_neighbor( INT32 vid1, INT32 vid2, i3::vector<INT32>& FaceList);
	void	_discontinuity_constraint(INT32 vid1, INT32 vid2, i3::vector<INT32>& FaceList);

	i3NVector* _getVector( INT32 vid);

	double * _getVertex( INT32 vid);
	double * _getVertex( i3NVector * pv);
	void	 _getVertex( INT32 vid, VEC3D *pv);
	void	 _getVertex( i3NVector * p, VEC3D *pv);
	void	 _setVertex( INT32 vid, VEC3D *pv);

	double * _getUV( INT32 vid);
	double * _getUV( i3NVector * pv);
	void	 _getUV( INT32 vid, VEC2D *pv);
	void	 _getUV( i3NVector *uv, VEC2D *pv);
	void	 _setUV( INT32 vid, VEC2D *pv);

	double * _getNorm( INT32 vid);
	double * _getNorm( i3NVector * pv);
	void	 _getNorm( INT32 vid, VEC3D *pv);
	void	 _getNorm( i3NVector * pv, VEC3D *pn);
	void	 _setNorm( INT32 vid, VEC3D *pv);

	double	_getRegion( INT32 vid);
	
	double	_calcErr( QUADRIC * pQuad, ERRINFO* pInfo);	//주어진 edge가 사라질 경우의 error값을 구한다.
	void	_CalcPlaneNormal( I_FACE * pPlane, i3NVector * pvNormal);
	
	/*bool _findOptimalVertPos( MATRIX * pMat, VEC3D * vPos, VEC3D * pOut);
	bool _findOptimalVertPos( MATRIX * pMat,  VEC3D * vPos, VEC3D * v1, VEC3D * v2, VEC3D * pOut);
	bool _findOptimalVertPos( MATRIX * pMtx, VEC3D *vPos, VEC3D*v1, VEC3D*v2,VEC3D*v3, VEC3D * pOut);*/

	// Error list관련
	void	_updateError( INT32 vid);
	INT32	_addError( ERRINFO * pErr,  bool bCalcErr, bool bIgnore);
	INT32	_removeErr( ERRINFO * pErr);
	INT32	_removeErr( INT32 idx);

	// Matrix를 더함
	//double	_addQuadric( MATRIX* m, MATRIX* m1, MATRIX* m2);
	//double  _addQuadric( QUADRIC* m, QUADRIC* m1, QUADRIC* m2);
	//double	_scaleQuadric( MATRIX* m, MATRIX* m1, double s);

	// Plane의 계수를 구한다.
	INT32	_CalcPlane( I_FACE * pvPlane, INT32 ibCnt);

	// 각 정점의 Error값을 초기화한다.
	INT32	_InitQuadrics( QUADRIC * pQuadric, I_FACE * pPlane);

	// 유효하며 겹치지 않는 Edge의 리스트를 구하고, Error값을 계산하여 pair를 초기화한다.
	void	_selectPair( double Threshold);

	//
	void	_constrain_Boundaries();
	
	void	_calcContract( ERRINFO * pMinErr, i3::vector<INT32>& lChange, i3::vector<INT32>& lDelete);
	void	_moveEdgeContract( ERRINFO * pMinErr);
	void	_applyContract( ERRINFO * pMinErr, i3::vector<INT32>& listChage, i3::vector<INT32>& listDelete, I3GEOM_VSPLIT * pVSplit);

	
	ERRINFO * _FindMinimalErr(void);

public:	//Test Function
	void	IncreasePrimCount( bool bIncrease);
	void	VerifyClosedVertex();
	INT32	DumpGeometry( i3GeometryAttr * pGeom);
	INT32	DumpGeometry(void);
	void	CountValidVertex(void);
//	bool	_isvaliderrtable(BinList * plist);				// 구현이 되지 않음..쓰이지 않는듯 하여 주석..

	INT32	_WeldVertices();
	void	MakeVertexColor( i3GeometryAttr * pGeom);

public:
	INT32	_CalcDimension( i3VertexArray * pVA);
	void	_CreateQuadrics(void);

public:
	INT32	BuildProgressive( INT32 nGoalFaceCount);
	INT32	Construct_N_Contract( INT32 targetFaceCount);
	void	_DumpData( i3GeometryAttr2 * pGeomAttr2);

public:
	bool	Build( i3GeometryAttr * pGeoAttr, i3GeometryAttr2 * pGeoAttr2);

	virtual	bool OnNode( i3Node * pNode, i3Node * pParent) override;

	i3TextureBindAttr * _findCurTex( i3Node * pNode);
};

#endif