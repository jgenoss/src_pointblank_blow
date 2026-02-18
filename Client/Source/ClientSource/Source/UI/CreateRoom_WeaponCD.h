#if !defined( __CREATEROOM_WEAPONCD_H__)
#define __CREATEROOM_WEAPONCD_H__

#include "CreateRoom_Base.h"

struct LimitAtt
{
	int Primary, Secondary, Melee, Throw, RPG7;

	LimitAtt() { Primary = Secondary = Melee = Throw = 1;	RPG7 = 0; }
};

class CreateRoom_WeaponCD : public CreateRoom_Base
{
public:
	CreateRoom_WeaponCD(i3UIScene* pBase);
	~CreateRoom_WeaponCD();

public:

	virtual void	InitData(bool isCreateRoom) override;
	virtual void	UpdateforStage(CSI_STAGE* stage, bool firstupdate = false) override;
	virtual bool	ConfimRoomInfo(ROOM_INFO_ALL &info) override;

private:

	void			WeaponCDUIUpdate();

	void			SetLimits();

	UINT8			GetLockAtLua();

private:

	LimitAtt		m_limitatt;
	INT32			m_weaponMode;
	UINT8			m_LockWeapon;

};

#endif