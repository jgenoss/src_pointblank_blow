#if !defined( __I3G_RENDER_STATE_H)
#define __I3G_RENDER_STATE_H

#if defined( I3G_DX)
 #include "i3RenderStateDX.h"
#elif defined( I3G_OGL)
 #include "i3RenderStateOpenGL.h"
#elif defined( I3G_OGLES)
	#include "i3RenderStateOGLES.h"
#elif defined( I3G_XBOX)
 #include "i3RenderStateXbox.h"
#elif defined( I3G_PSP)
	#include "i3RenderStatePSP.h"
#elif defined( I3G_PS2)
 #include "i3RenderStatePS2.h"
#endif

#endif
