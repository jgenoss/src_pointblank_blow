#include "stdafx.h"
#include "i3LevelResPrimitive_Sphere.h"
#include "i3LevelPrimitive_Sphere.h"

I3_CLASS_INSTANCE( i3LevelResPrimitive_Sphere);

i3LevelResPrimitive_Sphere::i3LevelResPrimitive_Sphere()
{
	m_nPrimType	= I3LV_RES_PRIM_SPHERE;

	setPath( "Sphere");
}

i3ClassMeta *	i3LevelResPrimitive_Sphere::GetInstanceMeta(void)
{
	return i3LevelPrimitive_Sphere::static_meta();
}

i3Node	*	i3LevelResPrimitive_Sphere::_BuildScene()
{
	return i3LevelPrimitive_Sphere::BuildSg();
}