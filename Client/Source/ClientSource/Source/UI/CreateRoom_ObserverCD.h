#if !defined( __CREATEROOM_OBSERVERCD_H__)
#define __CREATEROOM_OBSERVERCD_H__

#include "CreateRoom_Base.h"

class CreateRoom_ObserverCD : public CreateRoom_Base
{
public:
	CreateRoom_ObserverCD(i3UIScene* pBase);
	~CreateRoom_ObserverCD();

public:

	virtual void	InitData(bool isCreateRoom) override;
	virtual void	UpdateforStage(CSI_STAGE* stage, bool firstupdate = false) override;
	virtual bool	ConfimRoomInfo(ROOM_INFO_ALL &info) override;

private:

	void			ObserverCDUIUpdate();

	UINT8			GetLockAtLua();

private:

	UINT8			m_LockObserver;

};

#endif