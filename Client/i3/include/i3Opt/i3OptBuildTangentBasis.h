#if !defined( __I3_OPT_BUILD_TANGENT_BASIS_H)
#define __I3_OPT_BUILD_TANGENT_BASIS_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptBuildTangentBasis : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildTangentBasis, i3SceneOptimizer);
protected:
	i3::vector<i3GeometryAttr*>	m_GeoAttrList;

	void			_BuildIndexedTriList( i3GeometryAttr * pGeoAttr);
	void			_BuildIndexedTriStrip( i3GeometryAttr * pGeoAttr);

public:
	i3OptBuildTangentBasis(void);

	void			Build( i3GeometryAttr * pGeoAttr);

	virtual bool	OnNode( i3Node * pRoot, i3Node * pParent) override;
	virtual void Trace( i3Node * pRoot) override;
};

#endif
