#if !defined( __I3_OCCLUSION_QUERY_H)
#define __I3_OCCLUSION_QUERY_H

#include "i3Node.h"
#include "i3OcclusionQueryAttr.h"

class I3_EXPORT_SCENE i3OcclusionQuery : public i3Node
{
	I3SG_NODE_CLASS_DEFINE( i3OcclusionQuery, i3Node);

protected:
	i3OcclusionQueryAttr *		m_pAttr;	// [initialize at constructor]

public:
	i3OcclusionQuery(void);
	virtual ~i3OcclusionQuery(void);

	bool					isReady(void)					{ return m_pAttr->isReady(); }
	INT32					getResult(void)					{ return m_pAttr->getResult(); }
};

#endif
