#if !defined( __I3_OPT_BUILD_BSP_H )
#define __I3_OPT_BUILD_BSP_H

#include "i3SceneOptimizer.h"
#include "../include/i3Scene/i3BSPNode.h"

#define I3_BSP_OPT_COLOR			0x00001000			// no copy color
#define I3_BSP_OPT_TEXCOORD			0x00002000			// no copy texture coord
#define I3_BSP_OPT_NORMAL			0x00004000			// no copy normal vec
#define I3_BSP_OPT_MAX				0x00007000			// all no copy

#define I3_BSP_TYPE_BOX				0x10000000
#define I3_BSP_TYPE_POLYGON			0x20000000			// 폴리곤 단위로 분할
#define I3_BSP_TYPE_GEOMETRY		0x40000000			// 지오메트리 단위로 분할

#define I3_BSP_DIVIDE_SPACE			0x01000000			// 오브젝트를 공간의 중심으로 분할
#define I3_BSP_DIVIDE_OBJCOUNT		0x02000000			// 오브젝트의 수에 따른 분할
#define I3_BSP_DIVIDE_BOUNDBOX		0x04000000

#define I3_BSP_NODE_BINARY			0x00010000
#define I3_BSP_NODE_GRAPH			0x00020000
#define I3_BSP_NODE_DEFAULT			0x00020000

#define I3_BSP_MINMAX_X				0x00000001
#define I3_BSP_MINMAX_Y				0x00000002
#define I3_BSP_MINMAX_Z				0x00000004
#define I3_BSP_MINMAX_ALL			0x00000007

#define I3_BSP_PLANE_FRONT			0x00100000
#define I3_BSP_PLANE_BACK			0x00200000

#define I3_BSP_DEFAULT_DEPTH		8


typedef struct ALIGN4 _tagi3BSPTriIndex
{	
	UINT16				m_nIndex[3];
	UINT32				min, max;

	INT32				m_nSide;
	INT32				m_nFlag;

	i3VertexArray *		m_pVA;
	i3IndexArray *		m_pIA;
	VEC4D *				m_pDividePlane;
	MATRIX *			m_pWorld;

	_tagi3BSPTriIndex()
	{
		m_nSide			= 0;
		m_nFlag			= -1;

		m_pVA			= NULL;
		m_pIA			= NULL;
		m_pDividePlane	= NULL;
		m_pWorld		= NULL;
	}

} i3BSPTriIndex;

typedef struct ALIGN4 _tagi3BSPGeometry
{	
	INT32				m_nTriCount;
	MATRIX				WorldMat;

	_tagi3BSPTriIndex *	pTri;			// 재구성한 Geometry(메모리 영역)
	i3BoundBox *		pBoundBox;		// pTri에 대한 pBoundBox(메모리 역역)

	i3Geometry *		pGeometry;		// 포인터

	_tagi3BSPGeometry()
	{
		m_nTriCount	= 0;
		pGeometry	= NULL;
		pTri		= NULL;
		pBoundBox	= NULL;
		i3Matrix::Identity( &WorldMat );
	}
} i3BSPGeometry;

typedef struct ALIGN4 _tagi3BSPVertex
{
	VEC3D		m_Pos;
	VEC3D		m_Normal;
	COLORREAL	m_Color;
	VEC2D		m_UV;
} i3BSPVertex;

class I3_EXPORT_OPT i3OptBuildBSP : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBuildBSP );
protected:
	UINT32		m_nStyle;

	// Persist
	i3List		m_BSPList;
	i3List		m_BSPGeometryList;

	i3List		m_BSPTriList;
	i3List		m_GeoList;

	// Volatile
	VEC3D		m_MidPoint;

	INT32		m_nGeoCount;
	INT32		m_nDivideCount;
	INT32		m_nMaxDepth;
	INT32		m_nDepthCountForPlane;

