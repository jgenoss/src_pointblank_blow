#if !defined( I3OPT_REMOVE_ANIM_H)
#define I3OPT_REMOVE_ANIM_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptRemoveAnim : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptRemoveAnim);

protected:
	INT32	m_Count;

public:
	i3OptRemoveAnim(void);
	virtual ~i3OptRemoveAnim(void);

	virtual BOOL	OnNode( i3Node * pNode);
};

#endif