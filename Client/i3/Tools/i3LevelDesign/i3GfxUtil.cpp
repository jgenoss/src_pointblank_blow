#include "i3GfxType.h"
#include "i3GfxDefine.h"
#include "i3GfxUtil.h"

INT32 i3Gfx::GetImageFormatBitCount( I3G_IMAGE_FORMAT format)
{
	format = I3G_IMAGE_FORMAT(format & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB));

	switch( format)
	{
	// 4 bit formats
	case I3G_IMAGE_FORMAT_I4:	return 4;

	// 8bit formats
	case I3G_IMAGE_FORMAT_I8:
	case i3G_IMAGE_FORMAT_L8:
	case i3G_IMAGE_FORMAT_A8:	return 8;

	// 16 bit formats
	case I3G_IMAGE_FORMAT_RGB_565:
	case I3G_IMAGE_FORMAT_XRGB_1555:
	case I3G_IMAGE_FORMAT_ARGB_1555:
	case I3G_IMAGE_FORMAT_ABGR_4444:
	case I3G_IMAGE_FORMAT_XBGR_4444:
	case I3G_IMAGE_FORMAT_XRGB_4444:
	case I3G_IMAGE_FORMAT_ARGB_4444:	return 16;

	// 24 bit formats
	case I3G_IMAGE_FORMAT_RGB_888:
	case I3G_IMAGE_FORMAT_BGR_888:	return 24;

	// 32 bit format
	case I3G_IMAGE_FORMAT_RGBX_8888:
	case I3G_IMAGE_FORMAT_BGRX_8888:
	case I3G_IMAGE_FORMAT_RGBA_8888:
	case I3G_IMAGE_FORMAT_BGRA_8888:	return 32;

	// -------------------------
	// Bump-map
	// -------------------------
	case I3G_IMAGE_FORMAT_VU_88:	return 16;
	case I3G_IMAGE_FORMAT_QWVU_8888: return 32;

	// -------------------------
	// Compressed formats
	// -------------------------
	case I3G_IMAGE_FORMAT_DXT1_X:
	case I3G_IMAGE_FORMAT_DXT1:		return 4;

	case I3G_IMAGE_FORMAT_DXT2:
	case I3G_IMAGE_FORMAT_DXT3:
	case I3G_IMAGE_FORMAT_DXT4:
	case I3G_IMAGE_FORMAT_DXT5:		return 8;

	// -------------------------
	// HDR
	// -------------------------
	case I3G_IMAGE_FORMAT_ABGR_16F :	return 64;
	case I3G_IMAGE_FORMAT_ABGR_32F :	return 128;
	case I3G_IMAGE_FORMAT_ABGR_16 :		return 64;
	case I3G_IMAGE_FORMAT_GR_16F :		return 32;
	case I3G_IMAGE_FORMAT_R_32F :		return 32;
	case I3G_IMAGE_FORMAT_GR_32F :		64;

	// -------------------------
	// depth, stencil, vertex buffers
	// -------------------------
	case I3G_IMAGE_FORMAT_D16:	return 16;

	case I3G_IMAGE_FORMAT_D24S8:
	case I3G_IMAGE_FORMAT_D24X8:
	case I3G_IMAGE_FORMAT_D32F:
	case I3G_IMAGE_FORMAT_D24FS8 :
	case I3G_IMAGE_FORMAT_D32:		return 32;

	default :
		I3TRACE1( "i3Gfx::GetImageFormatBitCount() - Invalid Format : %x\n", format);
		return -1;
	}
}

