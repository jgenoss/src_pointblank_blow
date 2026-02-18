#if !defined( __I3_OPT_SKIN_GEOMETRY_H)
#define __I3_OPT_SKIN_GEOMETRY_H

#include "i3OptGeometryBase.h"

class I3_EXPORT_OPT i3OptSkinGeometry : public i3OptGeometryBase
{
	I3_EXPORT_CLASS_DEFINE( i3OptSkinGeometry, i3OptGeometryBase);

protected:

protected:
	i3SkinGeometryAttr *	_BuildSkin( i3GeometryAttr * pGeoAttr);

public:
	i3OptSkinGeometry(void);

	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void	Trace( i3Node * pRoot) override;

	virtual const char * getDescName(void) override;
	virtual	bool		SaveToXML( i3XMLElement * pXML) override;
	virtual	bool		LoadFromXML( i3XMLElement * pXML) override;
};

#endif
