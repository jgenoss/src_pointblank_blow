#include "i3FrameworkPCH.h"
#include "i3StaticObject.h"

I3_CLASS_INSTANCE(i3StaticObject);

i3StaticObject::i3StaticObject()
{
}

i3StaticObject::~i3StaticObject()
{
}

void i3StaticObject::Create(i3Framework * pFramework, i3GameResSceneGraph * pRes)
{
	i3GameObj::BindSgRes( pFramework, (i3GameResSceneGraph *) pRes);
}
