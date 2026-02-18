#include "StdAfx.h"
#include "PropEditorProfile.h"

CPropEditorProfile::CPropEditorProfile(void)
{
	Load();
}

CPropEditorProfile::~CPropEditorProfile(void)
{
	Save();
}

void	CPropEditorProfile::Load( void)
{
	m_MainWndX	= AfxGetApp()->GetProfileInt( "MainWnd", "X", CW_USEDEFAULT);
	m_MainWndY	= AfxGetApp()->GetProfileInt( "MainWnd", "Y", CW_USEDEFAULT);

	m_MainWndCX = AfxGetApp()->GetProfileInt( "MainWnd", "CX", CW_USEDEFAULT);
	m_MainWndCY = AfxGetApp()->GetProfileInt( "MainWnd", "CY", CW_USEDEFAULT);

	CRect	rc;
	GetClientRect(GetDesktopWindow(), rc);

	if ( m_MainWndX < 0 || m_MainWndX >= rc.right )
	{
		m_MainWndX = 10;
	}

	if ( m_MainWndY < 0 || m_MainWndY >= rc.bottom )
	{
		m_MainWndY = 10;
	}

	if( m_MainWndCX == 0)
	{
		m_MainWndX	= 0;
		m_MainWndY	= 0;
		m_MainWndCX = 680;
		m_MainWndCY = 480;
	}
}

void	CPropEditorProfile::Save( void)
{
	AfxGetApp()->WriteProfileInt( "MainWnd", "X", m_MainWndX);
	AfxGetApp()->WriteProfileInt( "MainWnd", "Y", m_MainWndY);
	AfxGetApp()->WriteProfileInt( "MainWnd", "CX", m_MainWndCX);
	AfxGetApp()->WriteProfileInt( "MainWnd", "CY", m_MainWndCY);
}