#if !defined( __I3_GIC_OBJECT_H)
#define __I3_GIC_OBJECT_H

#include "i3GICAll.h"

#if defined( I3_DEBUG)

class I3_EXPORT_GIC i3GICObject : public i3TreeNode
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE(i3GICObject, i3TreeNode);
protected:

public:
	i3GICObject(void) {}
	virtual ~i3GICObject(void) {}
};

#endif	// I3_DEBUG

#endif
