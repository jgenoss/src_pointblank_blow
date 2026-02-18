#include "pch.h"
#include "UISkillMenu.h"

//I3_ABSTRACT_CLASS_INSTANCE( UISkillMenu, UITabBase );
I3_CLASS_INSTANCE( UISkillMenu);

UISkillMenu::UISkillMenu()
{
}

UISkillMenu::~UISkillMenu()
{
}

void	UISkillMenu::SetEnable( bool bTrue , REAL32 tm )
{
	UITabBase::SetEnable( bTrue, tm );

	if( UITabBase::isEnable() == bTrue )
	{
		if( bTrue )
			OnMenuStart();
		else
			OnMenuExit();
	}
	
}

INT32	UISkillMenu::GetCurrentTab()
{
	return 0;
}

INT32	UISkillMenu::GetCurrentAvatar()
{
	return 0;
}