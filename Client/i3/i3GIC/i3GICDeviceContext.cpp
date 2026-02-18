#include "stdafx.h"
#include "i3GICDeviceContext.h"

I3_CLASS_INSTANCE( i3GICDeviceContext);

void i3GICDeviceContext::SetModeToDevice(void)
{
	::GetWindowExtEx( m_hDC, &m_SavedWndExt);
	::GetWindowOrgEx( m_hDC, &m_SavedWndOrg);

	::GetViewportExtEx( m_hDC, &m_SavedViewExt);
	::GetViewportOrgEx( m_hDC, &m_SavedViewOrg);

	m_SavedMapMode = ::GetMapMode( m_hDC);

	::SetMapMode( m_hDC, MM_TEXT);
}

void i3GICDeviceContext::SetModeToDraw(void)
{
	::SetMapMode( m_hDC, m_SavedMapMode);
	::SetWindowExtEx( m_hDC, m_SavedWndExt.cx, m_SavedWndExt.cy, nullptr);
	::SetWindowOrgEx( m_hDC, m_SavedWndOrg.x, m_SavedWndOrg.y, nullptr);

	::SetViewportExtEx( m_hDC, m_SavedViewExt.cx, m_SavedViewExt.cy, nullptr);
	::SetViewportOrgEx( m_hDC, m_SavedViewOrg.x, m_SavedViewOrg.y, nullptr);

	::SetMapMode( m_hDC, MM_ANISOTROPIC);
}
