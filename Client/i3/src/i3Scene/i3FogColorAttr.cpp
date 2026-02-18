#include "i3SceneDef.h"
#include "i3Math.h"
#include "i3FogColorAttr.h"

I3_CLASS_INSTANCE( i3FogColorAttr);

i3FogColorAttr::i3FogColorAttr(void)
{
	SetID( I3_ATTRID_FOG_COLOR);
}

void i3FogColorAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetFogColor( &m_Color);
}

void i3FogColorAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3FogColorAttr *) pObj)->Set( Get());
}

bool i3FogColorAttr::IsSame( i3RenderAttr * pAttr)
{
	return i3Color::IsSame( &m_Color, ((i3FogColorAttr *)pAttr)->Get());
}

UINT32 i3FogColorAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc;
	UINT32 col;

	col = i3Color::ConvertRGBA32( &m_Color);

	Rc = pResFile->GetStream()->Write( &col, sizeof(col));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogColorAttr::OnSave()", "Could not write color value.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3FogColorAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3FogColorAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	UINT32 col;

	Rc = pResFile->GetStream()->Read( &col, sizeof(col));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogColorAttr::OnLoad()", "Could not read color value.");
		return STREAM_ERR;
	}

	i3Color::ConvertRGBA32( col, &m_Color);

	return Rc;
}
