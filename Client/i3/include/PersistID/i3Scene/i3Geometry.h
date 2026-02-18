#if !defined( __I3_GEOMETRY_H)
#define __I3_GEOMETRY_H

#include "i3Base.h"
#include "i3Node.h"
#include "i3GeometryAttr.h"
#include "i3RenderAttrListAttr.h"

class I3_EXPORT_SCENE i3Geometry : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3Geometry);

protected:
	i3List	m_List;

protected:
	virtual void _GetWrappingBound( I3_GETBOUND_INFO * pInfo);

public:
	i3Geometry(void);
	virtual ~i3Geometry(void);

	void		AppendGeometryAttr( i3RenderAttr * pGeoAttr);
	void		RemoveGeometryAttr( i3RenderAttr * pGeoAttr);
	void		RemoveAllGeometryAttr(void);
	INT32		GetGeometryAttrCount(void)							{ return m_List.GetCount(); }
	i3RenderAttr * GetGeometryAttr( INT32 idx)					{ return (i3RenderAttr *) m_List.Items[idx]; }

	UINT32		GetTotalVertexCount(void);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
