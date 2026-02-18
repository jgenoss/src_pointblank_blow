#include "i3GFXType.h"
#include "../i3Base/i3CommonType.h"
#include "../i3Base/i3Debug.h"
#include "i3GfxType.h"
#include "i3GfxDefine.h"
#include "i3VertexArray.h"
#include "i3GfxUtilDx.h"

#if defined( I3G_DX)
#include "i3GfxDefineDX.h"
#include "i3GfxGlobalVariable.h"

//////////////////////////////////////////////////////////////////////////////
// Resource Type
D3DRESOURCETYPE	i3DXUT_GetNativeResourceType( I3G_RESOURCE_TYPE type)
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

I3G_RESOURCE_TYPE i3DXUT_GetI3ResourceType( D3DRESOURCETYPE type)
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
I3_EXPORT_GFX
D3DFORMAT	i3DXUT_GetNativeImageFormat( I3G_IMAGE_FORMAT format, bool *bExact )
{
	#define SET_NOTEXACT { if( bExact ) *bExact = false; }

	if( bExact ) *bExact = true;

	switch( format & ~(I3G_IMAGE_FORMAT_MASK_ZLIB | I3G_IMAGE_FORMAT_MASK_PLATFORM))
	{
	case I3G_IMAGE_FORMAT_DEFAULT_COLOR :		return D3DFMT_A8R8G8B8;

	// 4 bit formats
	// DirectX에서는 Index Image는 256까지만 지원하기 때문에
	// 4 Bits Image라고 하더라도 8 Bits Image로 생성한다.
	case I3G_IMAGE_FORMAT_I4		: SET_NOTEXACT; return D3DFMT_P8; // D3DFMT_P4;

	// 8bit formats
	case I3G_IMAGE_FORMAT_I8		:	return D3DFMT_P8;
	case i3G_IMAGE_FORMAT_L8		:	return D3DFMT_L8;
	case i3G_IMAGE_FORMAT_A8		:	return D3DFMT_A8;

	// 16 bit formats
	case I3G_IMAGE_FORMAT_BGR_565 :
	case I3G_IMAGE_FORMAT_RGB_565	:	return D3DFMT_R5G6B5;
	case I3G_IMAGE_FORMAT_XRGB_1555	:	return D3DFMT_X1R5G5B5;
	case I3G_IMAGE_FORMAT_ARGB_1555	:	return D3DFMT_A1R5G5B5;
	case I3G_IMAGE_FORMAT_XRGB_4444	:	return D3DFMT_X4R4G4B4;
	case I3G_IMAGE_FORMAT_ARGB_4444	:	return D3DFMT_A4R4G4B4;
	case I3G_IMAGE_FORMAT_ABGR_4444 :	return D3DFMT_A4R4G4B4;

	// 24 bit formats
	case I3G_IMAGE_FORMAT_RGB_888	:	SET_NOTEXACT; return D3DFMT_R8G8B8;
	case I3G_IMAGE_FORMAT_BGR_888	:	return D3DFMT_R8G8B8; // D3DFMT_B8G8R8;

	// 32 bit formats
	case I3G_IMAGE_FORMAT_RGBX_8888	:	SET_NOTEXACT; return D3DFMT_X8R8G8B8;
	case I3G_IMAGE_FORMAT_BGRX_8888	:	return D3DFMT_X8R8G8B8; // D3DFMT_X8B8G8R8;
	case I3G_IMAGE_FORMAT_RGBA_8888	:	SET_NOTEXACT; return D3DFMT_A8R8G8B8;
	case I3G_IMAGE_FORMAT_BGRA_8888	:	return D3DFMT_A8R8G8B8;

	// -------------------------
	// Bump-map
	// -------------------------
	case I3G_IMAGE_FORMAT_VU_88:		return D3DFMT_V8U8;
	case I3G_IMAGE_FORMAT_QWVU_8888:	return D3DFMT_Q8W8V8U8;

	// -------------------------
	// Compressed formats
	// -------------------------
	case I3G_IMAGE_FORMAT_DXT1_X:		
		{
			if( g_pRenderContext->IsCompressedTexture())
			{
				return D3DFMT_DXT1;
			}
			else
			{
				return D3DFMT_A8R8G8B8;
			}
		}
	case I3G_IMAGE_FORMAT_DXT1:		
		{
			if( (g_pRenderContext == nullptr) || g_pRenderContext->IsCompressedTexture())
			{
				return D3DFMT_DXT1;
			}
			else
			{
				//return D3DFMT_A8R8G8B8;
				return D3DFMT_A1R5G5B5;
			}
		}
	case I3G_IMAGE_FORMAT_DXT2:	
		{
			if( (g_pRenderContext == nullptr) || g_pRenderContext->IsCompressedTexture())
			{
				return D3DFMT_DXT2;
			}
			else
			{
				return D3DFMT_A8R8G8B8;
			}
		}		
	case I3G_IMAGE_FORMAT_DXT3:
		{
			if((g_pRenderContext == nullptr) || g_pRenderContext->IsCompressedTexture())
			{
				return D3DFMT_DXT3;
			}
			else
			{
				//return D3DFMT_A8R8G8B8;
				return D3DFMT_A1R5G5B5;
			}
		}
	case I3G_IMAGE_FORMAT_DXT4:
		{
			if((g_pRenderContext == nullptr) || g_pRenderContext->IsCompressedTexture())
			{
				return D3DFMT_DXT4;
			}
			else
			{
				return D3DFMT_A8R8G8B8;
			}
		}
	case I3G_IMAGE_FORMAT_DXT5:
		{
			if((g_pRenderContext == nullptr) || g_pRenderContext->IsCompressedTexture())
			{
				return D3DFMT_DXT5;
			}
			else
			{
				//return D3DFMT_A8R8G8B8;
				return D3DFMT_A1R5G5B5;
			}
		}

	// -------------------------
	// HDR
	// -------------------------
	case I3G_IMAGE_FORMAT_ABGR_16F :	return D3DFMT_A16B16G16R16F;
	case I3G_IMAGE_FORMAT_ABGR_32F :	return D3DFMT_A32B32G32R32F;
	case I3G_IMAGE_FORMAT_ABGR_16 :		return D3DFMT_A16B16G16R16;
	case I3G_IMAGE_FORMAT_GR_16F :		return D3DFMT_G16R16F;
	case I3G_IMAGE_FORMAT_R_32F	:		return D3DFMT_R32F;
	case I3G_IMAGE_FORMAT_GR_32F :		return D3DFMT_G32R32F;

	// -------------------------
	// depth, stencil, vertex buffers
	// -------------------------
	case I3G_IMAGE_FORMAT_D16:			return D3DFMT_D16;
	case I3G_IMAGE_FORMAT_D24S8:		return D3DFMT_D24S8;
	case I3G_IMAGE_FORMAT_D24X8:		return D3DFMT_D24X8;
	case I3G_IMAGE_FORMAT_D32:			return D3DFMT_D32;
	case I3G_IMAGE_FORMAT_D32F :		return D3DFMT_D32F_LOCKABLE;
	case I3G_IMAGE_FORMAT_D24FS8 :		return D3DFMT_D24FS8;

	#if defined( I3_DEBUG)
	default :
		i3Log( "i3GfxUtilDX", "[I3ERROR] Unsupported raster format : %x\n", format);
		break;
	#endif
	}

	return D3DFMT_UNKNOWN;
}

