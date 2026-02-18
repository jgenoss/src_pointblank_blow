#pragma once

#include "../PBConsoleCmd.h"


//void ClientTCPSocket::__Parse_Auth_Shop_Capsule( i3NetworkPacket * pPacket) 과 연동처리가 있습니다
class PBCmd_UseItem : public PBConsoleCmd
{
private:
	UINT32	m_itemID = 0;
	T_ItemDBIdx	m_itemDBIdx = 0;
	REAL32	m_timeBuffer = 0.0f;

public:
	PBCmd_UseItem(void) {}
	~PBCmd_UseItem(void) {}
public:
	virtual CMD_RESULT_FLAG RunCommand(REAL32 DeltaSec) override;
	virtual CMD_RESULT_FLAG ExecuteCommand( i3::vector<i3::string> &params) override;
};
