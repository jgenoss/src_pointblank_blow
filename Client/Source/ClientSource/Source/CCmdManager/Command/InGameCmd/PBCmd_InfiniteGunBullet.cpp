#include "pch.h"
#include "PBCmd_InfiniteGunBullet.h"

CMD_RESULT_FLAG PBCmd_InfiniteGunBullet :: RunCommand(REAL32 DeltaSec)
{
	g_pWeaponManager->COMMAND_SetUnlimitGun( GetBoolean());	
	return CMD_RESULT_OK;
}

CMD_RESULT_FLAG PBCmd_InfiniteGrenade :: RunCommand(REAL32 DeltaSec)
{
	g_pWeaponManager->COMMAND_SetUnlimitGrenade( GetBoolean());	
	return CMD_RESULT_OK;
}