const char * i3Gfx::GetImageFormatName( I3G_IMAGE_FORMAT format)
{
	format = I3G_IMAGE_FORMAT(format & ~(I3G_IMAGE_FORMAT_MASK_PLATFORM | I3G_IMAGE_FORMAT_MASK_ZLIB));

	switch( format)
	{
		// 4 bit formats
		case I3G_IMAGE_FORMAT_I4:	return "IDX4";

		// 8bit formats
		case I3G_IMAGE_FORMAT_I8:	return "IDX8";
		case i3G_IMAGE_FORMAT_L8:	return "LUX8";
		case i3G_IMAGE_FORMAT_A8:	return "ALPHA8";

		// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565:		return "RGB16_565";
		case I3G_IMAGE_FORMAT_XRGB_1555:	return "XRGB16_1555";
		case I3G_IMAGE_FORMAT_ARGB_1555:	return "ARGB16_1555";
		case I3G_IMAGE_FORMAT_ABGR_4444:	return "RGBA16_4444";
		case I3G_IMAGE_FORMAT_XBGR_4444:	return "RGBX16_4444";
		case I3G_IMAGE_FORMAT_XRGB_4444:	return "XRGB16_4444";
		case I3G_IMAGE_FORMAT_ARGB_4444:	return "ARGB16_4444";

		// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888:		return "RGB24_888";
		case I3G_IMAGE_FORMAT_BGR_888:		return "BGR24_888";

		// 32 bit format
		case I3G_IMAGE_FORMAT_RGBX_8888:	return "RGBX32_8888";
		case I3G_IMAGE_FORMAT_BGRX_8888:	return "XRGB32_8888";
		case I3G_IMAGE_FORMAT_RGBA_8888:	return "RGBA32_8888";
		case I3G_IMAGE_FORMAT_BGRA_8888:	return "ARGB32_8888";

		// -------------------------
		// Bump-map
		// -------------------------
		case I3G_IMAGE_FORMAT_VU_88:		return "VU_88";
		case I3G_IMAGE_FORMAT_QWVU_8888:	return "QWVU_8888";

		// -------------------------
		// Compressed formats
		// -------------------------
		case I3G_IMAGE_FORMAT_DXT1_X:		return "DXT1_X";
		case I3G_IMAGE_FORMAT_DXT1:			return "DXT1";
		case I3G_IMAGE_FORMAT_DXT2:			return "DXT2";
		case I3G_IMAGE_FORMAT_DXT3:			return "DXT3";
		case I3G_IMAGE_FORMAT_DXT4:			return "DXT4";
		case I3G_IMAGE_FORMAT_DXT5:			return "DXT5";

		// -------------------------
		// HDR
		// -------------------------
		case I3G_IMAGE_FORMAT_ABGR_16F :	return "RGBA64_16F";
		case I3G_IMAGE_FORMAT_ABGR_32F :	return "RGBA128_32F";
		case I3G_IMAGE_FORMAT_ABGR_16 :		return "RGBA64_16";
		case I3G_IMAGE_FORMAT_GR_16F :		return "GR32_16F";
		case I3G_IMAGE_FORMAT_R_32F :		return "R32_32F";
		case I3G_IMAGE_FORMAT_GR_32F :		return "GR64_32F";

		// -------------------------
		// depth, stencil, vertex buffers
		// -------------------------
		case I3G_IMAGE_FORMAT_D16:			return "D16";

		case I3G_IMAGE_FORMAT_D24S8:		return "D24S8";
		case I3G_IMAGE_FORMAT_D24X8:		return "D24X8";
		case I3G_IMAGE_FORMAT_D32F:			return "D32F";
		case I3G_IMAGE_FORMAT_D24FS8 :		return "D24FS8";
		case I3G_IMAGE_FORMAT_D32:			return "D32";

		default :							return "(Unknown)";
	}
}

// 각 Raster Format들에 대해, 버퍼 사이즈를 Byte 단위로 리턴
INT32 i3Gfx::CalcRasterBufferSize( INT32 width, INT32 height, INT32 alignbits, I3G_IMAGE_FORMAT format)
{
	INT32 bits, sp, Result;

	if( format & I3G_IMAGE_FORMAT_MASK_COMPRESSED)
	{
		switch( format)
		{
			case I3G_IMAGE_FORMAT_DXT1_X:
			case I3G_IMAGE_FORMAT_DXT1:		bits = 4;		break;

			case I3G_IMAGE_FORMAT_DXT2:
			case I3G_IMAGE_FORMAT_DXT3:
			case I3G_IMAGE_FORMAT_DXT4:
			case I3G_IMAGE_FORMAT_DXT5:		bits = 8;		break;
			default :
				i3Log( "i3Gfx::CalcRasterBufferSize()", "Unrecognized format : %08X\n", format);
				return -1;
		}

		Result = ((bits * width) >> 3) * height;
	}
	else
	{
		// Uncompressed Format
		bits = GetImageFormatBitCount( format);

		bits *= width;

		sp = (bits + alignbits - 1) >> alignbits;

		sp = (sp << alignbits) >> 3;		// byte 단위

		Result = sp * height;
	}

	return Result;
}

