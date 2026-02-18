#include "stdafx.h"

#if defined( I3_WINDOWS)
#include "i3Profile.h"

#define DEFAULT_SECTION "Setting"

i3Profile::i3Profile(CWnd *wnd)
{
	m_App = AfxGetApp();
	m_Wnd = wnd;
	m_SectionName = DEFAULT_SECTION;
	SetSection(DEFAULT_SECTION);
}

i3Profile::~i3Profile(void)
{
}

LPCTSTR i3Profile::SetSection(LPCTSTR Name)
{
	m_PrevSectionName = m_SectionName;
	m_SectionName = Name;
	return LPCTSTR(m_PrevSectionName);
}

LPCTSTR i3Profile::GetSection()
{
	return LPCTSTR(m_SectionName);
}

void i3Profile::LoadWindowPos(BOOL bNoSize)
{
	CString PrevSectionName = SetSection(DEFAULT_SECTION);

	int mx, my, cx, cy;

	mx = ::GetSystemMetrics( SM_CXFULLSCREEN) >> 1;
	my = ::GetSystemMetrics( SM_CYFULLSCREEN) >> 1;
	cx = mx + (mx >> 1);
	cy = my + (my >> 1);

	mx -= (cx >> 1);
	my -= (cy >> 1);

	CRect rc;

	rc.left   = GetInt("MainWndX", mx);
	rc.top    = GetInt("MainWndY", my);
	rc.right  = GetInt("MainWndCX", cx);
	rc.bottom = GetInt("MainWndCY", cy);

	SetSection(PrevSectionName);

	UINT Flag = SWP_SHOWWINDOW | (bNoSize ? SWP_NOSIZE : 0);
	m_Wnd->SetWindowPos(NULL, rc.left, rc.top, rc.right, rc.bottom, Flag);
	// m_Wnd->MoveWindow(&rc);
}

void i3Profile::SaveWindowPos()
{
	CString PrevSectionName = SetSection(DEFAULT_SECTION);

	CRect rc;
	m_Wnd->GetWindowRect(&rc);

	WriteInt("MainWndX",  rc.left);
	WriteInt("MainWndY",  rc.top);
	WriteInt("MainWndCX", rc.Width());
	WriteInt("MainWndCY", rc.Height());

	SetSection(PrevSectionName);
}

UINT i3Profile::GetInt(LPCTSTR szEntry, int nDefault)
{
	return m_App->GetProfileInt(m_SectionName, szEntry, nDefault);
}

CString i3Profile::GetString(LPCTSTR szEntry, LPCTSTR szDefault)
{
	return m_App->GetProfileString(m_SectionName, szEntry, szDefault);
}

void i3Profile::WriteInt(LPCTSTR szEntry, int nValue)
{
	BOOL bSuccess = m_App->WriteProfileInt(m_SectionName, szEntry, nValue);
	ASSERT(bSuccess);
}

void i3Profile::WriteString(LPCTSTR szEntry, LPCTSTR szValue)
{
	BOOL bSuccess = m_App->WriteProfileString(m_SectionName, szEntry, szValue);
	ASSERT(bSuccess);
}

#endif