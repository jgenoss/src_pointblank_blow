#include "i3GfxType.h"
#include "../i3Base/i3Debug.h"
#include "i3GfxUtil.h"
 
#ifdef I3G_OGLES
#include "i3GfxGlobalVariable.h"
#include "i3RenderTarget.h"
#include "i3GfxOGLES.h"
#include "i3GfxUtilOGLES.h"

I3_CLASS_INSTANCE( i3RenderTarget, i3GfxResource);

i3RenderTarget::i3RenderTarget(void)
{
	m_Width = 0;
	m_Height = 0;
	m_Format = I3G_IMAGE_FORMAT_NONE;
	m_bLockable = FALSE;
	m_bLocked = FALSE;
	m_Pitch = 0;

	m_ResType = I3G_RESOURCE_TYPE_RENDERTARGET;

	m_DepthMin = 0.0f;
	m_DepthMax = 1.0f;
}

i3RenderTarget::~i3RenderTarget(void)
{

}

BOOL i3RenderTarget::Create( UINT32 width, UINT32 height, I3G_IMAGE_FORMAT Format, BOOL bLockable)
{
	INT32 NativeFormat, Type;

	m_Width = width;
	m_Height = height;
	m_bLockable = bLockable;

	i3OGLESUT_GetNativeImageFormat( Format, &NativeFormat, &Type);

	m_Format = i3OGLESUT_GetI3ImageFormat( NativeFormat, Type);
	m_ResType = GetType();

	SetDepthRange( 0.0f, 1.0f);

	return TRUE;
}

/*
BOOL i3RenderTarget::Create( void * pNativeRTHandle)
{
	return TRUE;
}
*/

void i3RenderTarget::SetDepthRange( REAL32 Min, REAL32 Max)
{
	FIXED32 vmin, vmax;

	m_DepthMin = Min;
	m_DepthMax = Max;

	vmin = i3FXD::FromReal( Min);
	vmax = i3FXD::FromReal( Max);

	OGL_CHECK( I3OGL::glDepthRangex( vmin, vmax));	
}

void * i3RenderTarget::Lock( I3G_LOCKMETHOD lockMethod)
{
	return NULL;
}

void i3RenderTarget::Unlock(void)
{
}

#endif
