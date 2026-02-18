#if !defined( __CREATEROOM_SUBCD_H__)
#define __CREATEROOM_SUBCD_H__

#include "CreateRoom_Base.h"

class CreateRoom_SubCD : public CreateRoom_Base
{
public:
	CreateRoom_SubCD(i3UIScene* pBase);
	~CreateRoom_SubCD();

public:

	virtual void	 InitData(bool isCreateRoom) override;
	virtual void	 UpdateforStage(CSI_STAGE* stage, bool firstupdate = false) override;
	virtual bool	 ConfimRoomInfo(ROOM_INFO_ALL &info) override;

	void			 SetPushedButton(INT32 slot, INT32 btn);

private:

	bool			 isTeamChangeMap(CSI_STAGE* stage); 

private:

	PushButton_LIST	 button[CR_PushButton::TYPE_MAX];
	bool			 m_bisCreateRoom;
};

#endif
