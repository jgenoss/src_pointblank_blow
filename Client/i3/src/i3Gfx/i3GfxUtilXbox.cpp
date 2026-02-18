#include "i3GFXType.h"
#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3Debug.h"
#include "i3GfxType.h"
#include "i3GfxDefine.h"
#include "i3VertexArray.h"

#ifdef I3G_XBOX
#include "i3GfxDefineXbox.h"

//////////////////////////////////////////////////////////////////////////////
// Resource Type
D3DRESOURCETYPE	i3_GetNativeResourceType( I3G_RESOURCE_TYPE type)
{
	switch( type)
	{
		case I3G_RESOURCE_TYPE_RENDERTARGET	:	return D3DRTYPE_SURFACE;
		case I3G_RESOURCE_TYPE_TEXTURE		:	return D3DRTYPE_TEXTURE;
		case I3G_RESOURCE_TYPE_VOLUME		:	return D3DRTYPE_VOLUME;
		case I3G_RESOURCE_TYPE_VOLUMETEXTURE	:	return D3DRTYPE_VOLUMETEXTURE;
		case I3G_RESOURCE_TYPE_CUBETEXTURE	:	return D3DRTYPE_CUBETEXTURE;
		case I3G_RESOURCE_TYPE_VERTEXARRAY	:	return D3DRTYPE_VERTEXBUFFER;
		case I3G_RESOURCE_TYPE_INDEXARRAY	:	return D3DRTYPE_INDEXBUFFER;
	}

	return (D3DRESOURCETYPE) 0;
}

I3G_RESOURCE_TYPE i3_GetI3ResourceType( D3DRESOURCETYPE type)
{
	switch( type)
	{
		case D3DRTYPE_SURFACE		:		return I3G_RESOURCE_TYPE_RENDERTARGET;
		case D3DRTYPE_TEXTURE		:		return I3G_RESOURCE_TYPE_TEXTURE;
		case D3DRTYPE_VOLUME		:		return I3G_RESOURCE_TYPE_VOLUME	;
		case D3DRTYPE_VOLUMETEXTURE	:		return I3G_RESOURCE_TYPE_VOLUMETEXTURE;
		case D3DRTYPE_CUBETEXTURE	:		return I3G_RESOURCE_TYPE_CUBETEXTURE;	
		case D3DRTYPE_VERTEXBUFFER	:		return I3G_RESOURCE_TYPE_VERTEXARRAY;
		case D3DRTYPE_INDEXBUFFER	:		return I3G_RESOURCE_TYPE_INDEXARRAY;
	}

	return I3G_RESOURCE_TYPE_NONE;
}

