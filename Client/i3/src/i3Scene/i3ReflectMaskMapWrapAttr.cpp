#include "i3SceneDef.h"
#include "i3ReflectMaskMapWrapAttr.h"

I3_CLASS_INSTANCE( i3ReflectMaskMapWrapAttr);

i3ReflectMaskMapWrapAttr::i3ReflectMaskMapWrapAttr(void)
{
	SetID( I3_ATTRID_REFLECTMASKMAP_WRAP);
	m_Bind = I3G_TEXTURE_BIND_REFLECT_MASK;
}


