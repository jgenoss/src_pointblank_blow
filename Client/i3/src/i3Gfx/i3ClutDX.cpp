#include "i3GFXType.h"
#include "i3Clut.h"
#include "i3GfxUtil.h"

#if defined( I3G_DX)

I3_CLASS_INSTANCE( i3Clut);

i3Clut::~i3Clut(void)
{
	I3MEM_SAFE_FREE_POINTER(m_pBuff); 
}

bool i3Clut::Create( INT32 ColorCount, INT32 /*PixelBits*/, INT32 AlphaBits)
{
	UINT32 sz;
	I3G_IMAGE_FORMAT format;

	I3ASSERT( ColorCount > 0);

	// On DirectX, CLUT data format is fixed with I3G_IMAGE_FORMAT_BGRA_8888 | I3G_IMAGE_FORMAT_BGRX_8888
	if( AlphaBits > 0)
		format = I3G_IMAGE_FORMAT_BGRA_8888;
	else
		format = I3G_IMAGE_FORMAT_BGRX_8888;

	I3ASSERT( !(format & I3G_IMAGE_FORMAT_MASK_INDEXED));

	m_Count = ColorCount;
	m_PixelBits = i3Gfx::GetImageFormatBitCount( format);

	// DirectX에서 Palette는 반드시 256개인 것으로 가정되어 있다.
	sz = sizeof(PALETTEENTRY) * 256;
	
	m_pBuff		= (PALETTEENTRY *) i3MemAlloc( sz );

	return true;
}

void	i3Clut::GetColor( INT32 Index, COLOR * pColor)
{
	PALETTEENTRY * pPal;

	pPal = m_pBuff + Index;

	i3Color::Set( pColor, (UINT8) pPal->peRed, pPal->peGreen, pPal->peBlue, pPal->peFlags);
}

void	i3Clut::GetColor( INT32 Index, COLORREAL * pColor)
{
	PALETTEENTRY * pPal;

	pPal = m_pBuff + Index;

	i3Color::Set( pColor, (UINT8) pPal->peRed, pPal->peGreen, pPal->peBlue, pPal->peFlags);
}

UINT32	i3Clut::GetColorRGBA32( INT32 idx)
{
	PALETTEENTRY * pPal;

	pPal = m_pBuff + idx;

	return (((UINT32)pPal->peRed) << 24) | (pPal->peGreen << 16) | (pPal->peBlue << 8) | (pPal->peFlags);
}

UINT32 i3Clut::GetColorBGRA32( INT32 idx)
{
	PALETTEENTRY * pPal;

	pPal = m_pBuff + idx;

	return (((UINT32)pPal->peFlags) << 24) | (pPal->peRed << 16) | (pPal->peGreen << 8) | (pPal->peBlue);
}

void	i3Clut::SetColor( INT32 Index, COLOR * pColor)
{
	PALETTEENTRY * pPal;

	pPal = m_pBuff + Index;

	pPal->peRed		= i3Color::GetR( pColor);
	pPal->peGreen	= i3Color::GetG( pColor);
	pPal->peBlue	= i3Color::GetB( pColor);
	pPal->peFlags	= i3Color::GetA( pColor);
}

void	i3Clut::SetColor( INT32 Index, COLORREAL * pColor)
{
	PALETTEENTRY * pPal;

	pPal = m_pBuff + Index;

	pPal->peRed		= (UINT8) (i3Color::GetR( pColor) * 255.0f);
	pPal->peGreen	= (UINT8) (i3Color::GetG( pColor) * 255.0f);
	pPal->peBlue	= (UINT8) (i3Color::GetB( pColor) * 255.0f);
	pPal->peFlags	= (UINT8) (i3Color::GetA( pColor) * 255.0f);
}

UINT32	i3Clut::Load( i3Stream * pStream)
{
	i3::pack::CLUT_FILE_HEADER header;
	UINT32	Rc, Result = 0;

	Rc = pStream->Read( &header, sizeof(i3::pack::CLUT_FILE_HEADER));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Clut::Load()", "Could not read clut header data.");
		return STREAM_ERR;
	}

	Result += Rc;

	if( Create( header.m_ColorCount + 1, header.m_Format) == false)
	{
		i3Log( "i3Clut::Load()", "Could not create clut object.");
		return STREAM_ERR;
	}

	// CLUT의 Format은 반드시 I3G_IMAGE_FORMAT_ARGB_8888인 것으로 가정한다.
	// 각 Color Component의 순서는 R, G, B, A의 순이다.

	Rc = pStream->Read( m_pBuff, sizeof(PALETTEENTRY) * GetColorCount());
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Clut::Load()", "Could not read clut pixel data.");
		return STREAM_ERR;
	}

	Result += Rc;

	return Result;
}

UINT32	i3Clut::Save( i3Stream * pStream)
{
	i3::pack::CLUT_FILE_HEADER header;
	UINT32 Rc, Result = 0;

	header.m_ColorCount = (UINT8) (GetColorCount() - 1);
	header.m_Format = GetFormat();

	Rc = pStream->Write( &header, sizeof(i3::pack::CLUT_FILE_HEADER));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Clut::Save", "Could not write pixel data.");
		return STREAM_ERR;
	}
	Result += Rc;

	Rc = pStream->Write( GetData(), sizeof(PALETTEENTRY) * GetColorCount());
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Clut::Save()", "Could not write clut pixel data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3Clut::OnLoad( i3ResourceFile * pResFile)
{
	return Load( pResFile->GetStream());
}

UINT32 i3Clut::OnSave( i3ResourceFile * pResFile)
{
	return Save( pResFile->GetStream());
}
#endif