#include "i3GfxType.h"
#include "i3Clut.h"
#include "i3GfxUtil.h"
#include "i3VRAMManager.h"

#if defined( I3G_PSP)

I3_CLASS_INSTANCE( i3Clut, i3PersistantElement);


i3Clut::i3Clut(void)
{
	m_Count = 0;
	m_PixelBits = 0;
	m_Format = I3G_IMAGE_FORMAT_RGBA_8888;
	m_pBuff = NULL;
	m_BuffIdx = 0; 
}

i3Clut::~i3Clut(void)
{
	if( m_pBuff != NULL)
	{		
		//i3mem::Free( m_pBuff );
		i3VRAMManager::Free( m_BuffIdx ); 
		m_pBuff = NULL;
	}
}

BOOL i3Clut::Create( INT32 ColorCount, INT32 PixelBits, INT32 AlphaBits)
{
	UINT32 sz;
	I3G_IMAGE_FORMAT format;

	I3ASSERT( ColorCount > 0);

	switch( PixelBits )
	{
		case 16 :
			switch( AlphaBits)
			{
				case 0 :	format = I3G_IMAGE_FORMAT_XRGB_1555;	break;
				case 4 :	format = I3G_IMAGE_FORMAT_ARGB_4444;	break;
				default :	format = I3G_IMAGE_FORMAT_ARGB_1555;	break;
			}
			break;

		default :
			if( AlphaBits > 0)
				format = I3G_IMAGE_FORMAT_RGBA_8888;
			else
				format = I3G_IMAGE_FORMAT_RGBX_8888;
			break;
	}

	I3ASSERT( !(format & I3G_IMAGE_FORMAT_MASK_INDEXED));

	m_Count = ColorCount;
	m_Format = format;
	m_PixelBits = i3Gfx::GetImageFormatBitCount( format);

	sz = ((m_PixelBits * m_Count) + 7) >> 3;


//	m_BuffIdx	= i3MemAlign( sz, 64 );
//	m_pBuff		= i3mem::Lock(m_BuffIdx); 

	m_BuffIdx	= i3VRAMManager::Alloc( sz , 64 );
	m_pBuff		= i3VRAMManager::Lock(m_BuffIdx); 

	return TRUE;
}

void	i3Clut::GetColor( INT32 Index, COLOR * pColor)
{
	UINT16 * pDest;
	UINT8 * pDest2;
	register UINT16 val;
	UINT8 a;

	switch( m_Format)
	{
		case I3G_IMAGE_FORMAT_XRGB_1555 :
			pDest = ((UINT16 *) m_pBuff) + Index;

			val = *pDest;

			i3Color::Set( pColor,	(UINT8) ((val & 0x1F) << 3),
									(UINT8) (((val >> 5) & 0x1F) << 3),
									(UINT8) (((val >> 10) & 0x1F) << 3),
									255);
			break;

		case I3G_IMAGE_FORMAT_ARGB_1555 :
			pDest = ((UINT16 *) m_pBuff) + Index;

			val = *pDest;

			if( val & 0x8000)
				a = 255;
			else
				a = 0;

			i3Color::Set( pColor,	(UINT8) ((val & 0x1F) << 3),
									(UINT8) (((val >> 5) & 0x1F) << 3),
									(UINT8) (((val >> 10) & 0x1F) << 3),
									a);
			break;

		case I3G_IMAGE_FORMAT_ARGB_4444 :
			pDest = ((UINT16 *) m_pBuff) + Index;

			val = *pDest;

			i3Color::Set( pColor,	(UINT8) ((val & 0x0F) << 4),
									(UINT8) ((val & 0xF0)),
									(UINT8) ((val & 0x0F00) >> 4),
									(UINT8) ((val & 0xF000) >> 8));
			break;

		case I3G_IMAGE_FORMAT_RGBA_8888 :
			pDest2 = (UINT8 *) m_pBuff;

			pDest2 += (Index << 2);

			i3Color::Set( pColor, pDest2[0], pDest2[1], pDest2[2], pDest2[3]);
			break;

		default :
			i3Log( "i3Clut::GetColor()", "Invalid Image format.");
			exit( -1);
			break;
	}
}

void	i3Clut::GetColor( INT32 Index, COLORREAL * pColor)
{
	COLOR col;

	GetColor( Index, &col);

	i3Color::Set( pColor, &col);
}

