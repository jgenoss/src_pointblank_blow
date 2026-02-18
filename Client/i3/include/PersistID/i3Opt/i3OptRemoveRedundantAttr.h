#if !defined( __I3_OPT_REMOVE_REDUNDANT_ATTRS_H)
#define __I3_OPT_REMOVE_REDUNDANT_ATTRS_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveRedundantAttr : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptRemoveRedundantAttr);
protected:
	i3AttrStackManager		m_AttrStack;

protected:

public:
	i3OptRemoveRedundantAttr(void);
	virtual ~i3OptRemoveRedundantAttr(void);

	virtual BOOL OnPreNode( i3Node * pNode);
	virtual BOOL OnPostNode( i3Node * pNode);
};

#endif
