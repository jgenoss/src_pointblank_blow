#include "stdafx.h"
#include "i3MVFGenProfile.h"

Ci3MVFGenProfile::Ci3MVFGenProfile(void)
{
	m_szImportDir[0] = 0;
	m_szFileListName[0] = 0;
	m_szWorkDir[0] = 0;
	m_szExceptRull[0] = 0;
	m_nAlign = 4;
	Load();
}

Ci3MVFGenProfile::~Ci3MVFGenProfile(void)
{
	Save();
}

void Ci3MVFGenProfile::Load(void)
{
	CString str;

	str = AfxGetApp()->GetProfileString( "OPTION", "ImportDir", "");
	strcpy( m_szImportDir, LPCTSTR( str));
	
	str = AfxGetApp()->GetProfileString( "OPTION", "WorkDir", "");
	strcpy( m_szWorkDir, LPCTSTR( str));

	str = AfxGetApp()->GetProfileString( "OPTION", "ExceptRull", "SCC");
	strcpy( m_szExceptRull, LPCTSTR( str));

	m_nAlign = AfxGetApp()->GetProfileInt( "OPTION", "Align", 1 );

	m_MainWndX = AfxGetApp()->GetProfileInt( "MainWnd", "X", CW_USEDEFAULT);
	m_MainWndY = AfxGetApp()->GetProfileInt( "MainWnd", "Y", CW_USEDEFAULT);
	m_MainWndCX = AfxGetApp()->GetProfileInt( "MainWnd", "CX", CW_USEDEFAULT);
	m_MainWndCY = AfxGetApp()->GetProfileInt( "MainWnd", "CY", CW_USEDEFAULT);
}

void Ci3MVFGenProfile::Save(void)
{
	AfxGetApp()->WriteProfileString( "OPTION", "ImportDir", m_szImportDir);
	AfxGetApp()->WriteProfileString( "OPTION", "WorkDir", m_szWorkDir);
	AfxGetApp()->WriteProfileString( "OPTION", "ExceptRull", m_szExceptRull);
	AfxGetApp()->WriteProfileInt( "OPTION", "Align", m_nAlign );

	AfxGetApp()->WriteProfileInt( "MainWnd", "X", m_MainWndX);
	AfxGetApp()->WriteProfileInt( "MainWnd", "Y", m_MainWndY);
	AfxGetApp()->WriteProfileInt( "MainWnd", "CX", m_MainWndCX);
	AfxGetApp()->WriteProfileInt( "MainWnd", "CY", m_MainWndCY);
}