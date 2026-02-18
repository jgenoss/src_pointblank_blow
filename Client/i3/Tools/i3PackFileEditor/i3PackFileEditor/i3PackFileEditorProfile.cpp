#include "stdafx.h"
#include "i3PackFileEditorProfile.h"

i3PackFileEditorProfile::i3PackFileEditorProfile(void)
{
	m_szWorkDir[0] = 0;
	m_szTexDir[0] = 0;
	m_szOutDir[0] = 0;
	Load();
}

i3PackFileEditorProfile::~i3PackFileEditorProfile(void)
{
	Save();
}

void i3PackFileEditorProfile::Load(void)
{
	CString str;

	str = AfxGetApp()->GetProfileString( "OPTION", "WorkDir", "");
	strcpy( m_szWorkDir, LPCTSTR( str));

	str = AfxGetApp()->GetProfileString( "OPTION", "TexDir", "");
	strcpy( m_szTexDir, LPCTSTR( str));

	str = AfxGetApp()->GetProfileString( "OPTION", "OutDir", "");
	strcpy( m_szOutDir, LPCTSTR( str));

	m_MainWndX = AfxGetApp()->GetProfileInt( "MainWnd", "X", CW_USEDEFAULT);
	m_MainWndY = AfxGetApp()->GetProfileInt( "MainWnd", "Y", CW_USEDEFAULT);
	m_MainWndCX = AfxGetApp()->GetProfileInt( "MainWnd", "CX", CW_USEDEFAULT);
	m_MainWndCY = AfxGetApp()->GetProfileInt( "MainWnd", "CY", CW_USEDEFAULT);
}

void i3PackFileEditorProfile::Save(void)
{
	AfxGetApp()->WriteProfileString( "OPTION", "WorkDir", m_szWorkDir);
	AfxGetApp()->WriteProfileString( "OPTION", "TexDir", m_szTexDir);
	AfxGetApp()->WriteProfileString( "OPTION", "OutDir", m_szOutDir);

	AfxGetApp()->WriteProfileInt( "MainWnd", "X", m_MainWndX);
	AfxGetApp()->WriteProfileInt( "MainWnd", "Y", m_MainWndY);
	AfxGetApp()->WriteProfileInt( "MainWnd", "CX", m_MainWndCX);
	AfxGetApp()->WriteProfileInt( "MainWnd", "CY", m_MainWndCY);
}