protected:
	void		_RemoveBSPGeometry( i3BSPGeometry * pBSPGeo );
	void		_RemoveAllBSPGeometry( void );

	void		_CreateRoot( void );
	
	INT32		_FindGeometryByAddress( void * pAddress );

	void		_CalcMaxVec( VEC3D * pVecMax, VEC3D * pSrcVec, BOOL bInit );
	void		_CalcMinVec( VEC3D * pVecMin, VEC3D * pSrcVec, BOOL	bInit );

	BOOL		_CalcMaxVec( VEC3D * pVecMax, VEC3D * pSrcVec, UINT32 nStyle );
	BOOL		_CalcMinVec( VEC3D * pVecMin, VEC3D * pSrcVec, UINT32 nStyle );
	
	void		_BuildTriList( i3BSPNode * pNode, i3Geometry * pGeometry );
	void		_SortTriList( i3BSPNode * pNode, VEC4D * pPlane, MATRIX * pWorldMat );

	void		_BuildTriObject( i3BSPGeometry * pOutBSPGeo, i3Geometry * pGeo );

	// 수정 필요
	BOOL		_BuildConvexPoly( UINT32 gidx, i3VertexArray * pVA, i3BSPNode * pNode );

	void		_CalcSpaceBound( i3BSPNode * pRoot );
	void		_CalcPlane( VEC4D * pOutPlane, i3BSPNode * pParent, UINT32 nType );

	void		_SetGeometryName( i3Geometry * pOut, i3Geometry * pSrcGeo, INT32 nSide );
	//
	void		_FindNewVertex( VEC3D * pOut, VEC3D * pDef, VEC3D * pDest, REAL32 d, REAL32 f, REAL32 w);
	void		_AddBSPVertex( i3List * pVertexList, VEC3D * pPos, i3BSPTriIndex * pTri, INT32 idx );
	void		_AddBSPTri( i3List * pOutList, UINT16 idx1, UINT16 idx2, UINT16 idx3 );
	INT32		_DivideTri( i3List * pOutList, i3BSPTriIndex * pTri, i3List * pNewVertexList );

	// 필요 없는 면을 줄입니다.
	BOOL		_CheckRemoveOfVertex( i3List * pOutList, i3List * pList, i3VertexArray * pVA, VEC3D * pVec, VEC3D * pVec2, INT32 idx );
	void		_RemoveOfVertex( i3List * pList, i3VertexArray * pVA );
	void		_SummaryForCollision( i3Geometry * pGeometry );
	

	i3BSPGeometry * _CreateGeometryByTriIndex( i3BSPNode * pOutLeft, i3BSPNode * pOutRight,
		VEC4D * pPlane, i3BSPGeometry * pBSPGeo );
	
	void		_CreateGeometry( i3Geometry * pOutGeo, i3BSPNode * pNode, i3Geometry * pSrcGeometry, INT32 nFlag = 0 );

	i3BoundBox *_CreateBoundBoxForWorld( i3Geometry * pGeometry, MATRIX * pWorldMat );

	INT32		_getBSPGeometryCount( void )				{ return m_BSPGeometryList.GetCount(); }
	i3BSPGeometry * _getBSPGeometry( INT32 idx )			{ return (i3BSPGeometry*) m_BSPGeometryList.Items[ idx ]; }
	void		_setBSPGeometry( INT32 idx, i3BSPGeometry * pGeo )	{ m_BSPGeometryList.SetItem( idx, (void*) pGeo ); }
	void		_addBSPGeometry( i3BSPGeometry * pGeo )		{ m_BSPGeometryList.Add( (void*) pGeo ); }

public:
	UINT32		getStyle( void )							{ return m_nStyle; }
	void		setStyle( UINT32 nStyle )					{ m_nStyle = nStyle; }

	i3BSPNode * getNode( UINT32 idx )						{ return (i3BSPNode *) m_BSPList.Items[ idx ]; }
	void		setNode( UINT32 idx, i3BSPNode * pNode )	{ m_BSPList.Items[ idx ] = (void*) pNode; }
	void		addNode( i3BSPNode * pNode )				{ m_BSPList.Add( pNode ); }
	INT32		getNodeCount( void )						{ return m_BSPList.GetCount(); }
	
public:
	i3OptBuildBSP(void);
	virtual ~i3OptBuildBSP(void);

	// BSP 노드를 생성한다(재귀호출함수)
	void		CreateNode( i3BSPNode * pParent );
	void		CreateNode( I3G_PRIMITIVE prim, UINT32 gidx, i3VertexArray * pVA, i3BSPNode * pParent );

	// 컬리젼 체크
	BOOL		CheckBSP();

	// 노드 재정렬
	void		ReBuildNode( i3Node * pNode, i3BSPNode * pBSPNode );


	virtual BOOL OnNode( i3Node * pNode);
	virtual void Trace( i3Node * pRoot);
};

#endif

