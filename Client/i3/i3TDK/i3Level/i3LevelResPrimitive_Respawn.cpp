#include "stdafx.h"
#include "i3LevelResPrimitive_Respawn.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelPrim_Respawn.h"

//I3_CLASS_INSTANCE( i3LevelResPrimitive_Respawn, i3LevelResPrimitive);
I3_CLASS_INSTANCE( i3LevelResPrimitive_Respawn);

i3LevelResPrimitive_Respawn::i3LevelResPrimitive_Respawn(void)
{
	m_nPrimType = I3LV_RES_PRIMITIVE_RESPAWN;

	setPath( "Respawn");
}

i3ClassMeta *	i3LevelResPrimitive_Respawn::GetInstanceMeta(void)
{
	return i3LevelRespawn::static_meta();
}

i3Node * i3LevelResPrimitive_Respawn::_BuildScene(void)
{
	return i3LevelRespawn::BuildSg();
}

bool i3LevelResPrimitive_Respawn::OnExport( i3LevelScene * pScene, const char * pszPath)
{
	return true;
}

i3GameRes*	i3LevelResPrimitive_Respawn::CreateGameRes()
{
	return nullptr;
}
