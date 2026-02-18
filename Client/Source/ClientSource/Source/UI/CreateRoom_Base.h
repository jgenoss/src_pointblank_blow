#if !defined( __CREATEROOM_BASE_H__)
#define __CREATEROOM_BASE_H__

#include "UIBase.h"
#include "UIMapSlot.h"
#include "UIPushButton.h"

#include "CSI_TypeDef.h"

class CreateRoom_Base
{
public:

	CreateRoom_Base();
	CreateRoom_Base(i3UIScene* pBase);
	~CreateRoom_Base();

public:

	virtual void	InitData(bool isCreateRoom);
	virtual void	UpdateforRule(CSI_RULE* rule , bool firstupdate = false);
	virtual void	UpdateforStage(CSI_STAGE* stage, bool firstupdate = false);
	virtual bool	ConfimRoomInfo(ROOM_INFO_ALL &info);

	void			SetScene(i3UIScene* pBase)
	{
		m_pScene = pBase;
	}

protected:

	LuaState*	_CallLuaFunction(const char* function_name);
	void		_EndLuaFunction(LuaState* ls, int narg);

protected:

	i3UIScene*	m_pScene;
	CSI_STAGE*	m_pStage;

	bool		m_bisCreateRoom;
};

#endif