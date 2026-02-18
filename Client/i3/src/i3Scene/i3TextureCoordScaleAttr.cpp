#include "i3SceneDef.h"
#include "i3TextureCoordScaleAttr.h"

//I3_CLASS_INSTANCE( i3TextureCoordScaleAttr, i3RenderAttr);
I3_CLASS_INSTANCE( i3TextureCoordScaleAttr);

i3TextureCoordScaleAttr::i3TextureCoordScaleAttr(void)
{
	SetID( I3_ATTRID_TEXTURE_COORD_SCALE);
}

void i3TextureCoordScaleAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3TextureCoordScaleAttr *) pObj)->SetBindType( GetBindType());
	((i3TextureCoordScaleAttr *) pObj)->SetOutputElementCount( GetOutputElementCount());
	((i3TextureCoordScaleAttr *) pObj)->SetScaleFactor( GetScaleFactor());
}

void i3TextureCoordScaleAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureCoordScale( GetBindType(), GetScaleFactor(), GetOutputElementCount());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED TEXTURE_COORD_SCALE_INFO
	{
		UINT8			m_BindType = 0;
		i3::pack::VEC3D	m_ScaleFactor;
		INT32			m_OutputElementCount = 0;
		UINT8			m_Reserved[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TextureCoordScaleAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	i3Stream * pStream = pResFile->GetStream();
	pack::TEXTURE_COORD_SCALE_INFO Info;

	Info.m_BindType = (UINT8) GetBindType();
	Info.m_OutputElementCount = GetOutputElementCount();
	Info.m_ScaleFactor.x = i3Vector::GetX( &m_ScaleFactor);
	Info.m_ScaleFactor.y = i3Vector::GetY( &m_ScaleFactor);
	Info.m_ScaleFactor.z = i3Vector::GetZ( &m_ScaleFactor);

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureCoordScaleAttr::OnSave()", "Could not write Texture Coord. Scale Information.");
	}
	
	return Rc;
}

UINT32 i3TextureCoordScaleAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3TextureCoordScaleAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	i3Stream * pStream = pResFile->GetStream();
	pack::TEXTURE_COORD_SCALE_INFO Info;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureCoordScaleAttr::OnLoad()", "Could not read Texture coord scale Information.");
	}

	SetBindType( (I3G_TEXTURE_BIND) Info.m_BindType);
	SetOutputElementCount( Info.m_OutputElementCount);
	i3Vector::Set( &m_ScaleFactor, Info.m_ScaleFactor.x, Info.m_ScaleFactor.y, Info.m_ScaleFactor.z);
		
	return Rc;
}

#if defined( I3_DEBUG)
void i3TextureCoordScaleAttr::Dump(void)
{
	I3TRACE( "%s : %f, %f, %f\n", meta()->class_name(), 
		i3Vector::GetX( GetScaleFactor()), 
		i3Vector::GetY( GetScaleFactor()), 
		i3Vector::GetZ( GetScaleFactor()));
}
#endif
