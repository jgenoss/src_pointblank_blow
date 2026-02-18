#if !defined( __I3_OPT_REMOVE_REDUNDANT_ATTRS_H)
#define __I3_OPT_REMOVE_REDUNDANT_ATTRS_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveRedundantAttr : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptRemoveRedundantAttr, i3SceneOptimizer);
protected:
	i3AttrStackManager		m_AttrStack;

public:
	i3OptRemoveRedundantAttr(void);

	virtual bool OnPreNode( i3Node * pNode, i3Node * pParent) override;
	virtual bool OnPostNode( i3Node * pNode, i3Node * pParent) override;
};

#endif
