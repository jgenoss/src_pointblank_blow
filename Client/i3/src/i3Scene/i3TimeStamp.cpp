#include "i3SceneDef.h"
#include "i3TimeStamp.h"

I3_CLASS_INSTANCE( i3TimeStamp);

i3TimeStamp::~i3TimeStamp(void)
{
	I3_SAFE_RELEASE( m_pRefObject);
}
