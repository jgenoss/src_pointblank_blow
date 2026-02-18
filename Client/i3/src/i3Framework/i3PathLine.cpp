#include "i3FrameworkPCH.h"
#include "i3PathLine.h"

I3_CLASS_INSTANCE( i3PathLine);

i3PathLine::~i3PathLine(void)
{
	I3_SAFE_RELEASE( m_pStart);
	I3_SAFE_RELEASE( m_pEnd);
}

void i3PathLine::SetTo( i3::pack::PATH_LINK_INFO * pInfo)
{
	pInfo->m_Cost = m_Cost;
	pInfo->m_Style = m_PathStyle;
}
