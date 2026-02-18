#include <dds.h>

#include "ImageUtilCalc.h"
#include "ImageUtilDDS.h"
#include "ImageUtil16bits.h"
#include "ImageUtil24bits.h"
#include "ImageUtil32bits.h"

INT32 _CalcTotalBits(UINT32 val)
{
	INT32 count = 0, mask = 1, i;

	for (i = 0; i < 32; i++)
	{
		if (val & mask)
		{
			count++;
		}

		mask = mask << 1;
	}

	return count;
}


I3G_IMAGE_FORMAT FindImageFormat(UINT32 rmask, UINT32 gmask, UINT32 bmask, UINT32 amask)
{
	INT32 TotalBits, GBits;

	TotalBits = _CalcTotalBits(rmask | gmask | bmask | amask);

	switch (TotalBits)
	{
	case 8:	return I3G_IMAGE_FORMAT_I8;

	case 16:
		switch (_CalcTotalBits(amask))
		{
		case 0:
			GBits = _CalcTotalBits(gmask);

			switch (GBits)
			{
			case 4:			// 4444
				return I3G_IMAGE_FORMAT_XRGB_4444;

			case 5:			// x555
				return I3G_IMAGE_FORMAT_XRGB_1555;

			case 6:			// 565
				return I3G_IMAGE_FORMAT_RGB_565;
			}
			break;

		case 1:
			// 1555
			return I3G_IMAGE_FORMAT_ARGB_1555;

		case 4:
			// 4444
			return I3G_IMAGE_FORMAT_ARGB_4444;
		}
		break;

	case 24:
		if (rmask == 0x00FF0000)
		{
			// BGR
			return I3G_IMAGE_FORMAT_BGR_888;
		}
		else
		{
			// RGB
			return I3G_IMAGE_FORMAT_RGB_888;
		}

	case 32:
		if (rmask == 0x00FF0000)
		{
			if (amask != 0)
			{
				// BGRA
				return I3G_IMAGE_FORMAT_BGRA_8888;
			}
			else
			{
				// BGRX
				return I3G_IMAGE_FORMAT_BGRX_8888;
			}
		}
		else
		{
			if (amask != 0)
			{
				// RGBA
				return I3G_IMAGE_FORMAT_RGBA_8888;
			}
			else
			{
				// RGBX
				return I3G_IMAGE_FORMAT_RGBX_8888;
			}
		}
	}

	return I3G_IMAGE_FORMAT_NONE;
}

HRESULT LoadCubeMapDDS(IStream * pStream, BYTE * pDest, uint32_t width, uint32_t height, UINT32 imageSize, I3G_IMAGE_FORMAT format)
{
	HRESULT hr;
	char * pSrc = NULL;
	INT32 face;

	if ((format & I3G_IMAGE_FORMAT_MASK_COMPRESSED) == 0)
	{
		//Uncompressed format
		INT32 size = CalcRasterBufferSize(width, 1, 4, format);
		pSrc = (char *)malloc(size);
	}

	//face 6개에 대해서 계산을 해준다

	for (face = 0; face < 6; face++)
	{
		if (pSrc != NULL)
		{
			hr = LoadRasterData(pStream, pSrc, &pDest[imageSize * face], width, height, format, format);
		}
		else
		{
			hr = LoadCompressedDDS(pStream, &pDest[imageSize * face], width, height, format);
		}
	}

	if (pSrc != NULL)
	{
		free(pSrc);
	}

	return hr;
}

HRESULT LoadCompressedDDS(IStream * pStream, BYTE * pDest, uint32_t width, uint32_t height, I3G_IMAGE_FORMAT format)
{
	HRESULT hr;
	UINT32 LineSize;
	ULONG cbValue;

	// Compressed Texture의 각 MIPMAP Level Image의 크기는 1/4로 줄어든다.
	// 그러나, 아무리 작은 크기라 하더라도 8 bytes 보다 작아지지 않는다.
	// Compressed Tesxture의 데이터 크기를 구하는 식은 아래와 같다.
	//
	//  max(1, width ÷ 4) x max(1, height ÷ 4) x 8(DXT1) or 16(DXT2-5)
	//

	LineSize = CalcRasterBufferSize(width, height, 4, format);

	hr = pStream->Read(pDest, LineSize, &cbValue);

	return hr;
}

HRESULT LoadUnCompressedDDS(IStream * pStream, BYTE * pDest, uint32_t width, uint32_t height, I3G_IMAGE_FORMAT format)
{
	HRESULT hr;
	char * pSrc = NULL;
	UINT32 SrcLineSz;

	SrcLineSz = CalcRasterBufferSize(width, 1, 4, format);	// DWORD align

	pSrc = (char *)malloc(SrcLineSz);

	hr = LoadRasterData(pStream, pSrc, pDest, width, height, format, format);

	if (pSrc != NULL)
	{
		free(pSrc);
	}

	return hr;
}

