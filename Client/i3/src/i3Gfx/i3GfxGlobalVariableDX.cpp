#include "i3GFXType.h"

#ifdef I3G_DX
I3_EXPORT_GFX IDirect3D9	*			g_pD3D = nullptr;
I3_EXPORT_GFX IDirect3DDevice9 *		g_pD3DDevice = nullptr;
I3_EXPORT_GFX D3DPRESENT_PARAMETERS *	g_pD3Dpp = nullptr;
I3_EXPORT_GFX UINT						g_iCurrentAdapterIndex = 0;
#endif