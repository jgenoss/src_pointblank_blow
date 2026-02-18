#include "i3SceneDef.h"
#include "i3TextureCoordSourceAttr.h"

I3_CLASS_INSTANCE( i3TextureCoordSourceAttr);

i3TextureCoordSourceAttr::i3TextureCoordSourceAttr(void)
{
	SetID( I3_ATTRID_TEXTURE_COORD_SOURCE);
}


void i3TextureCoordSourceAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3TextureCoordSourceAttr * pAttr = (i3TextureCoordSourceAttr *) pObj;

	i3RenderAttr::CopyTo( pObj, method);

	pAttr->SetBindType( GetBindType());
	pAttr->SetMode( GetMode());
}

void i3TextureCoordSourceAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetTextureCoordSource( GetBindType(), GetMode());
}

bool i3TextureCoordSourceAttr::IsSame( i3RenderAttr * pAttr)
{
	i3TextureCoordSourceAttr * pDest = (i3TextureCoordSourceAttr *) pAttr;

	return ((pDest->GetBindType() == GetBindType()) && (pDest->GetMode() == GetMode()));
}

UINT32 i3TextureCoordSourceAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT8 val;

	// Stage Index
	val = (UINT8) GetBindType();
	Rc = pStream->Write( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureCoordSourceAttr::OnSave()", "Could not write texture stage index.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Texture Coord Mode
	val = (UINT8) i3Gfx::Convert( GetMode());
	Rc = pStream->Write( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureCoordSourceAttr::OnSave()", "Could not write texture coordinates source mode");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3TextureCoordSourceAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3TextureCoordSourceAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT8 val;

	Rc = pStream->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureCoordSourceAttr::OnLoad()", "Could not read texture stage index.");
		return STREAM_ERR;
	}
	Result += Rc;

	SetBindType( (I3G_TEXTURE_BIND) val);

	Rc = pStream->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureCoordSourceAttr::OnLoad()", "Could not read texture coordinates source mode.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

#if defined( I3_DEBUG)
static char s_TexCoordSourceString[][40] =
{
	"Vertex",
	"Camera-Space Position",
	"Camera-Space Normal",
	"Camera-Space Reflection Vector"
};

void i3TextureCoordSourceAttr::Dump(void)
{
	INT32 idx;

	switch( GetMode())
	{
		default :													idx = 0;	break;
		case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_POSITION	:		idx = 1;	break;
		case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_NORMAL	:		idx = 2;	break;
		case I3G_TEXTURE_COORD_SOURCE_CAMERA_SPACE_REFLECTION	:	idx = 3;	break;
	}

	I3TRACE( "%s : %d, %s\n", static_meta()->class_name(),
		GetBindType(),
		s_TexCoordSourceString[idx]);
}
#endif
