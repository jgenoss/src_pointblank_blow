#include "i3SceneDef.h"
#include "i3SpecularMapWrapAttr.h"

I3_CLASS_INSTANCE( i3SpecularMapWrapAttr);

i3SpecularMapWrapAttr::i3SpecularMapWrapAttr(void)
{
	SetID( I3_ATTRID_SPECULARMAP_WRAP);

	m_Bind = I3G_TEXTURE_BIND_SPECULAR;
}

