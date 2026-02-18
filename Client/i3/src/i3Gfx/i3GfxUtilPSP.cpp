#include "i3GfxType.h"
#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3Debug.h"
#include "i3GfxType.h"
#include "i3GfxDefine.h"
#include "i3VertexArray.h"
#include "i3GfxGlobalVariable.h"

#if defined( I3G_PSP)
#include "i3GfxDefinePSP.h"

//////////////////////////////////////////////////////////////////////////////
// Raster Format
UINT32	i3PSPUT_GetNativeImageFormat( I3G_IMAGE_FORMAT format, BOOL *bNotExact )
{
	#define SET_NOTEXACT { if( bNotExact ) *bNotExact = TRUE; }

	if( bNotExact ) *bNotExact = FALSE;

	switch( format & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB))
	{
	// 4 bit formats
	case I3G_IMAGE_FORMAT_I4		:	return SCEGU_PFIDX4;

	// 8bit formats
	case I3G_IMAGE_FORMAT_I8		:	return SCEGU_PFIDX8;
	case i3G_IMAGE_FORMAT_L8		:	SET_NOTEXACT;	return SCEGU_PFIDX8;
	case i3G_IMAGE_FORMAT_A8		:	SET_NOTEXACT;	return SCEGU_PFIDX8;

	// 16 bit formats
	case I3G_IMAGE_FORMAT_RGB_565	:	return SCEGU_PF5650;
	case I3G_IMAGE_FORMAT_XRGB_1555	:	SET_NOTEXACT;	return SCEGU_PF5551;
	case I3G_IMAGE_FORMAT_ARGB_1555	:	return SCEGU_PF5551;
	case I3G_IMAGE_FORMAT_XRGB_4444	:	SET_NOTEXACT;	return SCEGU_PF4444;
	case I3G_IMAGE_FORMAT_ARGB_4444	:	SET_NOTEXACT;	return SCEGU_PF4444;
	case I3G_IMAGE_FORMAT_ABGR_4444	:	return SCEGU_PF4444;

	// 24 bit formats
	case I3G_IMAGE_FORMAT_RGB_888	:	SET_NOTEXACT; return SCEGU_PF8888;
	case I3G_IMAGE_FORMAT_BGR_888	:	SET_NOTEXACT; return SCEGU_PF8888; // D3DFMT_B8G8R8;

	// 32 bit formats
	case I3G_IMAGE_FORMAT_RGBX_8888	:	return SCEGU_PF8888;
	case I3G_IMAGE_FORMAT_BGRX_8888	:	SET_NOTEXACT; return SCEGU_PF8888; // D3DFMT_X8B8G8R8;
	case I3G_IMAGE_FORMAT_RGBA_8888	:	return SCEGU_PF8888;
	case I3G_IMAGE_FORMAT_BGRA_8888	:	SET_NOTEXACT; return SCEGU_PF8888;

	case I3G_IMAGE_FORMAT_DXT1 :		return 8;
	case I3G_IMAGE_FORMAT_DXT3 :		return 9;
	case I3G_IMAGE_FORMAT_DXT5 :		return 10;

	default :
	#if defined( I3_DEBUG)
		i3Log( "i3GfxUtilDX", "[I3ERROR] Unsupported raster format : %d\n", format);
	#endif
		break;
	}

	return 0;
}