I3G_IMAGE_FORMAT	i3DXUT_GetI3ImageFormat( D3DFORMAT format, bool *bExact)
{
	#define SET_NOTEXACT { if( bExact ) *bExact = false; }

	if( bExact ) *bExact = true;

	switch( format)
	{
		case D3DFMT_P8:				return I3G_IMAGE_FORMAT_I8 ;
		case D3DFMT_X1R5G5B5:		return I3G_IMAGE_FORMAT_XRGB_1555;
		case D3DFMT_A1R5G5B5:		return I3G_IMAGE_FORMAT_ARGB_1555 ;
		case D3DFMT_R5G6B5:			return I3G_IMAGE_FORMAT_RGB_565;
		case D3DFMT_R8G8B8:			return I3G_IMAGE_FORMAT_BGR_888;
		case D3DFMT_A4R4G4B4:		return I3G_IMAGE_FORMAT_ARGB_4444;
		case D3DFMT_X8R8G8B8:		return I3G_IMAGE_FORMAT_BGRX_8888 ;	
		case D3DFMT_A8R8G8B8:		return I3G_IMAGE_FORMAT_BGRA_8888 ;
		case D3DFMT_DXT1:			return I3G_IMAGE_FORMAT_DXT1	;
		case D3DFMT_DXT2:			return I3G_IMAGE_FORMAT_DXT2	;
		case D3DFMT_DXT3:			return I3G_IMAGE_FORMAT_DXT3	;
		case D3DFMT_DXT4:			return I3G_IMAGE_FORMAT_DXT4	;
		case D3DFMT_DXT5:			return I3G_IMAGE_FORMAT_DXT5 ;
		case D3DFMT_A16B16G16R16F :	return I3G_IMAGE_FORMAT_ABGR_16F;
		case D3DFMT_A32B32G32R32F :	return I3G_IMAGE_FORMAT_ABGR_32F;
		case D3DFMT_A16B16G16R16 :	return I3G_IMAGE_FORMAT_ABGR_16;
		case D3DFMT_G16R16F :		return I3G_IMAGE_FORMAT_GR_16F;
		case D3DFMT_R32F :			return I3G_IMAGE_FORMAT_R_32F;
		case D3DFMT_G32R32F :		return I3G_IMAGE_FORMAT_GR_32F;

		case D3DFMT_D16 :			return I3G_IMAGE_FORMAT_D16;
		case D3DFMT_D24S8 :			return I3G_IMAGE_FORMAT_D24S8;
		case D3DFMT_D24X8 :			return I3G_IMAGE_FORMAT_D24X8;
		case D3DFMT_D32 :			return I3G_IMAGE_FORMAT_D32;
		case D3DFMT_D32F_LOCKABLE :	return I3G_IMAGE_FORMAT_D32F;
		case D3DFMT_D24FS8 :		return I3G_IMAGE_FORMAT_D24FS8;

		#if defined( I3_DEBUG)
		//case I3G_IMAGE_FORMAT_INDEX_4 :
		//case I3G_IMAGE_FORMAT_INDEX_4H :
		//case I3G_IMAGE_FORMAT_INDEX_4L :
		default :
			i3Log( "i3GfxUtilDX", "[I3ERROR] Unsupported raster format : %x\n", format);
			break;
		#endif
	}

	return I3G_IMAGE_FORMAT_NONE;
}

