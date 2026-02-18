#if !defined( __I3_OPT_BUILD_TANGENT_BASIS_H)
#define __I3_OPT_BUILD_TANGENT_BASIS_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptBuildTangentBasis : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBuildTangentBasis);
protected:
	i3List			m_GeoAttrList;

	void			_BuildIndexedTriList( i3GeometryAttr * pGeoAttr);
	void			_BuildIndexedTriStrip( i3GeometryAttr * pGeoAttr);

public:
	i3OptBuildTangentBasis(void);
	virtual ~i3OptBuildTangentBasis(void);

	void			Build( i3GeometryAttr * pGeoAttr);

	virtual BOOL	OnNode( i3Node * pRoot);
	virtual void Trace( i3Node * pRoot);
};

#endif
