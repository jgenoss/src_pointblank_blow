// i3CollideeGroup.cpp
#include "i3SceneDef.h"
#include "i3CollideeGroup.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3CollideeGroup);

bool i3CollideeGroup::Add(void* pCollidee)
{
	m_List.push_back(pCollidee);
	return true;
}

bool i3CollideeGroup::Remove(void* pCollidee)
{
	i3::vu::remove(m_List, pCollidee);
	return true;
}

bool i3CollideeGroup::Clear(void)
{
	m_List.clear();
	return true;
}