I3G_IMAGE_FORMAT	i3PSPUT_GetI3ImageFormat( UINT32 format)
{
	switch( format)
	{
		case SCEGU_PFIDX4 :		return I3G_IMAGE_FORMAT_I4;
		case SCEGU_PFIDX8:		return I3G_IMAGE_FORMAT_I8 ;
		case SCEGU_PF5551:		return I3G_IMAGE_FORMAT_ARGB_1555 ;
		case SCEGU_PF5650:		return I3G_IMAGE_FORMAT_RGB_565;
		case SCEGU_PF4444:		return I3G_IMAGE_FORMAT_ABGR_4444;
		case SCEGU_PF8888:		return I3G_IMAGE_FORMAT_RGBA_8888 ;	

		case 8 :				return I3G_IMAGE_FORMAT_DXT1;
		case 9 :				return I3G_IMAGE_FORMAT_DXT3;
		case 10 :				return I3G_IMAGE_FORMAT_DXT5;

		default :
		#if defined( I3_DEBUG)
			i3Log( "i3GfxUtilDX", "[I3ERROR] Unsupported raster format : %d\n", format);
		#endif
			break;
	}

	return I3G_IMAGE_FORMAT_NONE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Vertex Format

static UINT32 s_BlendWeightTable[] =
{
	0,
	SCEGU_WEIGHT_1,
	SCEGU_WEIGHT_2,
	SCEGU_WEIGHT_3,
	SCEGU_WEIGHT_4,
	SCEGU_WEIGHT_5,
	SCEGU_WEIGHT_6,
	SCEGU_WEIGHT_7,
	SCEGU_WEIGHT_8,
};

UINT32 i3PSPUT_GetNativeVertexFormat( i3VertexFormat * pFormat)
{
	UINT32 Format = 0;

	if( pFormat->GetHasPosition())
	{
		switch( pFormat->GetPositionType())
		{
			case I3VF_TYPE_REAL32 :			Format |= SCEGU_VERTEX_FLOAT;	break;
			case I3VF_TYPE_FIXED16_0 :		Format |= SCEGU_VERTEX_SHORT;	break;
			case I3VF_TYPE_FIXED16_15 :		Format |= SCEGU_VERTEX_SHORT;	break;
			case I3VF_TYPE_FIXED8_0 :		Format |= SCEGU_VERTEX_BYTE;	break;
			case I3VF_TYPE_FIXED8_7 :		Format |= SCEGU_VERTEX_BYTE;	break;
			default :
				i3Log( "i3PSPUT_GetNativeVertexFormat()", "Invalid Position Format : %d\n", pFormat->GetPositionType());
				break;
		}
	}

	if( pFormat->GetHasNormal())
	{
		switch( pFormat->GetNormalType())
		{
			case I3VF_TYPE_REAL32 :			Format |= SCEGU_NORMAL_FLOAT;	break;			
			case I3VF_TYPE_FIXED16_15 :		Format |= SCEGU_NORMAL_SHORT;	break;			
			case I3VF_TYPE_FIXED8_7 :		Format |= SCEGU_NORMAL_BYTE;	break;
			default :
				i3Log( "i3PSPUR_GetNativeVertexFormat()", "Invalid Normal Format : %d\n", pFormat->GetNormalType());
				break;
		}
	}

	if( pFormat->GetHasColor())
	{
		switch( pFormat->GetColorType())
		{
			case I3VF_TYPE_COLOR32_RGBA_8888 :	Format |= SCEGU_COLOR_PF8888;	break;
			case I3VF_TYPE_COLOR16_RGBA_1555 :	Format |= SCEGU_COLOR_PF5551;	break;
			case I3VF_TYPE_COLOR16_RGBX_565 :	Format |= SCEGU_COLOR_PF5650;	break;
			case I3VF_TYPE_COLOR16_RGBA_4444 :	Format |= SCEGU_COLOR_PF4444;	break;
			default :
				i3Log( "i3PSPUT_GetNativeVertexFormat()", "Invalid Color Format : %d\n", pFormat->GetColorType());
				break;
		}
	}

	if( pFormat->GetTextureCoordSetCount() > 0)
	{
		switch( pFormat->GetTextureCoordType())
		{
			case I3VF_TYPE_REAL32 :				Format |= SCEGU_TEXTURE_FLOAT;		break;
			case I3VF_TYPE_FIXED16_0 :			Format |= SCEGU_TEXTURE_USHORT;		break;
			case I3VF_TYPE_FIXED16_15 :			Format |= SCEGU_TEXTURE_USHORT;		break;
			case I3VF_TYPE_FIXED16_15_U :		Format |= SCEGU_TEXTURE_USHORT;		break;
			case I3VF_TYPE_FIXED8_0 :			Format |= SCEGU_TEXTURE_UBYTE;		break;
			case I3VF_TYPE_FIXED8_7 :			Format |= SCEGU_TEXTURE_UBYTE;		break;
			case I3VF_TYPE_FIXED8_7_U :			Format |= SCEGU_TEXTURE_UBYTE;		break;
			default :
				i3Log( "i3PSPUT_GetNativeVertexFormat()", "Invalid Texture Coord. format : %d\n", pFormat->GetTextureCoordType());
				break;
		}
	}

	if( pFormat->GetBlendWeightCount() > 0)
	{
		I3ASSERT( pFormat->GetBlendWeightCount() <= 8);

		switch( pFormat->GetBlendWeightType())
		{
			case I3VF_TYPE_REAL32 :				Format |= SCEGU_WEIGHT_FLOAT;		break;
			case I3VF_TYPE_FIXED16_15 :			Format |= SCEGU_WEIGHT_USHORT;		break;
			case I3VF_TYPE_FIXED8_7 :			Format |= SCEGU_WEIGHT_UBYTE;		break;
			default :
				i3Log( "i3PSPUT_GetNativeVertexFormat()", "Invalid Blend weight format : %d\n", pFormat->GetBlendWeightType());
				break;
		}

		Format |= s_BlendWeightTable[ (int)pFormat->GetBlendWeightCount()];
	}

	if( pFormat->IsTransformed())
	{
		Format |= SCEGU_THROUGH;
	}

	return Format;
}

#endif
