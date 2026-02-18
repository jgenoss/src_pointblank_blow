#include "stdafx.h"
#include "i3LevelResPrimitive_Box.h"
#include "i3LevelPrimitive_Box.h"

I3_CLASS_INSTANCE( i3LevelResPrimitive_Box);

i3LevelResPrimitive_Box::i3LevelResPrimitive_Box()
{
	m_nPrimType	= I3LV_RES_PRIM_BOX;
}

i3ClassMeta *	i3LevelResPrimitive_Box::GetInstanceMeta(void)
{
	return i3LevelPrimitive_Box::static_meta();
}

i3Node	*	i3LevelResPrimitive_Box::_BuildScene()
{
	return i3LevelPrimitive_Box::BuildSg();
}

