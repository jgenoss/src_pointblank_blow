#include "i3GfxType.h"
#include "i3RenderTarget.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED RENDER_TARGET_INFO
	{
		UINT16				m_Width = 0;
		UINT16				m_Height = 0;
		I3G_IMAGE_FORMAT	m_Format = I3G_IMAGE_FORMAT_NONE;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3RenderTarget::OnSave( i3ResourceFile * pResFile)
{
	pack::RENDER_TARGET_INFO Info;
	UINT32 Rc;

	Info.m_Width = (UINT16) GetWidth();
	Info.m_Height = (UINT16) GetHeight();
	Info.m_Format = GetFormat();

	Rc = pResFile->GetStream()->Write( &Info, sizeof(pack::RENDER_TARGET_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3RenderTarget::OnSave()", "Could not write render target information.");
	}

	return Rc;
}

UINT32 i3RenderTarget::OnLoad( i3ResourceFile * pResFile)
{
	pack::RENDER_TARGET_INFO Info;
	UINT32	Rc;
	
	Rc = pResFile->GetStream()->Read( &Info, sizeof(pack::RENDER_TARGET_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3RenderTarget::OnLoad()", "Could not read render target information.");
		return STREAM_ERR;
	}

	bool Rv = Create( Info.m_Width, Info.m_Height, Info.m_Format);
	#if defined( I3_DEBUG)
	if( Rv == false)
	{
		i3Log( "i3RenderTarget::OnLoad()", "Could not create render target (%d, %d, %d).", 
			Info.m_Width, Info.m_Height, Info.m_Format);
		return STREAM_ERR;
	}
	#endif

	return Rc;
}
