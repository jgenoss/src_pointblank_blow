#if !defined( __I3_OPT_GEOMETRY_BASE_H)
#define __I3_OPT_GEOMETRY_BASE_H

#include "i3SceneOptimizer.h"

struct VAINFO
{
	MATRIX			m_World;
	i3VertexArray *	m_pOldVA = nullptr;
	i3VertexArray * m_pNewVA = nullptr;
	INT32 *			m_pIndexTable = nullptr;
	INT32			m_Param1 = 0;
	INT32			m_Param2 = 0;

	VAINFO(void)
	{
		m_pOldVA = m_pNewVA = nullptr;
		m_Param1 = m_Param2 = 0;
		m_pIndexTable = nullptr;
	}

	void	Destroy(void)
	{
		I3MEM_SAFE_FREE_POINTER(m_pIndexTable);
		I3_SAFE_RELEASE( m_pNewVA);
	}
};

class I3_EXPORT_OPT i3OptGeometryBase : public i3SceneOptimizer
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3OptGeometryBase, i3SceneOptimizer);
protected:
	i3::vector<VAINFO*>	m_VAList;
	i3::vector<i3GeometryAttr*>	m_GeoList;

public:
	virtual ~i3OptGeometryBase(void);

	INT32				AddVA( i3VertexArray * pVA, MATRIX * pMatrix);
	INT32				FindVA( i3VertexArray * pVA);
	INT32				getVACount(void)					{ return INT32(m_VAList.size()); }
	VAINFO *			getVA( INT32 idx)					{ return m_VAList[idx]; }
	void				RemoveAllVA(void);

	INT32				getGeoAttrCount(void)				{ return INT32(m_GeoList.size()); }
	i3GeometryAttr *	getGeoAttr( INT32 idx)				{ return m_GeoList[idx]; }
	void				RemoveAllGeo(void)					{ m_GeoList.clear(); }

	void				GetWorldMatrix( i3Node * pNode, MATRIX * pMatrix);

	void				GatherAllVertexArray( i3Node * pRoot);
	void				ReplaceAllVertexArrayFromGeoAttr(void);
};

#endif
