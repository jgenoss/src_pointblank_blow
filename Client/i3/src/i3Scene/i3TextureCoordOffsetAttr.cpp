#include "i3SceneDef.h"
#include "i3TextureCoordOffsetAttr.h"

I3_CLASS_INSTANCE( i3TextureCoordOffsetAttr);

i3TextureCoordOffsetAttr::i3TextureCoordOffsetAttr(void)
{
	SetID( I3_ATTRID_TEXTURE_COORD_OFFSET);
}


void i3TextureCoordOffsetAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3TextureCoordOffsetAttr *) pObj)->SetBindType( GetBindType());
	((i3TextureCoordOffsetAttr *) pObj)->SetOutputElementCount( GetOutputElementCount());
	((i3TextureCoordOffsetAttr *) pObj)->SetOffset( GetOffset());
}

void i3TextureCoordOffsetAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureCoordOffset( GetBindType(), GetOffset(), GetOutputElementCount());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED TEXTURE_COORD_OFFSET_INFO
	{
		UINT8			m_BindType = 0;
		i3::pack::VEC3D	m_Offset;
		INT32			m_OutputElementCount = 0;
		UINT8			m_Reserved[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TextureCoordOffsetAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	i3Stream * pStream = pResFile->GetStream();
	pack::TEXTURE_COORD_OFFSET_INFO Info;

	Info.m_BindType = (UINT8) GetBindType();
	Info.m_OutputElementCount = GetOutputElementCount();
	Info.m_Offset.x = i3Vector::GetX( &m_Offset);
	Info.m_Offset.y = i3Vector::GetY( &m_Offset);
	Info.m_Offset.z = i3Vector::GetZ( &m_Offset);

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureCoordOffsetAttr::OnSave()", "Could not write Texture Coord. Scale Information.");
	}
	
	return Rc;
}

UINT32 i3TextureCoordOffsetAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3TextureCoordOffsetAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	i3Stream * pStream = pResFile->GetStream();
	pack::TEXTURE_COORD_OFFSET_INFO Info;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureCoordOffsetAttr::OnLoad()", "Could not read Texture coord scale Information.");
	}

	SetBindType( (I3G_TEXTURE_BIND) Info.m_BindType);
	SetOutputElementCount( Info.m_OutputElementCount);
	i3Vector::Set( &m_Offset, Info.m_Offset.x, Info.m_Offset.y, Info.m_Offset.z);
		
	return Rc;
}

#if defined( I3_DEBUG)
void i3TextureCoordOffsetAttr::Dump(void)
{
	I3TRACE( "%s : %f, %f, %f\n", meta()->class_name(), 
		i3Vector::GetX( GetOffset()), 
		i3Vector::GetY( GetOffset()), 
		i3Vector::GetZ( GetOffset()));
}
#endif
