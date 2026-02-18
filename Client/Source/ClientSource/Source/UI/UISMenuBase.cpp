#include "pch.h"
#include "UISMenuBase.h"

/***********************//***********************/
iSMenu::iSMenu(UISideMenu* p) : mParent(p), m_bSubMenuOpened(false)
{
	m_TempCharaInfo.Reset();
}

iSMenu::~iSMenu()
{
}