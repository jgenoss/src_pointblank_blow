#if !defined( __I3_SHAPE_H)
#define __I3_SHAPE_H

#include "i3GeometryAttr.h"

class I3_EXPORT_SCENE i3Shape : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3Shape);

protected:
	i3List			m_GeoList;

public:
	i3Shape(void);
	virtual ~i3Shape(void);

	void				AddGeometryAttr( i3GeometryAttr * pAttr);
	void				RemoveGeometryAttr( i3GeometryAttr * pAttr);
	void				RemoveAllGeometryAttr(void);
	INT32				getGeometryAttrCount(void)						{ return m_GeoList.GetCount(); }
	i3GeometryAttr *	getGeometryAttr( INT32 idx)						{ return (i3GeometryAttr *) m_GeoList.Items[idx]; }

	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
