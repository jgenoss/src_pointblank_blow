#pragma once

#include "../PBConsoleCmd.h"

class PBCmd_ViewBone : public PBConsoleCmd
{
private:
	INT32	m_UserIndex;
public:
	virtual CMD_RESULT_FLAG ExecuteCommand( i3::vector<i3::string> &params) override;
	virtual CMD_RESULT_FLAG RunCommand(REAL32 DeltaSec) override;
};