void	i3Clut::SetColor( INT32 Index, COLOR * pColor)
{
	UINT16 * pDest, val;
	UINT8 * pDest2;

	switch( m_Format)
	{
		case I3G_IMAGE_FORMAT_XRGB_1555 :
			val =	(UINT16) ((((i3Color::GetR( pColor) >> 3) & 0x001F)) |
					(((i3Color::GetG( pColor) >> 3) & 0x001F) << 5) |
					(((i3Color::GetB( pColor) >> 3) & 0x001F) << 10));

			pDest = ((UINT16 *) m_pBuff) + Index;

			*pDest = val;
			break;

		case I3G_IMAGE_FORMAT_ARGB_1555 :
			val =	(UINT16) ((((i3Color::GetR( pColor) >> 3) & 0x001F)) |
					(((i3Color::GetG( pColor) >> 3) & 0x001F) << 5) |
					(((i3Color::GetB( pColor) >> 3) & 0x001F) << 10) |
					(((i3Color::GetA( pColor) >> 7) & 0x0001) << 15));

			pDest = ((UINT16 *) m_pBuff) + Index;

			*pDest = val;
			break;

		case I3G_IMAGE_FORMAT_ARGB_4444 :
			val =	(UINT16) ((((i3Color::GetR( pColor) >> 4) & 0x000F)) |
					(((i3Color::GetG( pColor) >> 4) & 0x000F) << 4) |
					(((i3Color::GetB( pColor) >> 4) & 0x000F) << 8) |
					(((i3Color::GetA( pColor) >> 4) & 0x000F) << 12));

			pDest = ((UINT16 *) m_pBuff) + Index;

			*pDest = val;
			break;

		case I3G_IMAGE_FORMAT_RGBX_8888 :
		case I3G_IMAGE_FORMAT_RGBA_8888 :
			pDest2 = (UINT8 *) m_pBuff;

			pDest2 += (Index << 2);

			*pDest2 = i3Color::GetR( pColor);	pDest2++;
			*pDest2 = i3Color::GetG( pColor);	pDest2++;
			*pDest2 = i3Color::GetB( pColor);	pDest2++;
			*pDest2 = i3Color::GetA( pColor);
			break;

		case I3G_IMAGE_FORMAT_BGRX_8888 :
		case I3G_IMAGE_FORMAT_BGRA_8888 :
			pDest2 = (UINT8 *) m_pBuff;

			pDest2 += (Index << 2);

			*pDest2 = i3Color::GetB( pColor);	pDest2++;
			*pDest2 = i3Color::GetG( pColor);	pDest2++;
			*pDest2 = i3Color::GetR( pColor);	pDest2++;
			*pDest2 = i3Color::GetA( pColor);
			break;

		default :
			i3Log( "i3Clut::SetColor()", "Invalid Image Format");
			exit( -1);
			break;
	}
}

void	i3Clut::SetColor( INT32 Index, COLORREAL * pColor)
{
	COLOR col;

	i3Color::Set( &col, pColor);

	SetColor( Index, &col);
}

UINT32	i3Clut::Load( i3Stream * pStream)
{
	I3_CLUT_FILE_HEADER header;
	UINT32	Rc, Result = 0, Sz;
	I3G_IMAGE_FORMAT format;
	bool bSwapColor = false;

	Rc = pStream->Read( &header, sizeof(I3_CLUT_FILE_HEADER));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Clut::Load()", "Could not read clut header data.");
		return STREAM_ERR;
	}

	Result += Rc;

	switch( header.m_Format)
	{
		case I3G_IMAGE_FORMAT_BGRX_8888 :
			format = I3G_IMAGE_FORMAT_RGBX_8888;
			bSwapColor = true;
			break;

		case I3G_IMAGE_FORMAT_BGRA_8888 :
			format = I3G_IMAGE_FORMAT_RGBA_8888;
			//bSwapColor = true;
			break;

		default :
			format = header.m_Format;
			break;
	}

	if( Create( header.m_ColorCount + 1, format) == FALSE)
	{
		i3Log( "i3Clut::Load()", "Could not create clut object.");
		return STREAM_ERR;
	}

	Sz = i3Gfx::CalcRasterBufferSize( header.m_ColorCount + 1, 1, 1, header.m_Format);

	Rc = pStream->Read( m_pBuff, Sz);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Clut::Load()", "Could not read clut pixel data.");
		return STREAM_ERR;
	}

	if( bSwapColor)
	{
		INT32 i;
		UINT8 * pTemp, sp;

		pTemp = (UINT8 *) m_pBuff;

		for( i = 0; i < header.m_ColorCount; i++)
		{
			sp = pTemp[0];
			pTemp[0] = pTemp[2];
			pTemp[2] = sp;

			pTemp += 4;
		}
	}

	Result += Rc;

	return Result;
}

UINT32	i3Clut::Save( i3Stream * pStream)
{
	I3_CLUT_FILE_HEADER header;
	UINT32 Rc, Result = 0, Sz;

	header.m_ColorCount = (UINT8) (GetColorCount() - 1);
	header.m_Format = GetFormat();

	Rc = pStream->Write( &header, sizeof(I3_CLUT_FILE_HEADER));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Clut::Save", "Could not write pixel data.");
		return STREAM_ERR;
	}
	Result += Rc;

	Sz = ((i3Gfx::GetImageFormatBitCount( m_Format) * GetColorCount()) + 7) >> 3;

	Rc = pStream->Write( GetData(), Sz);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Clut::Save()", "Could not write clut pixel data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3Clut::GetColorRGBA32( INT32 idx)
{
	I3ASSERT(0);

	return 0;
}

UINT32	i3Clut::GetColorBGRA32( INT32 idx)
{
	I3ASSERT(0);

	return 0;
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
