#if !defined( __I3_SG_PERSISTANT_OBJECT_H)
#define __I3_SG_PERSISTANT_OBJECT_H

#include "i3Base.h"

class i3SgPersistantObject : public i3PersistantElement
{
	I3_ABSTRACT_CLASS_DEFINE( i3SgPersistantObject);

protected:

public:
	i3SgPersistantObject(void);
	virtual ~i3SgPersistantObject(void);

	virtual void OnBuildObjectList( i3List * pList);
};

#endif
