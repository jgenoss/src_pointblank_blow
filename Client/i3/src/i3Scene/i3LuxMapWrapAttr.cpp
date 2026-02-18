#include "i3SceneDef.h"
#include "i3LuxMapWrapAttr.h"

I3_CLASS_INSTANCE( i3LuxMapWrapAttr);

i3LuxMapWrapAttr::i3LuxMapWrapAttr(void)
{
	SetID( I3_ATTRID_LUXMAP_WRAP);
	m_Bind = I3G_TEXTURE_BIND_LUX;
}


