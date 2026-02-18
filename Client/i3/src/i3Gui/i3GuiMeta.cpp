#include "i3GuiPCH.h"
#include "i3GuiDefines.h"
#include "i3GuiResource.h"
#include "i3GuiBase.h"
#include "i3GuiWindow.h"
#include "i3GuiControl.h"
#include "i3GuiStatic.h"
#include "i3GuiButton.h"
#include "i3GuiEditBox.h"
#include "i3GuiTextBox.h"
#include "i3GuiSlider.h"
#include "i3Gui3DView.h"
#include "i3GuiProgressBar.h"
#include "i3GuiCheckBox.h"
#include "i3GuiRadioButton.h"
#include "i3GuiList.h"
#include "i3GuiPopupList.h"
#include "i3GuiRoot.h"
#include "i3GuiObjBase.h"
#include "i3GuiControlTemplate.h"

#if defined( I3G_DX)
#include "i3GuiMovie.h"
#endif

I3_EXPORT_GUI
void i3GuiRegisterMetas(void)
{
	i3FrameworkRegisterMetas();
}

#if defined( I3_WINDOWS) && defined( I3_DLL)
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3GuiRegisterMetas();
			i3System::HoldDllModule((HMODULE)hInst);
			break;
	}

	return true;
}

#endif
