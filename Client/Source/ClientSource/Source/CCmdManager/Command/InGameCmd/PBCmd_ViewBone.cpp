#include "pch.h"
#include "PBCmd_ViewBone.h"

CMD_RESULT_FLAG PBCmd_ViewBone::ExecuteCommand( i3::vector<i3::string> &params)
{
	if( params.size() > 0)
	{
		m_UserIndex = atoi( params.at(0).c_str());
	}
	else
		m_UserIndex = -1;
	return CMD_RESULT_OK;
}

CMD_RESULT_FLAG PBCmd_ViewBone::RunCommand(REAL32 DeltaSec)
{
	if( m_UserIndex == -1)
	{
		for( INT32 i = 0; i < g_pCharaManager->getCharaCount(); i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara(i);
			if( pChara != nullptr)
			{
				pChara->ToggleRenderBone();
			}
		}
	}
	else
	{
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_UserIndex);
		if( pChara != nullptr)
		{
			pChara->ToggleRenderBone();
		}
	}
	return CMD_RESULT_OK;
}