#if !defined( __I3_SHAPE_H)
#define __I3_SHAPE_H

#include "i3GeometryAttr.h"

class I3_EXPORT_SCENE i3Shape : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3Shape, i3PersistantElement);

protected:
	i3::vector<i3GeometryAttr*>			m_GeoList;

public:
	virtual ~i3Shape(void);

	void				AddGeometryAttr( i3GeometryAttr * pAttr);
	void				RemoveGeometryAttr( i3GeometryAttr * pAttr);
	void				RemoveAllGeometryAttr(void);
	INT32				getGeometryAttrCount(void)						{ return (INT32)m_GeoList.size(); }
	i3GeometryAttr *	getGeometryAttr( INT32 idx)						{ return m_GeoList[idx]; }

	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif
