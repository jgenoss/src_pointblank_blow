#if !defined( __USERMAP_BLOCK_GEOMETRY_H)
#define __USERMAP_BLOCK_GEOMETRY_H

typedef struct _tagUserMapBlockVtx
{
	VEC3D		m_Pos;
	VEC2D		m_Tex;
	VEC3D		m_Normal;
	VEC3D		m_Tangent;
	VEC3D		m_Binormal;
} BLOCKVTX;

class CUserMap_Block_Geometry : public i3ElementBase
{
	I3_CLASS_DEFINE( CUserMap_Block_Geometry);

protected:
	BLOCKVTX *			m_pVtx;
	INT32				m_Count;
	i3GeometryAttr *	m_pSrcGeo;

public:
	CUserMap_Block_Geometry(void);
	virtual ~CUserMap_Block_Geometry(void);

	void			Create( i3GeometryAttr * pGeo);

	INT32			getVtxCount(void)				{ return m_Count; }
	BLOCKVTX *		getVtx( INT32 idx)
	{
		I3_BOUNDCHK( idx, m_Count);

		return &m_pVtx[idx];
	}

	i3GeometryAttr *	getGeometry(void)			{ return m_pSrcGeo; }
};

#endif