//////////////////////////////////////////////////////////////////////////////
// Raster Format
D3DFORMAT	i3_GetNativeImageFormat( I3G_IMAGE_FORMAT format, BOOL *bExact )
{
	#define SET_NOTEXACT { if( bExact  ) *bExact = FALSE; }

	if( bExact ) *bExact  = FALSE;

	if( format & I3G_IMAGE_FORMAT_MASK_LINEAER )
	{
		switch( format)
		{
		// 4~8 bit format
		case I3G_IMAGE_FORMAT_I4		: SET_NOTEXACT; return D3DFMT_P8; // D3DFMT_LIN_P4;
		case I3G_IMAGE_FORMAT_I8		: SET_NOTEXACT; return D3DFMT_P8; // D3DFMT_LIN_P8;
		case i3G_IMAGE_FORMAT_L8		: return D3DFMT_LIN_L8;
		case i3G_IMAGE_FORMAT_A8		: return D3DFMT_LIN_A8;

		// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565	: return D3DFMT_LIN_R5G6B5;
		case I3G_IMAGE_FORMAT_XRGB_1555	: return D3DFMT_LIN_X1R5G5B5;
		case I3G_IMAGE_FORMAT_ARGB_1555	: return D3DFMT_LIN_A1R5G5B5;
		case I3G_IMAGE_FORMAT_XRGB_4444	: SET_NOTEXACT; return D3DFMT_LIN_A4R4G4B4; // D3DFMT_LIN_X4R4G4B4;
		case I3G_IMAGE_FORMAT_ARGB_4444	: return D3DFMT_LIN_A4R4G4B4;

		// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888	: SET_NOTEXACT; return D3DFMT_LIN_X8R8G8B8; // D3DFMT_LIN_R8G8B8;
		case I3G_IMAGE_FORMAT_BGR_888	: SET_NOTEXACT; return D3DFMT_LIN_X8R8G8B8; // D3DFMT_LIN_B8G8R8;

		// 32 bit formats
		case I3G_IMAGE_FORMAT_XRGB_8888	: return D3DFMT_LIN_X8R8G8B8;
		case I3G_IMAGE_FORMAT_XBGR_8888	: SET_NOTEXACT; return D3DFMT_LIN_X8R8G8B8; // D3DFMT_X8B8G8R8;
		case I3G_IMAGE_FORMAT_ARGB_8888	: return D3DFMT_LIN_A8R8G8B8;
		case I3G_IMAGE_FORMAT_ABGR_8888	: return D3DFMT_LIN_A8B8G8R8;
		case I3G_IMAGE_FORMAT_RGBA_8888	: return D3DFMT_LIN_R8G8B8A8;
		case I3G_IMAGE_FORMAT_BGRA_8888	: return D3DFMT_LIN_B8G8R8A8;

		// -------------------------
		// Bump-map
		// -------------------------
		case I3G_IMAGE_FORMAT_VU_88		: return D3DFMT_LIN_V8U8;
		case I3G_IMAGE_FORMAT_QWVU_8888	: return D3DFMT_LIN_Q8W8V8U8;

		// -------------------------
		// HDR
		// -------------------------

		// -------------------------
		// depth, stencil, vertex buffers
		// -------------------------
		case I3G_IMAGE_FORMAT_D16		: return D3DFMT_LIN_D16;
		case I3G_IMAGE_FORMAT_D24S8		: return D3DFMT_LIN_D24S8;
		case I3G_IMAGE_FORMAT_D24X8		: SET_NOTEXACT; return D3DFMT_LIN_D24S8; // D3DFMT_D24X8;
		case I3G_IMAGE_FORMAT_D32		: SET_NOTEXACT; return D3DFMT_LIN_D24S8; // D3DFMT_D32;

		#if defined( I3_DEBUG)
		default :
			i3Log( "i3GfxUtilXbox", "[I3ERROR] Unsupported image format : %d\n", format);
			I3ASSERT(0);
			break;
		#endif
		}
	}
	else
	{
		switch( format)
		{
		// 4~8 bit format
		case I3G_IMAGE_FORMAT_I4		: SET_NOTEXACT; return D3DFMT_P8; // D3DFMT_P4;
		case I3G_IMAGE_FORMAT_I8		: return D3DFMT_P8;
		case i3G_IMAGE_FORMAT_L8		: return D3DFMT_L8;
		case i3G_IMAGE_FORMAT_A8		: return D3DFMT_A8;

		// 16 bit formats
		case I3G_IMAGE_FORMAT_RGB_565	: return D3DFMT_R5G6B5;
		case I3G_IMAGE_FORMAT_XRGB_1555	: return D3DFMT_X1R5G5B5;
		case I3G_IMAGE_FORMAT_ARGB_1555	: return D3DFMT_A1R5G5B5;
		case I3G_IMAGE_FORMAT_XRGB_4444	: SET_NOTEXACT; return D3DFMT_A4R4G4B4; // D3DFMT_X4R4G4B4
		case I3G_IMAGE_FORMAT_ARGB_4444	: return D3DFMT_A4R4G4B4;

		// 24 bit formats
		case I3G_IMAGE_FORMAT_RGB_888	: SET_NOTEXACT; return D3DFMT_X8R8G8B8; // D3DFMT_R8G8B8;
		case I3G_IMAGE_FORMAT_BGR_888	: SET_NOTEXACT; return D3DFMT_X8R8G8B8; // D3DFMT_B8G8R8;

		// 32 bit formats
		case I3G_IMAGE_FORMAT_XRGB_8888	: return D3DFMT_X8R8G8B8;
		case I3G_IMAGE_FORMAT_XBGR_8888	: SET_NOTEXACT; return D3DFMT_X8R8G8B8; // D3DFMT_X8B8G8R8;
		case I3G_IMAGE_FORMAT_ARGB_8888	: return D3DFMT_A8R8G8B8;
		case I3G_IMAGE_FORMAT_ABGR_8888	: return D3DFMT_A8B8G8R8;
		case I3G_IMAGE_FORMAT_RGBA_8888	: return D3DFMT_R8G8B8A8;
		case I3G_IMAGE_FORMAT_BGRA_8888	: return D3DFMT_B8G8R8A8;

		// -------------------------
		// Bump-map
		// -------------------------
		case I3G_IMAGE_FORMAT_VU_88		: return D3DFMT_V8U8;
		case I3G_IMAGE_FORMAT_QWVU_8888	: return D3DFMT_Q8W8V8U8;

		// -------------------------
		// Compressed formats
		// -------------------------
		case I3G_IMAGE_FORMAT_DXT1_X	: SET_NOTEXACT; return D3DFMT_DXT1;
		case I3G_IMAGE_FORMAT_DXT1		: return D3DFMT_DXT1;
		case I3G_IMAGE_FORMAT_DXT2		: return D3DFMT_DXT2;
		case I3G_IMAGE_FORMAT_DXT3		: return D3DFMT_DXT3;
		case I3G_IMAGE_FORMAT_DXT4		: return D3DFMT_DXT4;
		case I3G_IMAGE_FORMAT_DXT5		: return D3DFMT_DXT5;

		// -------------------------
		// HDR
		// -------------------------

		// -------------------------
		// depth, stencil, vertex buffers
		// -------------------------
		case I3G_IMAGE_FORMAT_D16		: return D3DFMT_D16;
		case I3G_IMAGE_FORMAT_D24S8		: return D3DFMT_D24S8;
		case I3G_IMAGE_FORMAT_D24X8		: SET_NOTEXACT; return D3DFMT_D24S8; // D3DFMT_D24X8;
		case I3G_IMAGE_FORMAT_D32		: SET_NOTEXACT; return D3DFMT_D24S8; // D3DFMT_D32;

		#if defined( I3_DEBUG)
		default :
			i3Log( "i3GfxUtilXbox", "[I3ERROR] Unsupported image format : %d\n", format);
			I3ASSERT(0);
			break;
		#endif
		}
	}
	return D3DFMT_UNKNOWN;
}