////////////////////////////////////////////////////////////////////////////////////////
// Usage Flags

I3G_USAGE i3DXUT_GetI3Usage( DWORD usage)
{
	I3G_USAGE Result = 0;

	if( usage & D3DUSAGE_DYNAMIC)
		Result |= I3G_USAGE_FREQUENT_UPDATE;

	if( usage & D3DUSAGE_SOFTWAREPROCESSING)
		Result |= I3G_USAGE_SOFTWAREPROCESSING;


	if( usage & D3DUSAGE_WRITEONLY)
		Result |= I3G_USAGE_WRITEONLY;

	if( usage & D3DUSAGE_POINTS)
		Result |= I3G_USAGE_POINT_SPRITES;

	if( usage & D3DUSAGE_RENDERTARGET)
		Result |= I3G_USAGE_RENDERTARGET;

	if( usage & D3DUSAGE_AUTOGENMIPMAP)
		Result |= I3G_USAGE_AUTOGENMIPMAP;

	return Result;
}

DWORD i3DXUT_GetNativeUsage( I3G_USAGE usage)
{
	DWORD Result = 0;

	if( usage & I3G_USAGE_FREQUENT_UPDATE)
		Result |= D3DUSAGE_DYNAMIC;

	if( usage & I3G_USAGE_SOFTWAREPROCESSING)
		Result |= D3DUSAGE_SOFTWAREPROCESSING;

	if( usage & I3G_USAGE_WRITEONLY)
		Result |= D3DUSAGE_WRITEONLY;

	if( usage & I3G_USAGE_POINT_SPRITES)
		Result |= D3DUSAGE_POINTS;

	if( usage & I3G_USAGE_RENDERTARGET)
		Result |= D3DUSAGE_RENDERTARGET;

	if( usage & I3G_USAGE_AUTOGENMIPMAP)
		Result |= D3DUSAGE_AUTOGENMIPMAP;

	return Result;
}

