#include "i3SceneDef.h"
#include "i3NormalMapWrapAttr.h"

I3_CLASS_INSTANCE( i3NormalMapWrapAttr);

i3NormalMapWrapAttr::i3NormalMapWrapAttr(void)
{
	SetID( I3_ATTRID_NORMALMAP_WRAP);
	m_Bind = I3G_TEXTURE_BIND_NORMAL;
}


