#include "StdAfx.h"
#include "UIEditorProfile.h"

CUIEditorProfile::CUIEditorProfile(void)
{
	m_szWorkDir[0] = 0;

	Load();
}

CUIEditorProfile::~CUIEditorProfile(void)
{
	Save();
}

void	CUIEditorProfile::Save()
{
	AfxGetApp()->WriteProfileString( "OPTION", "WorkDir", m_szWorkDir);

	AfxGetApp()->WriteProfileInt("MainWnd", "X",	m_MainWndX);
	AfxGetApp()->WriteProfileInt("MainWnd", "Y",	m_MainWndY);
	AfxGetApp()->WriteProfileInt("MainWnd", "CX",	m_MainWndCX);
	AfxGetApp()->WriteProfileInt("MainWnd", "CY",	m_MainWndCY);
}

void	CUIEditorProfile::Load()
{
	CString str;

	str = AfxGetApp()->GetProfileString("OPTION", "WorkDir", "");
	strcpy( m_szWorkDir, LPCTSTR( str));

	m_MainWndX	= AfxGetApp()->GetProfileInt( "MainWnd", "X", CW_USEDEFAULT);
	m_MainWndY	= AfxGetApp()->GetProfileInt( "MainWnd", "Y", CW_USEDEFAULT);
	m_MainWndCX = AfxGetApp()->GetProfileInt( "MainWnd", "CX", CW_USEDEFAULT);
	m_MainWndCY = AfxGetApp()->GetProfileInt( "MainWnd", "CY", CW_USEDEFAULT);

	i3ResourceFile::SetWorkingFolder( m_szWorkDir);
	i3ResourceFile::setSearchExternRefEnable( true);
}
