#if !defined( __I3OPT_REMOVE_DUPLICATED_VA_H__)
#define __I3OPT_REMOVE_DUPLICATED_VA_H__

#include "i3OptCompactMesh.h"

class I3_EXPORT_OPT i3OptRemoveDuplicatedVA : public i3OptCompactMesh
{
	I3_EXPORT_CLASS_DEFINE( i3OptRemoveDuplicatedVA, i3OptCompactMesh);
	
public:
	virtual void	Trace( i3Node * pRoot) override;
};

#endif
