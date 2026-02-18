#include "stdafx.h"
#include "i3TDKDock.h"

static	bool	s_bDockUpdatePending = false;

I3_EXPORT_TDK
void i3TDKDock::SetPendingUpdate( bool bFlag)
{
	s_bDockUpdatePending = bFlag;
}

I3_EXPORT_TDK
bool i3TDKDock::IsPendingUpdate(void)
{
	return s_bDockUpdatePending;
}

