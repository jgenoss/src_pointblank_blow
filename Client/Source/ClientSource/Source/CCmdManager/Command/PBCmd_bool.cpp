#include "pch.h"
#include "PBCmd_bool.h"

/*virtual*/ CMD_RESULT_FLAG PBCmd_bool::ExecuteCommand( i3::vector<i3::string> &params)
{
	if( params.size() > 0)
	{
		m_bBoolean = atoi( params.at(0).c_str()) > 0;
		return CMD_RESULT_OK;
	}
	else
	{
		m_bBoolean = m_bBoolean!=true;
		return CMD_RESULT_OK; 
	}
}