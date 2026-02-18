#if !defined( I3OPT_REMOVE_ANIM_H)
#define I3OPT_REMOVE_ANIM_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveAnim : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptRemoveAnim, i3SceneOptimizer);

protected:
	INT32	m_Count = 0;

public:
	i3OptRemoveAnim(void);
	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif