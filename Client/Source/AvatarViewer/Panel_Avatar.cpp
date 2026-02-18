// Panel_Avatar.cpp : implementation file
//

#include "pch.h"
#include "AvatarViewer.h"
#include "Panel_Avatar.h"
#include "GlobalVar.h"

// CPanel_Avatar

IMPLEMENT_DYNAMIC(CPanel_Avatar, i3TDKRibbonDockBase)

CPanel_Avatar::CPanel_Avatar()
{

}

CPanel_Avatar::~CPanel_Avatar()
{
}


void CPanel_Avatar::UpdateAll(void)
{
	if( g_pAvatar != NULL)
		m_Dlg.UpdateAll( g_pAvatar);
}

void CPanel_Avatar::CreatePartsList( void)
{
	m_Dlg.CreatePartsList();
}

BEGIN_MESSAGE_MAP(CPanel_Avatar, i3TDKRibbonDockBase)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CPanel_Avatar message handlers



int CPanel_Avatar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKRibbonDockBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		m_Dlg.Create( IDD_PANEL_AVATAR, this);
	}

	return 0;
}