I3G_USAGE i3DXUT_DetermineRuntimeUsage( i3VertexFormat * pFormat, I3G_USAGE usage)
{
	I3G_USAGE Result;
	//i3GfxCaps * pCaps = g_pRenderContext->GetCaps();

	Result = usage;

	// Software로 Vertex를 처리하는 경우는 아래와 같다.
	//
	// 1. Vertex Blending을 하는데, Blend Index의 개수가 Caps 개수를 넘어가는 경우.
	// 2. Vertex Blending을 하는데, Blending Matrix의 개수가 MaxVertexProcessMatrix 개수를 넘어가는 경우.
	// 3. Particle.
	/*
	if( pFormat->GetBlendIndexCount() > pCaps->GetMaxBlendIndexCount())
	{
		Result |= I3G_USAGE_SOFTWAREPROCESSING;
	}

	if( pFormat->GetBlendWeightCount() > pCaps->GetMaxVertexBlendMatrix())
	{
		Result |= I3G_USAGE_SOFTWAREPROCESSING;
	}

	if( pFormat->GetPointSpriteCoordCount() > 0)
	{
		Result |= I3G_USAGE_SOFTWAREPROCESSING;
	}
	*/

	return Result;
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
	D3DFVF_TEX5,
	D3DFVF_TEX6,
	D3DFVF_TEX7,
	D3DFVF_TEX8,
};

static UINT32	s_DXFVF_TexFormat[] =
{
	D3DFVF_TEXTUREFORMAT1,
	D3DFVF_TEXTUREFORMAT2,
	D3DFVF_TEXTUREFORMAT3,
	D3DFVF_TEXTUREFORMAT4
};

DWORD i3DXUT_GetNativeVertexFormat( i3VertexFormat * pFormat)
{
	DWORD Format = 0;
	INT32 i;

	if( pFormat->GetHasPosition())
	{
		int sp = 0;

		if( pFormat->GetBlendIndexCount() > 0)
		{
			Format |= D3DFVF_LASTBETA_UBYTE4;
			sp = 1;
		}

		if( (pFormat->GetBlendWeightCount() + sp) > 0)
		{
			switch( pFormat->GetBlendWeightCount() + sp)
			{
				case 1 :	Format |= D3DFVF_XYZB1;	break;
				case 2 :	Format |= D3DFVF_XYZB2;	break;
				case 3 :	Format |= D3DFVF_XYZB3;	break;
				case 4 :	Format |= D3DFVF_XYZB4;	break;
				case 5 :	Format |= D3DFVF_XYZB5;	break;
			}
		}
		else
		{
			if( pFormat->IsTransformed() && !pFormat->GetHasNormal())
				Format |= D3DFVF_XYZRHW;
			else
				Format |= D3DFVF_XYZ;
		}
	}

	if( pFormat->GetHasColor())
	{
		Format |= D3DFVF_DIFFUSE;
	}

	if( pFormat->GetHasNormal())
	{
		Format |= D3DFVF_NORMAL;
	}

	/*
	if( pFormat->GetPointSpriteSizeCount() > 0)
	{
		Format |= D3DFVF_PSIZE;
	}
	*/

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

	// Tangent와 Binormal은 FVF에서는 지원하지 않는다.

	return Format;
}

#endif