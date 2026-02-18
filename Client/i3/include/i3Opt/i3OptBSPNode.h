#if !defined( __I3_OPT_BSP_NODE_H)
#define __I3_OPT_BSP_NODE_H

class i3OptBSPGeometry;

struct I3OPT_BSP_TRI
{
	VEC3D	m_Point[3];
	VEC4D	m_Plane;
	INT32	m_GeometryIndex = 0;
	INT32	m_GeometryNodeIndex = 0;
	INT32	m_TriIndex = 0;
} ;

struct DIVIDE_RESULT_INFO
{
	VEC3D		m_Point[3];
	VEC4D		m_Plane;
	INT32		m_SplitCount = 0x7FFFFFFF;
	REAL32		m_BestRelation = 1.0f;

	/*void		reset(void)
	{
		m_SplitCount = 0x7FFFFFFF;
		m_BestRelation = 1.0f;
	}*/
} ;




struct I3OPT_BSP_CONTEXT;
// 익스포트추가->다시 제거. (12.09.19.수빈)

class i3OptBSPNode : public i3ElementBase
{
	I3_CLASS_DEFINE( i3OptBSPNode, i3ElementBase);
protected:
	VEC4D				m_Plane;
	i3::vector<i3OptBSPGeometry*> m_GeoList;

	i3OptBSPNode *		m_pParent = nullptr;
	i3OptBSPNode *		m_pFront = nullptr;
	i3OptBSPNode *		m_pBack = nullptr;

	I3OPT_BSP_TRI *		m_pPoly = nullptr;
	INT32				m_PolyCount = 0;

protected:
	bool			_isConvexGeometry(void);
	bool			_FindBestHyperPlane( DIVIDE_RESULT_INFO * pInfo);
	bool			_FindHalfPlane( DIVIDE_RESULT_INFO * pInfo);
	bool			_FindBestPlaneInGeo( DIVIDE_RESULT_INFO * pInfo);
	void			_SplitGeometry(void);
	bool			_isValidPlane( VEC4D * pPlane);
	void			_getBound( VEC3D * pMin, VEC3D * pMax);

public:
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

	INT32				getGeometryCount(void)							{ return INT32(m_GeoList.size()); }
	i3OptBSPGeometry *	getGeometry( INT32 idx)							{ return m_GeoList[idx]; }
	void				addGeometry( i3OptBSPGeometry * pGeo);
	void				addGeometries( i3::vector<i3OptBSPGeometry*>& List);
	void				removeGeometry( i3OptBSPGeometry * pGeo);
	void				removeAllGeometry(void);

	void				BuildBSPTri(void);
	void				BuildBSP( I3OPT_BSP_CONTEXT * pCtx);

	void				VLog(void);
	void				Dump( INT32 depth = 0);

	i3BinaryPartitionNode *		BuildSg(void);
};

#endif
