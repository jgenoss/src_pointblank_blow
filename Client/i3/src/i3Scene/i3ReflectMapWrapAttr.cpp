#include "i3SceneDef.h"
#include "i3ReflectMapWrapAttr.h"

I3_CLASS_INSTANCE( i3ReflectMapWrapAttr);

i3ReflectMapWrapAttr::i3ReflectMapWrapAttr(void)
{
	SetID( I3_ATTRID_REFLECTMAP_WRAP);
	m_Bind = I3G_TEXTURE_BIND_REFLECT;
}

