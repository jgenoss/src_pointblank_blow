#pragma once

#include "../PBCmd_bool.h"

class PBCmd_PenetrateWall : public PBCmd_bool
{
public:
	virtual CMD_RESULT_FLAG RunCommand(REAL32 DeltaSec) override;
};
