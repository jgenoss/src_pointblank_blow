#pragma once

#include "../PBConsoleCmd.h"


//void ClientTCPSocket::__Parse_Auth_Shop_Capsule( i3NetworkPacket * pPacket) 과 연동처리가 있습니다
class PBCmd_SetGunBullet : public PBConsoleCmd
{
private:
	INT32 m_Bullet = 0;
public:
	PBCmd_SetGunBullet(void) {}
	virtual ~PBCmd_SetGunBullet(void) {}
public:
	virtual CMD_RESULT_FLAG RunCommand(REAL32 DeltaSec) override;
	virtual CMD_RESULT_FLAG ExecuteCommand( i3::vector<i3::string> &params) override;
};
