#include "i3SceneDef.h"
#include "i3TextureWrapAttr.h"

I3_CLASS_INSTANCE( i3TextureWrapAttr);

i3TextureWrapAttr::i3TextureWrapAttr(void)
{
	SetID( I3_ATTRID_TEXTURE_WRAP);
}


void i3TextureWrapAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureWrap( m_Bind, m_HorzWrap, m_VertWrap);
}

void i3TextureWrapAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3TextureWrapAttr *) pObj)->SetHorzWrap( GetHorzWrap());
	((i3TextureWrapAttr *) pObj)->SetVertWrap( GetVertWrap());
	((i3TextureWrapAttr *) pObj)->SetBindType( GetBindType());
}

bool i3TextureWrapAttr::IsSame( i3RenderAttr * pAttr)
{
	return ((m_Bind == ((i3TextureWrapAttr *)pAttr)->GetBindType()) &&
			(m_HorzWrap == ((i3TextureWrapAttr *)pAttr)->GetHorzWrap()) &&
			(m_VertWrap == ((i3TextureWrapAttr *)pAttr)->GetVertWrap()));
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED TEXTUREWRAP_INFO
	{
		UINT8		m_HorzWrap = 0;
		UINT8		m_VertWrap = 0;
		UINT16		m_Bind = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TextureWrapAttr::OnSave( i3ResourceFile * pResFile)
{
	pack::TEXTUREWRAP_INFO Info;
	UINT32 Rc;

	Info.m_HorzWrap = (UINT8) i3Gfx::Convert( m_HorzWrap);
	Info.m_VertWrap = (UINT8) i3Gfx::Convert( m_VertWrap);
	Info.m_Bind = (UINT16) m_Bind;

	Rc = pResFile->GetStream()->Write( &Info, sizeof(pack::TEXTUREWRAP_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureWrapAttr::OnSave()", "Could not write texture wrap information.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3TextureWrapAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3TextureWrapAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	pack::TEXTUREWRAP_INFO Info;
	UINT32 Rc;

	Rc = pResFile->GetStream()->Read( &Info, sizeof(pack::TEXTUREWRAP_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureWrapAttr::OnSave()", "Could not read texture wrap information.");
		return STREAM_ERR;
	}

	m_HorzWrap = i3Gfx::Convert( (I3G_PERSIST_TEXTURE_WRAP) Info.m_HorzWrap);
	m_VertWrap = i3Gfx::Convert( (I3G_PERSIST_TEXTURE_WRAP) Info.m_VertWrap);
	m_Bind =	(I3G_TEXTURE_BIND) Info.m_Bind;

	return Rc;
}

#if defined( I3_DEBUG)
void i3TextureWrapAttr::Dump(void)
{
	static char s_WrapStr[2][32] =
	{
		"Wrap",
		"Clamp"
	};
	INT32 idxHorz, idxVert;

	switch( m_HorzWrap)
	{
		case I3G_TEXTURE_WRAP_CLAMP :	idxHorz = 1;	break;
		default :						idxHorz = 0;	break;
	}

	switch( m_VertWrap)
	{
		case I3G_TEXTURE_WRAP_CLAMP :	idxVert = 1;	break;
		default :						idxVert = 0;	break;
	}

	I3TRACE( "%s : Horz:%s  Vert:%s\n", meta()->class_name(), s_WrapStr[idxHorz], s_WrapStr[idxVert]);
}
#endif
