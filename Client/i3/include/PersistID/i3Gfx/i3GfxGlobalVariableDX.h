#if !defined( __I3G_GLOBAL_VARIABLE_DX_H)
#define __I3G_GLOBAL_VARIABLE_DX_H

#if !defined(I3_WINDOWS) || !defined(I3G_DX)
#error "This header file is only for DirectX and Windows."
#endif

#include <d3d9.h>

I3_EXTERN_GFX LPDIRECT3D9			g_pD3D;
I3_EXTERN_GFX LPDIRECT3DDEVICE9	g_pD3DDevice;

#endif