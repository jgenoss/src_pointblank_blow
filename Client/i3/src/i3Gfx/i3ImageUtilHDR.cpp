#include "i3GfxType.h"
#include "i3Texture.h"

#define		CALC_PARAM( srcBitCount, destBitCount)	\
	{\
		INT32 sp;\
		\
		/* Source Image의 1 line의 길이(bytes)를 구한다. */ \
		srcLineSz = (((width * srcBitCount) + 7) >> 3);\
		\
		I3ASSERT( srcLineAlign != 0 );\
		\
		/* align을 반영한 길이를 구한다. */ \
		sp = ((srcLineSz + srcLineAlign - 1) / srcLineAlign) * srcLineAlign;\
		\
		/* 1 line 마다 alignment에 의해 추가되어야 하는 padding byte의 개수 */ \
		srcLineGap = sp - srcLineSz;\
		\
		/* Dest. image의 1 line의 길이를 구한다. */ \
		destLineSz = (((width * destBitCount) + 7) >> 3); \
		\
		I3ASSERT( destLineAlign != 0 );\
		\
		/* align을 반영한 길이를 구한다. */ \
		sp = ((destLineSz + destLineAlign - 1) / destLineAlign) * destLineAlign;\
		\
		/* 1 line 마다 alignment에 의해 추가되어야 하는 padding byte의 개수 */ \
		destLineGap = sp - destLineSz;\
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// HDR
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// I3G_IMAGE_FORMAT_ABGR_16F Convert
	// Convert 16F_ABGR to 16 Bit Format
	void i3Image_Convert16F_ABGR_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert16F_ABGR_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert16F_ABGR_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert16F_ABGR_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert16F_ABGR_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 16F_ABGR to 24 Bit Format
	void i3Image_Convert16F_ABGR_to_24_RGB_888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert16F_ABGR_to_24_BGR_8884( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 16F_ABGR to 32 Bit Format
	void i3Image_Convert16F_ABGR_to_32_RGBX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert16F_ABGR_to_32_BGRX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert16F_ABGR_to_32_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert16F_ABGR_to_32_BGRA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

// I3G_IMAGE_FORMAT_ABGR_32F
	// Convert 32F_ABGR to 16 Bit Format
	void i3Image_Convert32F_ABGR_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert32F_ABGR_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert32F_ABGR_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert32F_ABGR_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_ABGR_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 32F_ABGR to 24 Bit Format
	void i3Image_Convert32F_ABGR_to_24_RGB_888( REAL32 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
	{
		INT32 srcLineSz, destLineSz;
		INT32 srcLineGap, destLineGap;
		INT32 i, j;

		CALC_PARAM( 64, 24);

		for( i = 0; i < height; i++)
		{
			for( j = 0; j < width; j++)
			{
				*pDest = (UINT8)((*pSrc)*255);
				pDest++;
				pSrc++;

				*pDest = (UINT8)((*pSrc)*255);
				pDest++;
				pSrc++;

				*pDest = (UINT8)((*pSrc)*255);
				pDest++;
				pSrc++;

				pSrc++;
			}

			pSrc += srcLineGap;
			pDest += destLineGap;
		}
	}
	
	void i3Image_Convert32F_ABGR_to_24_BGR_8884( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 32F_ABGR to 32 Bit Format
	void i3Image_Convert32F_ABGR_to_32_RGBX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert32F_ABGR_to_32_BGRX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert32F_ABGR_to_32_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert32F_ABGR_to_32_BGRA_8888( REAL32 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT8 * pDest, INT32 destLineAlign)
	{
		INT32 srcLineSz, destLineSz;
		INT32 srcLineGap, destLineGap;
		INT32 i, j;

		CALC_PARAM( 64, 32);

		for( i = 0; i < height; i++)
		{
			for( j = 0; j < width; j++)
			{
				*pDest = (UINT8)((*pSrc)*255);
				pDest++;
				pSrc++;

				*pDest = (UINT8)((*pSrc)*255);
				pDest++;
				pSrc++;

				*pDest = (UINT8)((*pSrc)*255);
				pDest++;
				pSrc++;

				//*pDest = (UINT8)((*pSrc)*255);
				*pDest = (UINT8)255;
				pDest++;
				pSrc++;
			}

			pSrc += srcLineGap;
			pDest += destLineGap;
		}
	}

// I3G_IMAGE_FORMAT_ABGR_16
	// Convert 16 ABGR to 16 Bit Format
	void i3Image_Convert16_ABGR_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert16_ABGR_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert16_ABGR_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert16_ABGR_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert16_ABGR_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 16 ABGR to 24 Bit Format
	void i3Image_Convert16_ABGR_to_24_RGB_888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert16_ABGR_to_24_BGR_8884( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 16 ABGR to 32 Bit Format
	void i3Image_Convert16_ABGR_to_32_RGBX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert16_ABGR_to_32_BGRX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert16_ABGR_to_32_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert16_ABGR_to_32_BGRA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

// I3G_IMAGE_FORMAT_R_32F
	// Convert 32F_R to 16 Bit Format
	void i3Image_Convert32F_R_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}
	
	void i3Image_Convert32F_R_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_R_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_R_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_R_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 32F_R to 24 Bit Format
	void i3Image_Convert32F_R_to_24_RGB_888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_R_to_24_BGR_8884( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 32F_R to 32 Bit Format
	void i3Image_Convert32F_R_to_32_RGBX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_R_to_32_BGRX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_R_to_32_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_R_to_32_BGRA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	I3_EXPORT_GFX
	void	i3PixelGet_R_32F( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
	{
		REAL32 * pSrc;
		INT32 lv =  pTex->getLockedLevel();

		pSrc = (REAL32 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

		pCol->r = *pSrc;
		pCol->g = 0.0f;
		pCol->b = 0.0f;
		pCol->a = 1.0f;
	}

	I3_EXPORT_GFX
	void	i3PixelSet_R_32F( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
	{
		REAL32 * pDest;
		INT32 lv =  pTex->getLockedLevel();

		pDest = (REAL32 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

		*pDest = pCol->r;
	}


// I3G_IMAGE_FORMAT_GR_32F
	// Convert 32F_GR to 16 Bit Format
	void i3Image_Convert32F_GR_to_16_RGB_565( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_GR_to_16_XRGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_GR_to_16_ARGB_1555( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_GR_to_16_ARGB_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_GR_to_16_ABGR_4444( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 32F_GR to 24 Bit Format
	void i3Image_Convert32F_GR_to_24_RGB_888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_GR_to_24_BGR_8884( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	// Convert 32F_GR to 32 Bit Format
	void i3Image_Convert32F_GR_to_32_RGBX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_GR_to_32_BGRX_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_GR_to_32_RGBA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	void i3Image_Convert32F_GR_to_32_BGRA_8888( UINT8 * pSrc, INT32 width, INT32 height, INT32 srcLineAlign, UINT16 * pDest, INT32 destLineAlign)
	{

	}

	I3_EXPORT_GFX
	void	i3PixelGet_GR_32F( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
	{
		REAL32 * pSrc;
		INT32 lv =  pTex->getLockedLevel();

		pSrc = (REAL32 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

		pCol->r = pSrc[0];
		pCol->g = pSrc[1];
		pCol->b = 0.0f;
		pCol->a = 1.0f;
	}

	I3_EXPORT_GFX
	void	i3PixelSet_GR_32F( i3Texture * pTex, INT32 x, INT32 y, COLORREAL * pCol)
	{
		REAL32 * pDest;
		INT32 lv =  pTex->getLockedLevel();

		pDest = (REAL32 *) (pTex->GetLockedBuffer( lv) + (y * pTex->GetPitch( lv)) + (x * pTex->GetStride( lv)));

		pDest[0] = pCol->r;
		pDest[1] = pCol->g;
	}
