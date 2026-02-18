#if !defined( I3OPT_GEO_NONINDEXED_TO_INDEXED_H)
#define I3OPT_GEO_NONINDEXED_TO_INDEXED_H

#include "i3SceneOptimizer.h"
#include "i3OptGeometryBase.h"

class I3_EXPORT_OPT i3OptGeo_NonIndexed_to_Indexed : public i3OptGeometryBase
{
	I3_EXPORT_CLASS_DEFINE( i3OptGeo_NonIndexed_to_Indexed, i3OptGeometryBase);
		
protected:
	void			Convert( VAINFO * pInfo);

	void			MakeIndexedGeo( VAINFO * pInfo, i3GeometryAttr * pGeoAttr, INT32 nAICount);

public:
	i3OptGeo_NonIndexed_to_Indexed(void);

public:
	virtual void	Trace( i3Node * pRoot) override;
};

#endif