I3G_IMAGE_FORMAT	i3_GetI3ImageFormat( D3DFORMAT format, BOOL *bExact )
{
	#define SET_NOTEXACT { if( bExact ) *bExact = FALSE; }

	if( bExact ) *bExact = TRUE;

	switch( format)
	{
	// 4~8 bit format
	case D3DFMT_P8: return I3G_IMAGE_FORMAT_I8;
	case D3DFMT_L8: return i3G_IMAGE_FORMAT_L8;
	case D3DFMT_A8: return i3G_IMAGE_FORMAT_A8;

	// 16 bit formats
	case D3DFMT_R5G6B5: return I3G_IMAGE_FORMAT_RGB_565;
	case D3DFMT_X1R5G5B5: return I3G_IMAGE_FORMAT_XRGB_1555;
	case D3DFMT_A1R5G5B5: return I3G_IMAGE_FORMAT_ARGB_1555;
	case D3DFMT_A4R4G4B4: return I3G_IMAGE_FORMAT_ARGB_4444;

	// 32 bit formats
	case D3DFMT_X8R8G8B8: return I3G_IMAGE_FORMAT_XRGB_8888;
	case D3DFMT_A8R8G8B8: return I3G_IMAGE_FORMAT_ARGB_8888;
	case D3DFMT_A8B8G8R8: return I3G_IMAGE_FORMAT_ABGR_8888;
	case D3DFMT_R8G8B8A8: return I3G_IMAGE_FORMAT_RGBA_8888;
	case D3DFMT_B8G8R8A8: return I3G_IMAGE_FORMAT_BGRA_8888;

	// -------------------------
	// Bump-map
	// -------------------------
	case D3DFMT_V8U8: return I3G_IMAGE_FORMAT_VU_88;
	// case D3DFMT_Q8W8V8U8: return I3G_IMAGE_FORMAT_QWVU_8888;

	// -------------------------
	// Compressed formats
	// -------------------------
	case D3DFMT_DXT1: SET_NOTEXACT; return I3G_IMAGE_FORMAT_DXT1;
	case D3DFMT_DXT2: SET_NOTEXACT; return I3G_IMAGE_FORMAT_DXT2;
	// case D3DFMT_DXT3: return I3G_IMAGE_FORMAT_DXT3;
	case D3DFMT_DXT4: SET_NOTEXACT; return I3G_IMAGE_FORMAT_DXT4;
	// case D3DFMT_DXT5: return I3G_IMAGE_FORMAT_DXT5;

	// -------------------------
	// HDR
	// -------------------------

	// -------------------------
	// depth, stencil, vertex buffers
	// -------------------------
	case D3DFMT_D16: return I3G_IMAGE_FORMAT_D16;
	case D3DFMT_D24S8: return I3G_IMAGE_FORMAT_D24S8;

	#if defined( I3_DEBUG)
	default :
		i3Log( "i3GfxUtilXbox", "[I3ERROR] Unsupported image format : %d\n", format);
		I3ASSERT(0);
		break;
	#endif
	}

	return I3G_IMAGE_FORMAT_NONE;
}

