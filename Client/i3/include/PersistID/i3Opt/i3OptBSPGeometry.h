#if !defined( __I3_OPY_BSP_GEOMETRY_H)
#define __I3_OPY_BSP_GEOMETRY_H

class i3OptBSPGeometry : public i3ElementBase
{
	I3_CLASS_DEFINE( i3OptBSPGeometry);
protected:
	MATRIX			m_Trans;
	i3List			m_AttrList;
	i3Geometry *	m_pGeo;
	bool			m_bTransparent;

protected:
	i3OptBSPGeometry *	_CreateNewGeometry( void);
	void				_SplitTriangle( VEC4D * pPlane, i3VertexArray * pOrgVA, INT32 idxOrg, i3VertexArray * pFrontVA, INT32 * pidxFront, i3VertexArray * pBackVA, INT32 * pidxBack);

public:
	i3OptBSPGeometry(void);
	virtual ~i3OptBSPGeometry(void);

	MATRIX *		getMatrix(void)						{ return &m_Trans; }
	void			setMatrix( MATRIX * pMat)			{ i3Matrix::Copy( &m_Trans, pMat); }

	i3List *		getAttrList(void)					{ return &m_AttrList; }
	INT32			getAttrCount(void)					{ return m_AttrList.GetCount(); }
	i3RenderAttr *	getAttr( INT32 idx)					{ return (i3RenderAttr *) m_AttrList.Items[ idx]; }
	void			removeAllAttrs(void);

	i3Geometry *	getGeometry(void)					{ return m_pGeo; }
	void			setGeometry( i3Geometry * pGeo);
	
	bool			isTransparent(void)					{ return m_bTransparent; }
	void			setTransparent( bool bFlag)			{ m_bTransparent = bFlag; }

	INT32			GetTriangleCount(void);

	// 주어진 Plane으로 자신을 분할한다.
	// 반환되는 결과값에 따라 pFront 및 pBack의 값은 아래의 의미를 가진다.
	//
	// ---------------------+----------------+---------------
	//      side            |     pFront     |   pBack
	// ---------------------+----------------+---------------
	// I3_FACE_FRONT        |      this      |    NULL
	// I3_FACE_BACK         |      NULL      |    this
	// I3_FACE_COINCIDENT   |      NULL      |    NULL
	// I3_FACE_SPAN         |  new geometry  | new geometry
	// ---------------------+----------------+---------------
	//
	I3_FACE_SIDE	Split( VEC4D * pPlane, i3OptBSPGeometry ** pFront, i3OptBSPGeometry ** pBack);

	i3Node *		BuildSg(void);

	void			VLog(void);
};

#endif
