#if !defined( __I3_GEOMETRY_H)
#define __I3_GEOMETRY_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3GeometryAttr.h"
#include "i3RenderAttrListAttr.h"

namespace i3 { class occ_mesh_set; }

class I3_EXPORT_SCENE i3Geometry : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Geometry, i3Node);

protected:
	i3::vector<i3RenderAttr*>	m_List;

protected:
	virtual void _GetWrappingBound( I3_GETBOUND_INFO * pInfo) override;
	virtual void	PreTrace(i3SceneTracer * pTracer) override;
	virtual void	CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled) override;

public:
	i3Geometry(void);
	virtual ~i3Geometry(void);

	void		AppendGeometryAttr( i3RenderAttr * pGeoAttr);
	void		RemoveGeometryAttr( i3RenderAttr * pGeoAttr);
	void		RemoveAllGeometryAttr(void);
	INT32		GetGeometryAttrCount(void)							{ return (INT32)m_List.size(); }
	i3RenderAttr * GetGeometryAttr( INT32 idx)					{ return m_List[idx]; }

	UINT32		GetTotalVertexCount(void);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	OnLostDevice( bool bLostDevice) override;
	virtual void	OnRevive( i3RenderContext * pCtx) override;
};

#endif
