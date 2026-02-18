#if !defined( __I3OPT_SETUP_CENTER_MESS_H__)
#define __I3OPT_SETUP_CENTER_MESS_H__


#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptSetupCenterMass : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptSetupCenterMass, i3SceneOptimizer);

public:
	i3OptSetupCenterMass();
	virtual bool	OnNode( i3Node * pNode, i3Node * pParent) override;
};

#endif