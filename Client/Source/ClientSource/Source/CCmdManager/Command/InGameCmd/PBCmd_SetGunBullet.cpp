#include "pch.h"
#include "PBCmd_SetGunBullet.h"

CMD_RESULT_FLAG PBCmd_SetGunBullet :: RunCommand(REAL32 DeltaSec)
{
	if(g_pWeaponManager->COMMAND_GetGunBullet() == m_Bullet)
		return CMD_RESULT_LOOP;
	g_pWeaponManager->COMMAND_SetGunBullet(m_Bullet);	
	return CMD_RESULT_OK;
}

/*virtual*/ CMD_RESULT_FLAG PBCmd_SetGunBullet::ExecuteCommand( i3::vector<i3::string> &params)
{
	if( params.size() > 0)
	{
		m_Bullet = atoi( params.at(0).c_str());

		if(m_Bullet < 0)
			m_Bullet = 0;
		else if(m_Bullet > 999)
			m_Bullet = 999;

		return CMD_RESULT_OK;
	}

	return CMD_RESULT_FAIL;
}

