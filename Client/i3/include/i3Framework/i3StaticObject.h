#if !defined( __I3_STATICOBJECT_H)
#define __I3_STATICOBJECT_H

#include "i3GameObj.h"

class I3_EXPORT_FRAMEWORK i3StaticObject : public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE(i3StaticObject, i3GameObj);

public:
	i3StaticObject();
	virtual ~i3StaticObject();

	virtual void		Create(i3Framework * pFramework, i3GameResSceneGraph * pRes);
};

#endif
