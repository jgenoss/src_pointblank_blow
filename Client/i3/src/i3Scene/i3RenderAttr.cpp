#include "i3SceneDef.h"
#include "i3RenderAttr.h"

I3_CLASS_INSTANCE( i3RenderAttr);

void i3RenderAttr::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	List.push_back( this);
}

#if defined( I3_DEBUG)
void i3RenderAttr::Dump(void)
{
	I3TRACE( "%s\n", meta()->class_name());
}
#endif
