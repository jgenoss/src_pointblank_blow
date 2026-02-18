#include "i3FrameworkPCH.h"
#include "i3GameInterface.h"

void i3GameInterface::OnRegisterMeta(void)
{
	i3BaseRegisterMetas();
	i3MathRegisterMetas();
	i3InputRegisterMetas();
	i3SoundRegisterMetas();
	i3GfxRegisterMetas();
	i3SceneRegisterMetas();

	s_pInteractProc = nullptr;
	s_pInteractProc2 = nullptr;
	s_pInteractProc3 = nullptr;
}

bool i3GameInterface::OnDrive( i3Viewer * pViewer)
{
	pViewer->Render();

	return true;
}

#if defined( I3_WINDOWS)
UINT32	i3GameInterface::GetViewportStyle(void)
{
	return WS_OVERLAPPEDWINDOW;
}

UINT32	i3GameInterface::GetIcon(void)
{
	return 0;
}

#if defined( I3_DEBUG)
INT32 i3GameInterface::OnQueryPropertyDlgCount(void)
{
	return 0;
}

void i3GameInterface::OnQueryPropertyDlg( HINSTANCE hInst, INT32 idx, i3ClassMeta ** ppMeta, i3GameObjPropertyDialog ** ppDlg)
{
	*ppDlg = nullptr;
}
#endif

#endif
