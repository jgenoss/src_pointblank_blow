#if !defined( __I3G_GLOBAL_VARIABLE_XB_H)
#define __I3G_GLOBAL_VARIABLE_XB_H

#if !defined(I3_XBOX) || !defined(I3G_XBOX)
#error "This header file is only for Xbox."
#endif

#include <xtl.h>

extern LPDIRECT3D8			g_pD3D;
extern LPDIRECT3DDEVICE8	g_pD3DDevice;

#endif