#include "i3FrameworkPCH.h"
#include "i3PathPoint.h"


I3_CLASS_INSTANCE( i3PathPoint);

void i3PathPoint::SetTo(i3::pack::PATH_POINT_INFO * pInfo)
{
	pInfo->m_Priority		= m_Priority;
	
	i3Vector::Copy( &pInfo->m_Pos, &m_Pos);

	i3Color::Set( &pInfo->m_Color, &m_Color);
}
