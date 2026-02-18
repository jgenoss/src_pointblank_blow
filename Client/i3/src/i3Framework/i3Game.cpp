#include "i3FrameworkPCH.h"
#include "i3Game.h"

static i3Framework *		s_pFrmWork = nullptr;

i3Framework * i3Game::getFramework(void)
{
	return s_pFrmWork;
}

void i3Game::setFramework( i3Framework * pFrmwork)
{
	s_pFrmWork = pFrmwork;
}

i3EffectManager * i3Game::getEffectManager(void)
{
	return s_pFrmWork->getEffectManager();
}