UINT32	i3Gfx::CalcVertexCount( I3G_PRIMITIVE Prim, UINT32 PrimCount)
{
	switch( Prim)
	{
		case I3G_PRIM_LINELIST :			return (PrimCount << 1);

		case I3G_PRIM_LINESTRIP :			return PrimCount + 1;

		case I3G_PRIM_TRILIST :				return PrimCount * 3;

		case I3G_PRIM_TRISTRIP :
		case I3G_PRIM_TRIFAN :				return PrimCount + 2;

		#if defined( I3_COMPILER_GCC)
		default :							return PrimCount;
		#endif
	}

	return PrimCount;
}

UINT32	i3Gfx::CalcPrimCount( I3G_PRIMITIVE Prim, UINT32 VertexCount)
{
	switch( Prim)
	{
		case I3G_PRIM_LINELIST :			return VertexCount >> 1;

		case I3G_PRIM_LINESTRIP :			return VertexCount - 1;

		case I3G_PRIM_TRILIST :				return VertexCount / 3;

		case I3G_PRIM_TRISTRIP :
		case I3G_PRIM_TRIFAN :				return VertexCount - 2;

		default :							return VertexCount;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Usage Conversion
I3G_PERSIST_USAGE i3Gfx::Convert( I3G_USAGE usage)
{
	switch( usage)
	{
		case I3G_USAGE_FREQUENT_UPDATE :					return I3G_PERSIST_USAGE_FREQUENT_UPDATE;
		#ifdef I3G_USAGE_SOFTWAREPROCESSING
		case I3G_USAGE_SOFTWAREPROCESSING :			return I3G_PERSIST_USAGE_SOFTWAREPROCESSING;
		#endif
		case I3G_USAGE_WRITEONLY :					return I3G_PERSIST_USAGE_WRITEONLY;
		case I3G_USAGE_POINT_SPRITES :				return I3G_PERSIST_USAGE_POINT_SPRITES;
	}

	return I3G_PERSIST_USAGE_NONE;
}

I3G_USAGE i3Gfx::Convert( I3G_PERSIST_USAGE usage)
{
	switch( usage)
	{
		case I3G_PERSIST_USAGE_FREQUENT_UPDATE :		return I3G_USAGE_FREQUENT_UPDATE;
		#ifdef I3G_USAGE_SOFTWAREPROCESSING
		case I3G_PERSIST_USAGE_SOFTWAREPROCESSING :		return I3G_USAGE_SOFTWAREPROCESSING;
		#endif
		case I3G_PERSIST_USAGE_WRITEONLY :				return I3G_USAGE_WRITEONLY;
		case I3G_PERSIST_USAGE_POINT_SPRITES :			return I3G_USAGE_POINT_SPRITES;

		default :
			return I3G_USAGE_NONE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Render mode Conversion
I3G_PERSIST_RENDER_MODE i3Gfx::Convert( I3G_RENDER_MODE mode)
{
	switch( mode)
	{
		case I3G_RENDER_POINT :		return I3G_PERSIST_RENDER_MODE_POINT;
		case I3G_RENDER_LINE :		return I3G_PERSIST_RENDER_MODE_LINE;
		default :					return I3G_PERSIST_RENDER_MODE_POLYGON;
	}
}

I3G_RENDER_MODE	i3Gfx::Convert( I3G_PERSIST_RENDER_MODE mode)
{
	switch( mode)
	{
		case I3G_PERSIST_RENDER_MODE_POINT :	return I3G_RENDER_POINT;
		case I3G_PERSIST_RENDER_MODE_LINE :		return I3G_RENDER_LINE;
		default :								return I3G_RENDER_POLYGON;
	} 
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Shade Mode Conversion
I3G_PERSIST_SHADE_MODE i3Gfx::Convert( I3G_SHADE_MODE mode)
{
	switch( mode)
	{
		case I3G_SHADE_FLAT :		return I3G_PERSIST_SHADE_MODE_FLAT;
		case I3G_SHADE_PHONG :		return I3G_PERSIST_SHADE_MODE_PHONG;
		case I3G_SHADE_PPL :		return I3G_PERSIST_SHADE_MODE_PPL;
		case I3G_SHADE_LUXMAP :		return I3G_PERSIST_SHADE_MODE_LUXMAP;
		default :					return I3G_PERSIST_SHADE_MODE_GOURAUD;
	}
}

I3G_SHADE_MODE i3Gfx::Convert( I3G_PERSIST_SHADE_MODE mode)
{
	switch( mode)
	{
		case I3G_PERSIST_SHADE_MODE_FLAT :		return I3G_SHADE_FLAT;
		case I3G_PERSIST_SHADE_MODE_PHONG :		return I3G_SHADE_PHONG;
		case I3G_PERSIST_SHADE_MODE_PPL :		return I3G_SHADE_PPL;
		case I3G_PERSIST_SHADE_MODE_LUXMAP :	return I3G_SHADE_LUXMAP;
		default :								return I3G_SHADE_GOURAUD;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Face Cull mode Conversion
I3G_PERSIST_FACE_CULL i3Gfx::Convert( I3G_FACE_CULL mode)
{
	switch( mode)
	{
		case I3G_FACE_CULL_BACK		: return I3G_PERSIST_FACE_CULL_BACK;
		case I3G_FACE_CULL_FRONT	: return I3G_PERSIST_FACE_CULL_FRONT;
		default 					: return I3G_PERSIST_FACE_CULL_NONE;
	}
}

I3G_FACE_CULL i3Gfx::Convert( I3G_PERSIST_FACE_CULL mode)
{
	switch( mode)
	{
		case I3G_PERSIST_FACE_CULL_BACK		: return I3G_FACE_CULL_BACK;
		case I3G_PERSIST_FACE_CULL_FRONT	: return I3G_FACE_CULL_FRONT;
		default								: return I3G_FACE_CULL_NONE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Blend Conversion
I3G_PERSIST_BLEND i3Gfx::Convert( I3G_BLEND blend)
{
	switch( blend)
	{
		case I3G_BLEND_ONE			: return I3G_PERSIST_BLEND_ONE;
		case I3G_BLEND_SRCCOLOR		: return I3G_PERSIST_BLEND_SRCCOLOR;
		case I3G_BLEND_INVSRCCOLOR	: return I3G_PERSIST_BLEND_INVSRCCOLOR;
		case I3G_BLEND_SRCALPHA		: return I3G_PERSIST_BLEND_SRCALPHA;
		case I3G_BLEND_INVSRCALPHA	: return I3G_PERSIST_BLEND_INVSRCALPHA;
		case I3G_BLEND_DESTALPHA	: return I3G_PERSIST_BLEND_DESTALPHA;
		case I3G_BLEND_INVDESTALPHA	: return I3G_PERSIST_BLEND_INVDESTALPHA;

		#if !defined( I3G_PSP)
		case I3G_BLEND_DESTCOLOR	: return I3G_PERSIST_BLEND_DESTCOLOR;
		case I3G_BLEND_INVDESTCOLOR	: return I3G_PERSIST_BLEND_INVDESTCOLOR;

		#if !defined( I3G_OGLES)
		case I3G_BLEND_SRCALPHASAT	: return I3G_PERSIST_BLEND_SRCALPHASAT;
		#endif
		#endif

		default						: return I3G_PERSIST_BLEND_ZERO;
	}
}

I3G_BLEND i3Gfx::Convert( I3G_PERSIST_BLEND blend)
{
	switch( blend)
	{
		case I3G_PERSIST_BLEND_ONE			: return I3G_BLEND_ONE			;
		case I3G_PERSIST_BLEND_SRCCOLOR		: return I3G_BLEND_SRCCOLOR		;
		case I3G_PERSIST_BLEND_INVSRCCOLOR	: return I3G_BLEND_INVSRCCOLOR	;
		case I3G_PERSIST_BLEND_SRCALPHA		: return I3G_BLEND_SRCALPHA		;
		case I3G_PERSIST_BLEND_INVSRCALPHA	: return I3G_BLEND_INVSRCALPHA	;
		case I3G_PERSIST_BLEND_DESTALPHA	: return I3G_BLEND_DESTALPHA	;
		case I3G_PERSIST_BLEND_INVDESTALPHA	: return I3G_BLEND_INVDESTALPHA	;
		case I3G_PERSIST_BLEND_DESTCOLOR	: return I3G_BLEND_DESTCOLOR	;
		case I3G_PERSIST_BLEND_INVDESTCOLOR	: return I3G_BLEND_INVDESTCOLOR	;
		case I3G_PERSIST_BLEND_SRCALPHASAT	: return I3G_BLEND_SRCALPHASAT	;

		default								: return I3G_BLEND_ZERO;
	}
}

I3G_BLEND_OP i3Gfx::Convert( I3G_PERSIST_BLEND_OP blend)
{
	switch( blend)
	{
		case I3G_PERSIST_BLEND_OP_ADD			:	return I3G_BLEND_OP_ADD		;
		case I3G_PERSIST_BLEND_OP_SUBTRACT		:	return I3G_BLEND_OP_SUBTRACT	;
		case I3G_PERSIST_BLEND_OP_REVSUBTRACT	:	return I3G_BLEND_OP_REVSUBTRACT;
		case I3G_PERSIST_BLEND_OP_MIN			:	return I3G_BLEND_OP_MIN		;
		case I3G_PERSIST_BLEND_OP_MAX			:	return I3G_BLEND_OP_MAX;
		default									:	return I3G_BLEND_OP_NONE;
	}
}

I3G_PERSIST_BLEND_OP i3Gfx::Convert( I3G_BLEND_OP blend)
{
	switch( blend)
	{
		case I3G_BLEND_OP_ADD			:	return I3G_PERSIST_BLEND_OP_ADD		;
		case I3G_BLEND_OP_SUBTRACT		:	return I3G_PERSIST_BLEND_OP_SUBTRACT	;
		case I3G_BLEND_OP_REVSUBTRACT	:	return I3G_PERSIST_BLEND_OP_REVSUBTRACT;
		case I3G_BLEND_OP_MIN			:	return I3G_PERSIST_BLEND_OP_MIN		;
		case I3G_BLEND_OP_MAX			:	return I3G_PERSIST_BLEND_OP_MAX;
		default							:	return I3G_PERSIST_BLEND_OP_NONE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Fog mode Conversion
I3G_PERSIST_FOG_MODE	i3Gfx::Convert( I3G_FOG_MODE mode)
{
	switch( mode)
	{
		case I3G_FOG_LINEAR :	return I3G_PERSIST_FOG_LINEAR;
		case I3G_FOG_EXP :		return I3G_PERSIST_FOG_EXP;
		case I3G_FOG_EXP2 :		return I3G_PERSIST_FOG_EXP2;
		default				:	return I3G_PERSIST_FOG_NONE;
	}
}

I3G_FOG_MODE	i3Gfx::Convert( I3G_PERSIST_FOG_MODE mode)
{
	switch( mode)
	{
		case I3G_PERSIST_FOG_LINEAR :	return I3G_FOG_LINEAR;
		case I3G_PERSIST_FOG_EXP :		return I3G_FOG_EXP;
		case I3G_PERSIST_FOG_EXP2 :		return I3G_FOG_EXP2;
		default						:	return I3G_FOG_NONE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Stencil Operation Conversion
I3G_PERSIST_STENCIL_OP i3Gfx::Convert( I3G_STENCIL_OP op)
{
	switch( op)
	{
		case I3G_STENCIL_OP_ZERO		: return I3G_PERSIST_STENCIL_OP_ZERO;
		case I3G_STENCIL_OP_REPLACE		: return I3G_PERSIST_STENCIL_OP_REPLACE;
		case I3G_STENCIL_OP_INC_CLAMP	: return I3G_PERSIST_STENCIL_OP_INC_CLAMP;
		case I3G_STENCIL_OP_DEC_CLAMP	: return I3G_PERSIST_STENCIL_OP_DEC_CLAMP;
		case I3G_STENCIL_OP_INVERT		: return I3G_PERSIST_STENCIL_OP_INVERT;

		#if defined( I3G_DX) || defined( I3G_XBOX)
			case I3G_STENCIL_OP_INC			: return I3G_PERSIST_STENCIL_OP_INC;
			case I3G_STENCIL_OP_DEC			: return I3G_PERSIST_STENCIL_OP_DEC;
		#endif

		default							: return I3G_PERSIST_STENCIL_OP_KEEP;
	}
}

I3G_STENCIL_OP i3Gfx::Convert( I3G_PERSIST_STENCIL_OP op)
{
	switch( op)
	{
		case I3G_PERSIST_STENCIL_OP_ZERO		: return I3G_STENCIL_OP_ZERO;
		case I3G_PERSIST_STENCIL_OP_REPLACE		: return I3G_STENCIL_OP_REPLACE;
		case I3G_PERSIST_STENCIL_OP_INC_CLAMP	: return I3G_STENCIL_OP_INC_CLAMP;
		case I3G_PERSIST_STENCIL_OP_DEC_CLAMP	: return I3G_STENCIL_OP_DEC_CLAMP;
		case I3G_PERSIST_STENCIL_OP_INVERT		: return I3G_STENCIL_OP_INVERT;
		case I3G_PERSIST_STENCIL_OP_INC			: return I3G_STENCIL_OP_INC;
		case I3G_PERSIST_STENCIL_OP_DEC			: return I3G_STENCIL_OP_DEC;
		default									: return I3G_STENCIL_OP_KEEP;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Compare Function Conversion
I3G_PERSIST_COMP_FUNC i3Gfx::Convert( I3G_COMP_FUNC func)
{
	switch( func)
	{
		case I3G_COMP_LESS		: return I3G_PERSIST_COMP_LESS;
		case I3G_COMP_EQUAL		: return I3G_PERSIST_COMP_EQUAL;
		case I3G_COMP_LEQUAL	: return I3G_PERSIST_COMP_LEQUAL;
		case I3G_COMP_GREATER	: return I3G_PERSIST_COMP_GREATER;
		case I3G_COMP_NOTEQUAL	: return I3G_PERSIST_COMP_NOTEQUAL;
		case I3G_COMP_GEQUAL	: return I3G_PERSIST_COMP_GEQUAL;
		case I3G_COMP_ALWAYS	: return I3G_PERSIST_COMP_ALWAYS;
		default					: return I3G_PERSIST_COMP_NEVER;
	}
}

I3G_COMP_FUNC i3Gfx::Convert( I3G_PERSIST_COMP_FUNC func)
{
	switch( func)
	{
		case I3G_PERSIST_COMP_LESS		: return I3G_COMP_LESS;
		case I3G_PERSIST_COMP_EQUAL		: return I3G_COMP_EQUAL;
		case I3G_PERSIST_COMP_LEQUAL	: return I3G_COMP_LEQUAL;
		case I3G_PERSIST_COMP_GREATER	: return I3G_COMP_GREATER;
		case I3G_PERSIST_COMP_NOTEQUAL	: return I3G_COMP_NOTEQUAL;
		case I3G_PERSIST_COMP_GEQUAL	: return I3G_COMP_GEQUAL;
		case I3G_PERSIST_COMP_ALWAYS	: return I3G_COMP_ALWAYS;
		default							: return I3G_COMP_NEVER;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Texture Wrap mode Conversion
I3G_PERSIST_TEXTURE_WRAP i3Gfx::Convert( I3G_TEXTURE_WRAP mode)
{
	switch( mode)
	{
		case I3G_TEXTURE_WRAP_WRAP :	return I3G_PERSIST_TEXTURE_WRAP_WRAP;
		default						:	return I3G_PERSIST_TEXTURE_WRAP_CLAMP;
	}
}

I3G_TEXTURE_WRAP i3Gfx::Convert( I3G_PERSIST_TEXTURE_WRAP mode)
{
	switch( mode)
	{
		case I3G_PERSIST_TEXTURE_WRAP_WRAP :	return I3G_TEXTURE_WRAP_WRAP;
		default								:	return I3G_TEXTURE_WRAP_CLAMP;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Texture Filter mode Conversion
I3G_PERSIST_TEXTURE_FILTER i3Gfx::Convert( I3G_TEXTURE_FILTER mode)
{
	switch( mode)
	{
		case I3G_TEXTURE_FILTER_POINT					: return I3G_PERSIST_TEXTURE_FILTER_POINT					;
		case I3G_TEXTURE_FILTER_LINEAR					: return I3G_PERSIST_TEXTURE_FILTER_LINEAR					;
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT		: return I3G_PERSIST_TEXTURE_FILTER_POINT_MIPMAP_POINT		;
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR		: return I3G_PERSIST_TEXTURE_FILTER_POINT_MIPMAP_LINEAR		;
		case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT		: return I3G_PERSIST_TEXTURE_FILTER_LINEAR_MIPMAP_POINT		;
		case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR	: return I3G_PERSIST_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR	;
		default											: return I3G_PERSIST_TEXTURE_FILTER_NONE;
	}

#if !defined( I3_COMPILER_VC)
	return I3G_PERSIST_TEXTURE_FILTER_NONE;
#endif
}

I3G_TEXTURE_FILTER i3Gfx::Convert( I3G_PERSIST_TEXTURE_FILTER mode)
{
	switch( mode)
	{
		case I3G_PERSIST_TEXTURE_FILTER_POINT					: return I3G_TEXTURE_FILTER_POINT					;
		case I3G_PERSIST_TEXTURE_FILTER_LINEAR					: return I3G_TEXTURE_FILTER_LINEAR					;
		case I3G_PERSIST_TEXTURE_FILTER_POINT_MIPMAP_POINT		: return I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT		;
		case I3G_PERSIST_TEXTURE_FILTER_POINT_MIPMAP_LINEAR		: return I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR		;
		case I3G_PERSIST_TEXTURE_FILTER_LINEAR_MIPMAP_POINT		: return I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT		;
		case I3G_PERSIST_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR	: return I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR	;
		default													: return I3G_TEXTURE_FILTER_NONE;
	}

	#if !defined( I3_COMPILER_VC)
		return I3G_TEXTURE_FILTER_NONE;
	#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Primitive
I3G_PERSIST_PRIMITIVE i3Gfx::Convert( I3G_PRIMITIVE prim)
{
	switch( prim)
	{
		case I3G_PRIM_POINTLIST:	return I3G_PERSIST_PRIM_POINTLIST;
		case I3G_PRIM_LINELIST:		return I3G_PERSIST_PRIM_LINELIST;
		case I3G_PRIM_LINESTRIP:	return I3G_PERSIST_PRIM_LINESTRIP;
		case I3G_PRIM_TRILIST:		return I3G_PERSIST_PRIM_TRILIST;
		case I3G_PRIM_TRISTRIP:		return I3G_PERSIST_PRIM_TRISTRIP;

	#if !defined( I3G_DX) && !defined( I3G_XBOX) && !defined( I3G_OGLES)
		case I3G_PRIM_POINTSPRITE :	return I3G_PERSIST_PRIM_POINTSPRITE;
	#endif

		default :					return I3G_PERSIST_PRIM_TRIFAN;
	}
}

I3G_PRIMITIVE i3Gfx::Convert( I3G_PERSIST_PRIMITIVE prim)
{
	switch( prim)
	{
		case I3G_PERSIST_PRIM_POINTLIST:	return I3G_PRIM_POINTLIST;
		case I3G_PERSIST_PRIM_LINELIST:		return I3G_PRIM_LINELIST;
		case I3G_PERSIST_PRIM_LINESTRIP:	return I3G_PRIM_LINESTRIP;
		case I3G_PERSIST_PRIM_TRILIST:		return I3G_PRIM_TRILIST;
		case I3G_PERSIST_PRIM_TRISTRIP:		return I3G_PRIM_TRISTRIP;
		case I3G_PERSIST_PRIM_TRIFAN:		return I3G_PRIM_TRIFAN;
		case I3G_PERSIST_PRIM_POINTSPRITE :	return I3G_PRIM_POINTSPRITE;
		default:							return I3G_PRIM_NONE;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Primitive
I3G_PERSIST_LIGHT_TYPE	i3Gfx::Convert( I3G_LIGHT_TYPE type)
{
	switch( type)
	{
		case I3G_LIGHT_DIRECTIONAL :	return I3G_PERSIST_LIGHT_DIRECTIONAL;
		case I3G_LIGHT_SPOT :			return I3G_PERSIST_LIGHT_SPOT;
		default :						return I3G_PERSIST_LIGHT_POINT;
		
	}
}

I3G_LIGHT_TYPE	i3Gfx::Convert( I3G_PERSIST_LIGHT_TYPE type)
{
	switch( type)
	{
		case I3G_PERSIST_LIGHT_DIRECTIONAL :	return I3G_LIGHT_DIRECTIONAL;
		case I3G_PERSIST_LIGHT_SPOT :			return I3G_LIGHT_SPOT;
		default :								return I3G_LIGHT_POINT;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Texture Coordinates Source
I3G_PERSIST_TEXTURE_COORD_SOURCE	i3Gfx::Convert( I3G_TEXTURE_COORD_SOURCE mode)
{
	switch( mode)
	{
		case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION		: 
			return I3G_PERSIST_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION;

		case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL		: 
			return I3G_PERSIST_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL;

		case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION	: 
			return I3G_PERSIST_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION;

		default					: 
			return I3G_PERSIST_TEXTURE_COORD_SOURCE_VERTEX;
	}
}

I3G_TEXTURE_COORD_SOURCE			i3Gfx::Convert( I3G_PERSIST_TEXTURE_COORD_SOURCE mode)
{
	switch( mode)
	{
		case I3G_PERSIST_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION		: 
			return I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION;

		case I3G_PERSIST_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL		: 
			return I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL;

		case I3G_PERSIST_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION	: 
			return I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION;

		default					: 
			return I3G_TEXTURE_COORD_SOURCE_VERTEX;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Texture Function
I3G_PERSIST_TEXTURE_FUNCTION		i3Gfx::Convert( I3G_TEXTURE_FUNCTION func)
{
	switch( func)
	{
		case I3G_TEXTURE_FUNC_REPLACE		: 
			return I3G_PERSIST_TEXTURE_FUNC_REPLACE;

		case I3G_TEXTURE_FUNC_MODULATE		: 
			return I3G_PERSIST_TEXTURE_FUNC_MODULATE;

		case I3G_TEXTURE_FUNC_ADD	: 
			return I3G_PERSIST_TEXTURE_FUNC_ADD;

		default					: 
			return I3G_PERSIST_TEXTURE_FUNC_SPECULAR_HIGHLIGHT;
	}
}

I3G_TEXTURE_FUNCTION				i3Gfx::Convert( I3G_PERSIST_TEXTURE_FUNCTION func)
{
	switch( func)
	{
		case I3G_PERSIST_TEXTURE_FUNC_REPLACE		: 
			return I3G_TEXTURE_FUNC_REPLACE;

		case I3G_PERSIST_TEXTURE_FUNC_MODULATE		: 
			return I3G_TEXTURE_FUNC_MODULATE;

		case I3G_PERSIST_TEXTURE_FUNC_ADD	: 
			return I3G_TEXTURE_FUNC_ADD;

		default					: 
			return I3G_TEXTURE_FUNC_SPECULAR_HIGHLIGHT;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Texture Function
I3G_PERSIST_CUBE_TEXTURE_FACE		i3Gfx::Convert( I3G_CUBE_TEXTURE_FACE face)
{
	switch( face)
	{
		case I3G_CUBE_TEXTURE_FACE_PX :
			return I3G_PERSIST_CUBE_TEXTURE_FACE_PX;

		case I3G_CUBE_TEXTURE_FACE_NX :
			return I3G_PERSIST_CUBE_TEXTURE_FACE_NX;

		case I3G_CUBE_TEXTURE_FACE_PY :
			return I3G_PERSIST_CUBE_TEXTURE_FACE_PY;

		case I3G_CUBE_TEXTURE_FACE_NY :
			return I3G_PERSIST_CUBE_TEXTURE_FACE_NY;

		case I3G_CUBE_TEXTURE_FACE_PZ :
			return I3G_PERSIST_CUBE_TEXTURE_FACE_PZ;

		default :
			return I3G_PERSIST_CUBE_TEXTURE_FACE_NZ;
	}
}

I3G_CUBE_TEXTURE_FACE				i3Gfx::Convert( I3G_PERSIST_CUBE_TEXTURE_FACE face)
{
	switch( face)
	{
		case I3G_PERSIST_CUBE_TEXTURE_FACE_PX :
			return I3G_CUBE_TEXTURE_FACE_PX;

		case I3G_PERSIST_CUBE_TEXTURE_FACE_NX :
			return I3G_CUBE_TEXTURE_FACE_NX;

		case I3G_PERSIST_CUBE_TEXTURE_FACE_PY :
			return I3G_CUBE_TEXTURE_FACE_PY;

		case I3G_PERSIST_CUBE_TEXTURE_FACE_NY :
			return I3G_CUBE_TEXTURE_FACE_NY;

		case I3G_PERSIST_CUBE_TEXTURE_FACE_PZ :
			return I3G_CUBE_TEXTURE_FACE_PZ;

		default :
			return I3G_CUBE_TEXTURE_FACE_NZ;
	}
}