HRESULT LoadRasterData(IStream * pStream, char * pSrc, BYTE * pDest, UINT32 width, UINT32 height, I3G_IMAGE_FORMAT srcFormat, I3G_IMAGE_FORMAT destFormat)
{
	HRESULT hr;
	UINT32 i, bits;
	UINT32 SrcLineSz, DestLineSz;

	// DDS 파일에서 각 MIPMAP Level Image의 Scan Line은 4 bytes align되어 있다.
	SrcLineSz = CalcRasterBufferSize(width, 1, 4, srcFormat);	// DWORD align

	DestLineSz = CalcRasterBufferSize(width, 1, 1, destFormat);

	bits = GetImageFormatBitCount(srcFormat);

	for (i = 0; i < height; i++)
	{
		ULONG cbValue;
		hr = pStream->Read(pSrc, SrcLineSz, &cbValue);

		switch (bits)
		{
		case 16:
			switch (destFormat)
			{
			case I3G_IMAGE_FORMAT_RGB_565:		i3Image_Convert16_X555_to_16_565((UINT16 *)pSrc, width, 1, 4, (UINT16 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_XRGB_1555:	i3Image_Convert16_X555_to_16_X555((UINT16 *)pSrc, width, 1, 4, (UINT16 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_ARGB_1555:	i3Image_Convert16_X555_to_16_1555((UINT16 *)pSrc, width, 1, 4, (UINT16 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_RGB_888:		i3Image_Convert16_X555_to_24_RGB((UINT16 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_BGR_888:		i3Image_Convert16_X555_to_24_BGR((UINT16 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_RGBX_8888:	i3Image_Convert16_X555_to_32_RGBA((UINT16 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_BGRX_8888:	i3Image_Convert16_X555_to_32_BGRA((UINT16 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_RGBA_8888:	i3Image_Convert16_X555_to_32_RGBA((UINT16 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_BGRA_8888:	i3Image_Convert16_X555_to_32_BGRA((UINT16 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			default:
				break;
			}
			break;

		case 24:
			switch (destFormat)
			{
			case I3G_IMAGE_FORMAT_RGB_565:		i3Image_Convert24_RGB_to_16_RGB_565((UINT8 *)pSrc, width, 1, 4, (UINT16 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_XRGB_1555:	i3Image_Convert24_RGB_to_16_ARGB_X555((UINT8 *)pSrc, width, 1, 4, (UINT16 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_ARGB_1555:	i3Image_Convert24_RGB_to_16_ARGB_1555((UINT8 *)pSrc, width, 1, 4, (UINT16 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_RGB_888:		i3Image_Convert24_RGB_to_24_RGB((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_BGR_888:		i3Image_Convert24_RGB_to_24_BGR((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_RGBX_8888:	i3Image_Convert24_RGB_to_32_BGRA((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_BGRX_8888:	i3Image_Convert24_RGB_to_32_RGBA((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_RGBA_8888:	i3Image_Convert24_RGB_to_32_BGRA((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_BGRA_8888:	i3Image_Convert24_RGB_to_32_RGBA((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			default:
				break;
			}
			break;

		default:			// 32 bits
			switch (destFormat)
			{
			case I3G_IMAGE_FORMAT_RGB_565:		i3Image_Convert32_RGBA_to_16_RGB_565((UINT8 *)pSrc, width, 1, 4, (UINT16 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_XRGB_1555:	i3Image_Convert32_RGBX_to_16_ARGB_1555((UINT8 *)pSrc, width, 1, 4, (UINT16 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_ARGB_1555:	i3Image_Convert32_RGBX_to_16_ARGB_1555((UINT8 *)pSrc, width, 1, 4, (UINT16 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_RGB_888:		i3Image_Convert32_RGBA_to_24_RGB((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_BGR_888:		i3Image_Convert32_RGBA_to_24_BGR((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_RGBX_8888:	i3Image_Convert32_RGBA_to_32_RGBA((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_BGRX_8888:	i3Image_Convert32_RGBX_to_32_BGRA((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_RGBA_8888:	i3Image_Convert32_RGBX_to_32_RGBA((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			case I3G_IMAGE_FORMAT_BGRA_8888:	i3Image_Convert32_RGBX_to_32_BGRA((UINT8 *)pSrc, width, 1, 4, (UINT8 *)pDest, 1);	break;
			default:
				break;
			}
			break;
		}

		pDest += DestLineSz;
	}

	return hr;
}