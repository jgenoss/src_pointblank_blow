#pragma once

#include "..\UIHUDBase.h"
#include "..\..\Mode\Domination\Domination_Defines.h"


struct PushEventBase
{
	virtual void Process() = 0;
	virtual ~PushEventBase() = 0 {}
};

class UIHUDDominationBase : public UIHUDBase
{
	I3_CLASS_DEFINE( UIHUDDominationBase, UIHUDBase);

public:
	UIHUDDominationBase(void);
	virtual ~UIHUDDominationBase(void);

	virtual void	OnCreate( i3GameNode* pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSec) override;

	virtual void	_InitializeAtLoad(i3UIScene* pScene) override;
	virtual bool	OnExitStart() override;

	virtual bool	SetHudEvent( INT32 nEventNum, INT32 arg = 0) { return false;}
	virtual bool	PushEvent( PushEventBase* pEvent) { return false; }

protected:
	i3UIScene* m_pUIScene;
};

