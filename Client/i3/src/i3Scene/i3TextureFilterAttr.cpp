#include "i3SceneDef.h"
#include "i3TextureFilterAttr.h"

I3_CLASS_INSTANCE( i3TextureFilterAttr);

i3TextureFilterAttr::i3TextureFilterAttr(void)
{
	SetID( I3_ATTRID_TEXTURE_FILTER);
}


void i3TextureFilterAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureFilter( m_Bind, m_MagFilter, m_MinFilter);
}

void i3TextureFilterAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3TextureFilterAttr * pDest = (i3TextureFilterAttr *) pObj;

	i3RenderAttr::CopyTo( pObj, method);

	pDest->SetMagFilter( GetMagFilter());
	pDest->SetMinFilter( GetMinFilter());
	pDest->SetBindType( GetBindType());
}

bool i3TextureFilterAttr::IsSame( i3RenderAttr * pAttr)
{
	return ((m_Bind == ((i3TextureFilterAttr *)pAttr)->GetBindType()) &&
			(m_MagFilter == ((i3TextureFilterAttr *)pAttr)->GetMagFilter()) &&
			(m_MinFilter == ((i3TextureFilterAttr *)pAttr)->GetMinFilter()));
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED TEXTUREFILTER_INFO
	{
		UINT8		m_MagFilter = 0;
		UINT8		m_MinFilter = 0;
		UINT16		m_Bind = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TextureFilterAttr::OnSave( i3ResourceFile * pResFile)
{
	pack::TEXTUREFILTER_INFO Info;
	UINT32 Rc;

	Info.m_MagFilter = (UINT8) i3Gfx::Convert( m_MagFilter);
	Info.m_MinFilter = (UINT8) i3Gfx::Convert( m_MinFilter);
	Info.m_Bind = (UINT16) m_Bind;

	Rc = pResFile->GetStream()->Write( &Info, sizeof(pack::TEXTUREFILTER_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureFilterAttr::OnSave()", "Could not write texture filter information.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3TextureFilterAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3TextureFilterAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	pack::TEXTUREFILTER_INFO Info;
	UINT32 Rc;

	Rc = pResFile->GetStream()->Read( &Info, sizeof(pack::TEXTUREFILTER_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureFilterAttr::OnSave()", "Could not read texture filter information.");
		return STREAM_ERR;
	}

	m_MagFilter = i3Gfx::Convert( (I3G_PERSIST_TEXTURE_FILTER) Info.m_MagFilter);
	m_MinFilter = i3Gfx::Convert( (I3G_PERSIST_TEXTURE_FILTER) Info.m_MinFilter);
	m_Bind = (I3G_TEXTURE_BIND) Info.m_Bind;

	return Rc;
}

#if defined( I3_DEBUG)
void i3TextureFilterAttr::Dump(void)
{
	static char s_FilterStr[7][32] =
	{
		"None",
		"Point",
		"Linear",
		"Point Mipmap Point",
		"Point Mipmap Linear",
		"Linear Mipmap Point",
		"Linear Mipmap Linear"
	};
	INT32 idxMag  = 1, idxMin = 1;

	switch( m_MagFilter)
	{
		case I3G_TEXTURE_FILTER_POINT	:				idxMag = 1;	break;
		case I3G_TEXTURE_FILTER_LINEAR	:				idxMag = 2;	break;

		#if !defined( I3G_DX) && !defined( I3G_XBOX)
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT :	idxMag = 3;	break;
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR :	idxMag = 4;	break;
		case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT :	idxMag = 5;	break;
		case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR :	idxMag = 6;	break;
		#endif

		default :										idxMag = 0;	break;
	}

	switch( m_MinFilter)
	{
		case I3G_TEXTURE_FILTER_POINT	:				idxMin = 1;	break;
		case I3G_TEXTURE_FILTER_LINEAR	:				idxMin = 2;	break;

		#if !defined( I3G_DX) && !defined( I3G_XBOX)
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_POINT :	idxMag = 3;	break;
		case I3G_TEXTURE_FILTER_POINT_MIPMAP_LINEAR :	idxMag = 4;	break;
		case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT :	idxMag = 5;	break;
		case I3G_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR :	idxMag = 6;	break;
		#endif

		default :										idxMin = 0;	break;
	}

	I3TRACE( "%s : Mag:%s  Min:%s\n", meta()->class_name(), s_FilterStr[idxMag], s_FilterStr[idxMin]);
}
#endif
