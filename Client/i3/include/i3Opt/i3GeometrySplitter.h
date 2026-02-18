#if !defined( __I3_GEOMETRY_SPLITTER_H)
#define __I3_GEOMETRY_SPLITTER_H

class I3_EXPORT_OPT i3GeometrySplitter : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GeometrySplitter, i3ElementBase);

protected:
	i3::vector<VEC4D*>	m_PlaneList;

	i3VertexArray *		m_pPoolVA = nullptr;
	INT32				m_PoolCount = 0;

	i3VertexArray *		m_pIncVA = nullptr;
	INT32				m_IncCount = 0;

	i3VertexArray *		m_pExcVA = nullptr;
	INT32				m_ExcCount = 0;

	i3VertexArray *		m_pSrcVA = nullptr;

	MATRIX				m_Matrix;

	REAL32				m_EPS = 0.009f;

protected:
	INT32			getPoolCount(void)				{ return m_PoolCount; }
	void			getPool( INT32 idx, VEC3D * p);
	INT32			addPool( bool bAdd = true);
	void			setPoolVertex( INT32 idxDest, i3VertexArray * pVA, INT32 idx0, INT32 idx1 = -1, REAL32 t = 0.0f);

	INT32			addInc(void);
	void			setIncVertex( INT32 idxDest, i3VertexArray * pVA, INT32 idx0, INT32 idx1 = -1, REAL32 t = 0.0f);

	INT32			addExc(void);
	void			setExcVertex( INT32 idxDest, i3VertexArray * pVA, INT32 idx0, INT32 idx1 = -1, REAL32 t = 0.0f);

	INT32			Split( INT32 idx0, INT32 idx1, INT32 idx2);

#if defined( I3_DEBUG)
	void			vlogPool( INT32 idx, UINT8 r, UINT8 g, UINT8 b);
	void			vlogInc( INT32 idx, UINT8 r, UINT8 g, UINT8 b);
	void			vlogExc( INT32 idx, UINT8 r, UINT8 g, UINT8 b);
#endif

public:
	virtual ~i3GeometrySplitter(void);

	INT32			getIncCount(void)				{ return m_IncCount; }
	i3VertexArray *	getIncBuffer(void)				{ return m_pIncVA; }
	
	INT32			getExcCount(void)				{ return m_ExcCount; }
	i3VertexArray *	getExcBuffer(void)				{ return m_pExcVA; }

	INT32			getPlaneCount(void)				{ return INT32(m_PlaneList.size()); }
	VEC4D *			getPlane( INT32 idx)			{ return m_PlaneList[idx]; }
	void			addPlane( VEC4D * pPlane);
	void			addPlane( i3BoundBox * pBox);
	void			removeAllPlane(void);

	MATRIX *		getMatrix(void)					{ return &m_Matrix; }

	INT32			Split( i3VertexArray * pVA, MATRIX * pMtx, VEC4D * pPlane);
};

#endif
