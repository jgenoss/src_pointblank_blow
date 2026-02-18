#include "i3GfxType.h"
#include "../i3Base/i3Debug.h"
#include "i3GfxUtil.h"

#ifdef I3G_PSP
#include "i3GfxGlobalVariable.h"
#include "i3RenderTarget.h"
#include "i3GfxUtilPSP.h"

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

	m_DrawBP = SCEGU_VRAM_BP_0;
	m_DispBP = SCEGU_VRAM_BP_1;
	m_DepthBP = SCEGU_VRAM_BP_2;

	m_OffsetX = SCEGU_SCR_OFFSETX;
	m_OffsetY = SCEGU_SCR_OFFSETY;

	m_DepthMin = 65535;
	m_DepthMax = 0;
}

i3RenderTarget::~i3RenderTarget(void)
{

}

BOOL i3RenderTarget::Create( UINT32 width, UINT32 height, I3G_IMAGE_FORMAT Format, BOOL bLockable)
{
	int native_format;

	m_Width = width;
	m_Height = height;
	m_Format = Format;
	m_bLockable = bLockable;

	native_format = i3PSPUT_GetNativeImageFormat( Format);

	if( i3Gfx::GetImageFormatBitCount( Format) == 16)
	{
		m_DrawBP = SCEGU_VRAM_BP_0;
		m_DispBP = SCEGU_VRAM_BP_1;
		m_DepthBP = SCEGU_VRAM_BP_2;
	}
	else
	{
		m_DrawBP = SCEGU_VRAM_BP32_0;
		m_DispBP = SCEGU_VRAM_BP32_1;
		m_DepthBP = SCEGU_VRAM_BP32_2;
	}

	// m_DrawBP와 m_DispBP의 간격은 SCEGU_VRAM_BUFSIZE 크기만큼이다.
	//
	// 만약 32 bits format으로 Render Target을 생성해야 한다면,
	// m_DrawBP와 m_DispBP의 간격은 SCEGU_VRAM_BUFSIZE * 2 크기만큼이 되어야 한다.
	//
	// 현재는 단지 16 bits format만을 고려한다.
	sceGuDrawBuffer( native_format, m_DrawBP, SCEGU_VRAM_WIDTH);
	sceGuDispBuffer( m_Width, m_Height, m_DispBP, SCEGU_VRAM_WIDTH);
	sceGuDepthBuffer( m_DepthBP, SCEGU_VRAM_WIDTH);

	SetOffset( m_OffsetX, m_OffsetY);
	SetDepthRange( m_DepthMin, m_DepthMax);

	m_ResType = GetType();

	return TRUE;
}

BOOL i3RenderTarget::Create( void * pNativeRTHandle)
{
	return TRUE;
}

void i3RenderTarget::SetOffset( INT32 x, INT32 y)
{
	m_OffsetX = x;
	m_OffsetY = y;

	sceGuOffset( m_OffsetX, m_OffsetY);
}

void i3RenderTarget::SetDepthRange( UINT32 Min, UINT32 Max)
{
	m_DepthMin = Min;
	m_DepthMax = Max;

	sceGuDepthRange( m_DepthMin, m_DepthMax);
	sceGuDepthOffset( 0);
}

UINT32 i3RenderTarget::GetBufferSize(void)
{
	return i3Gfx::CalcRasterBufferSize( m_Width, m_Height, 16, m_Format);
}

void * i3RenderTarget::Lock( I3G_LOCKMETHOD lockMethod)
{
	return NULL;
}

void i3RenderTarget::Unlock(void)
{
}

#endif