////////////////////////////////////////////////////////////////////////////////////////
// Usage Flags

I3G_USAGE i3_GetI3Usage( DWORD usage)
{
	switch( usage)
	{
		case D3DUSAGE_DYNAMIC				: return I3G_USAGE_DYNAMIC;
		// case D3DUSAGE_SOFTWAREPROCESSING : return I3G_USAGE_SOFTWAREPROCESSING;
		case D3DUSAGE_WRITEONLY				: return I3G_USAGE_WRITEONLY;
		case D3DUSAGE_POINTS				: return I3G_USAGE_POINT_SPRITES;
		case D3DUSAGE_RTPATCHES				: return I3G_USAGE_RTPATCHES;
	}

	return I3G_USAGE_NONE;
}

DWORD i3_GetNativeUsage( I3G_USAGE usage)
{
	switch( usage)
	{
		case I3G_USAGE_DYNAMIC				: return D3DUSAGE_DYNAMIC;
		// case I3G_USAGE_SOFTWAREPROCESSING :	return D3DUSAGE_SOFTWAREPROCESSING;
		case I3G_USAGE_WRITEONLY			: return D3DUSAGE_WRITEONLY;
		case I3G_USAGE_POINT_SPRITES		: return D3DUSAGE_POINTS;
		case D3DUSAGE_RTPATCHES				: return I3G_USAGE_RTPATCHES;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Vertex Format
static UINT32		s_DXFVF_TexSet[] =
{
	D3DFVF_TEX0,
	D3DFVF_TEX1,
	D3DFVF_TEX2,
	D3DFVF_TEX3,
	D3DFVF_TEX4,
	//D3DFVF_TEX5,
	//D3DFVF_TEX6,
	//D3DFVF_TEX7,
	//D3DFVF_TEX8,
};

static UINT32	s_DXFVF_TexFormat[] =
{
	D3DFVF_TEXTUREFORMAT1,
	D3DFVF_TEXTUREFORMAT2,
	D3DFVF_TEXTUREFORMAT3,
	D3DFVF_TEXTUREFORMAT4
};

DWORD i3_GetNativeVertexFormat( i3VertexFormat * pFormat)
{
	DWORD Format = 0;
	INT32 i;

	if( pFormat->GetHasPosition())
	{
		if( pFormat->IsTransformed() && !pFormat->GetHasNormal())
			Format |= D3DFVF_XYZRHW;
		else
			Format |= D3DFVF_XYZ;
	}

	if( pFormat->GetHasColor())
	{
		Format |= D3DFVF_DIFFUSE;
	}

	if( pFormat->GetHasNormal())
	{
		Format |= D3DFVF_NORMAL;
	}

	//if( pFormat->GetPointSpriteCoordCount() > 0)
	//{
	//	Format |= D3DFVF_PSIZE;
	//}

	if( pFormat->GetTextureCoordSetCount() > 0)
	{
		Format |= s_DXFVF_TexSet[ pFormat->GetTextureCoordSetCount()];

		for( i = 0; i < pFormat->GetTextureCoordSetCount(); i++)
		{
			switch( pFormat->GetTextureCoordCount())
			{
				case 1 :		Format |= D3DFVF_TEXCOORDSIZE1( i);	break;
				case 2 :		Format |= D3DFVF_TEXCOORDSIZE2( i);	break;
				case 3 :		Format |= D3DFVF_TEXCOORDSIZE3( i);	break;
				case 4 :		Format |= D3DFVF_TEXCOORDSIZE4( i);	break;
			}
		}
	}

	return Format;
}

#endif