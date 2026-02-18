#if !defined( __USERMAP_BLOCK_H)
#define __USERMAP_BLOCK_H

#include "UserMap_Block_Geometry.h"

class CUserMap_Block : public i3ElementBase
{
	I3_CLASS_DEFINE( CUserMap_Block);
protected:
	i3List			m_GeoList;
	INT32			m_idxApply;

protected:
	void			_Rec_Create( i3Node * pNode);
	void			_Rec_Resize( i3Node * pNode, VEC3D * pScale);
	void			_ResizeGeometry( i3GeometryAttr * pGeoAttr, VEC3D * pScale, VEC3D * pVMin, VEC3D * pVMax);

public:
	CUserMap_Block(void);
	virtual ~CUserMap_Block(void);

	void			Create( i3Node * pNode);
	void			Resize( i3Node * pNode, VEC3D * pScale);
	void			RemoveAll(void);

	INT32						getBlockGeometryCount(void)			{ return m_GeoList.GetCount(); }
	CUserMap_Block_Geometry *	getBlockGeometry( INT32 idx)		{ return (CUserMap_Block_Geometry *) m_GeoList.Items[idx]; }
};

#endif
