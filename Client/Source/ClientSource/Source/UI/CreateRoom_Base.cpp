#include "pch.h"
#include "CreateRoom_Base.h"

#include "ClientStageInfoUtil.h"

CreateRoom_Base::CreateRoom_Base()
{
	m_pScene			= nullptr;
	m_pStage			= nullptr;
	m_bisCreateRoom		= false;
}

CreateRoom_Base::CreateRoom_Base(i3UIScene * pBase)
{
	m_pScene = pBase;
}

CreateRoom_Base::~CreateRoom_Base()
{
	m_pScene = nullptr;
	m_pStage = nullptr;
}

void		CreateRoom_Base::InitData(bool isCreateRoom)
{
}

void		CreateRoom_Base::UpdateforRule(CSI_RULE* rule, bool firstupdate)
{
}

void		CreateRoom_Base::UpdateforStage(CSI_STAGE* stage, bool firstupdate)
{
}

bool		CreateRoom_Base::ConfimRoomInfo(ROOM_INFO_ALL & info)
{
	return true;
}

LuaState *	CreateRoom_Base::_CallLuaFunction(const char * function_name)
{
	return UILua::CallLuaFunction(m_pScene, function_name);
}

void		CreateRoom_Base::_EndLuaFunction(LuaState * ls, int narg)
{
	UILua::EndLuaFunction(ls, narg);
}
