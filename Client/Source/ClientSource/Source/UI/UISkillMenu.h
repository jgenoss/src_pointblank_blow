#pragma once

#include "UITabBase.h"
//#include "UITabProfileSkill.h"

class UISkillMenu : public UITabBase
{
	I3_CLASS_DEFINE( UISkillMenu, UITabBase );

public:
	virtual void	OnMenuStart() = 0;
	
	virtual void	OnMenuExit()
	{

	}
public:
	virtual void	SetEnable( bool bTrue = true, REAL32 tm = 0.f );
	INT32			GetCurrentTab();
	INT32			GetCurrentAvatar();

public:
	UISkillMenu();
	virtual ~UISkillMenu();
};