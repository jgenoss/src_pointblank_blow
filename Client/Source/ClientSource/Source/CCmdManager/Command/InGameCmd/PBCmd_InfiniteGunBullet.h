#pragma once

#include "../PBCmd_bool.h"


class PBCmd_InfiniteGunBullet : public PBCmd_bool
{
public:
	virtual CMD_RESULT_FLAG RunCommand(REAL32 DeltaSec) override;
};

class PBCmd_InfiniteGrenade : public PBCmd_bool
{
public:
	virtual CMD_RESULT_FLAG RunCommand( REAL32 DeltaSec) override;
};