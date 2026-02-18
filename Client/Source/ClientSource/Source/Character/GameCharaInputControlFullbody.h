#ifndef _GAME_CHARA_INPUT_CONTROL_FULLBODY_H_
#define _GAME_CHARA_INPUT_CONTROL_FULLBODY_H_

#include "GameCharaInputControl.h"

// 상속보다는 구성으로. 네트워크 인풋에서 사용가능한 기능만 인터페이스로 빼둠.
// Update() 부분에서는 네트워크 인풋의 불필요한 로직 제외.
class CGameCharaInputControlNetwork;
class CGameCharaInputControlFullbody : public CGameCharaInputControl
{
	I3_CLASS_DEFINE(CGameCharaInputControlFullbody, CGameCharaInputControl);

public:
	virtual void	OnFirstUpdate(REAL32 tm) override;
	virtual void	OnUpdate(REAL32 tm) override;

	void			ProcessNetworkPosition(GAMEINFO_CHARA_NETDATA* info);
	void			ProcessNetworkRotationTheta(GAMEINFO_CHARA_NETDATA* info);
	void			ProcessNetworkRotationRho(GAMEINFO_CHARA_NETDATA* info);

public:
	void			SetNetworkInputControl(CGameCharaInputControlNetwork* adaptee);

private:
	CGameCharaInputControlNetwork* m_controlNetwork = nullptr;
};

#endif