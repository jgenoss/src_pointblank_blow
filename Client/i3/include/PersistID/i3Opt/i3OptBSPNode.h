#if !defined( __I3_OPT_BSP_NODE_H)
#define __I3_OPT_BSP_NODE_H

#include "i3OptBSPGeometry.h"

typedef struct i3OptBSPTri
{
	VEC3D	m_Point[3];
	VEC4D	m_Plane;
	INT32	m_GeometryIndex;
	INT32	m_GeometryNodeIndex;
	INT32	m_TriIndex;
} I3OPT_BSP_TRI;

typedef struct _DivideResultInfo
{
	VEC3D		m_Point[3];
	VEC4D		m_Plane;
	INT32		m_SplitCount;
	REAL32		m_BestRelation;

	void		reset(void)
	{
		m_SplitCount = 0x7FFFFFFF;
		m_BestRelation = 1.0f;
	}
} DIVIDE_RESULT_INFO;

#define SPLIT_COND_CONVEX			0x00000001
#define	SPLIT_COND_DEPTH			0x00000002
#define	SPLIT_COND_POLYCOUNT		0x00000004

enum SPLIT_PLANE
{
	SPLIT_PLANE_XAXIS,
	SPLIT_PLANE_YAXIS,
	SPLIT_PLANE_ZAXIS
};

enum SPLIT_MODE
{
	SPLIT_MODE_HYPERPLANE,				// Geometry 중의 한 Polygon을 분할 Plane으로 선정하는 방식
	SPLIT_MODE_HALFPLANE,				// Geometry Bound Volume을 양분하는 Plane
};

typedef struct _tagSplitContext
{
	INT32			m_Depth;
	INT32			m_LimitDepth;
	INT32			m_LimitCount;
	UINT32			m_Condition;
	SPLIT_PLANE		m_Plane;
	SPLIT_MODE		m_Mode;
	bool			m_bSplitPoly;
} I3OPT_BSP_CONTEXT;

class i3OptBSPNode : public i3ElementBase
{
	I3_CLASS_DEFINE( i3OptBSPNode);
protected:
	VEC4D				m_Plane;
	i3List				m_GeoList;

	i3OptBSPNode *		m_pParent;
	i3OptBSPNode *		m_pFront;
	i3OptBSPNode *		m_pBack;

	I3OPT_BSP_TRI *		m_pPoly;
	INT32				m_PolyCount;

protected:
	bool			_isConvexGeometry(void);
	bool			_FindBestHyperPlane( DIVIDE_RESULT_INFO * pInfo);
	bool			_FindHalfPlane( DIVIDE_RESULT_INFO * pInfo);
	bool			_FindBestPlaneInGeo( DIVIDE_RESULT_INFO * pInfo);
	void			_SplitGeometry(void);
	bool			_isValidPlane( VEC4D * pPlane);
	void			_getBound( VEC3D * pMin, VEC3D * pMax);

public:
	i3OptBSPNode(void);
	virtual ~i3OptBSPNode(void);

	VEC4D *				getPlane(void)					{ return &m_Plane; }
	void				setPlane( VEC4D * pPlane)		{ i3Vector::Copy( &m_Plane, pPlane); }

	i3OptBSPNode *		getParent(void)					{ return m_pParent; }
	void				setParent( i3OptBSPNode * pParent)
	{
		m_pParent = pParent;
	}

	i3OptBSPNode *		getFrontNode(void)				{ return m_pFront; }
	void				setFrontNode( i3OptBSPNode * pNode)
	{
		I3_REF_CHANGE( m_pFront, pNode);
	}

	i3OptBSPNode *		getBackNode(void)				{ return m_pBack; }
	void				setBackNode( i3OptBSPNode * pNode)
	{
		I3_REF_CHANGE( m_pBack, pNode);
	}

	INT32				getGeometryCount(void)							{ return m_GeoList.GetCount(); }
	i3OptBSPGeometry *	getGeometry( INT32 idx)							{ return (i3OptBSPGeometry *) m_GeoList.Items[idx]; }
	void				addGeometry( i3OptBSPGeometry * pGeo);
	void				addGeometries( i3List * pList);
	void				removeGeometry( i3OptBSPGeometry * pGeo);
	void				removeAllGeometry(void);

	void				BuildBSPTri(void);
	void				BuildBSP( I3OPT_BSP_CONTEXT * pCtx);

	void				VLog(void);
	void				Dump( INT32 depth = 0);

	i3BinaryPartitionNode *		BuildSg(void);
};

#endif
