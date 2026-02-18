#include "i3SceneDef.h"
#include "i3RenderModeAttr.h"

I3_CLASS_INSTANCE( i3RenderModeAttr);

i3RenderModeAttr::i3RenderModeAttr(void)
{
	m_Mode = I3G_RENDER_POLYGON;

	SetID( I3_ATTRID_RENDER_MODE);
}


void i3RenderModeAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetRenderMode( m_Mode);
}

void i3RenderModeAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3RenderModeAttr *) pObj)->Set( Get());
}

bool i3RenderModeAttr::IsSame( i3RenderAttr * pAttr)
{
	return m_Mode == ((i3RenderModeAttr *) pAttr)->Get();
}
	
UINT32 i3RenderModeAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	UINT8 val;

	val = (UINT8) i3Gfx::Convert( m_Mode);

	Rc = pResFile->GetStream()->Write( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3RenderModeAttr::OnSave()", "Could not write render mode value.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3RenderModeAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3RenderModeAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	UINT8 val;

	Rc = pResFile->GetStream()->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3RenderModeAttr::OnLoad()", "Could not read render mode value.");
		return STREAM_ERR;
	}

	m_Mode = i3Gfx::Convert( (I3G_PERSIST_RENDER_MODE) val);

	return Rc;
}

#if defined( I3_DEBUG)
void i3RenderModeAttr::Dump(void)
{
	static char s_RenderModeStr[3][20] =
	{
		"Point",
		"Line",
		"Polygon",
	};
	INT32 idx;

	switch( m_Mode)
	{
		case I3G_RENDER_LINE	:	idx = 1;		break;
		case I3G_RENDER_POLYGON	:	idx = 2;		break;
		default :					idx = 0;		break;
	}

	I3TRACE( "%s : %s\n", meta()->class_name(), s_RenderModeStr[idx]);
}
#endif
