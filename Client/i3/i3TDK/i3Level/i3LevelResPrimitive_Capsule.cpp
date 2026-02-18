#include "stdafx.h"
#include "i3LevelPrimitive_Capsule.h"
#include "i3LevelResPrimitive_Capsule.h"

I3_CLASS_INSTANCE(i3LevelResPrimitive_Capsule);

i3LevelResPrimitive_Capsule::i3LevelResPrimitive_Capsule()
{
	m_nPrimType = I3LV_RES_PRIM_CAPSULE;

	setPath( "Capsule");
}

i3ClassMeta * i3LevelResPrimitive_Capsule::GetInstanceMeta()
{
	return i3LevelPrimitive_Capsule::static_meta();
}

i3Node * i3LevelResPrimitive_Capsule::_BuildScene()
{
	return i3LevelPrimitive_Capsule::BuildSg();
}

