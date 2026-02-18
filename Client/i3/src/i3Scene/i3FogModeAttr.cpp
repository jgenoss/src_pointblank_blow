#include "i3SceneDef.h"
#include "i3FogModeAttr.h"

I3_CLASS_INSTANCE( i3FogModeAttr);

i3FogModeAttr::i3FogModeAttr(void)
{
	SetID( I3_ATTRID_FOG_MODE);
}

void i3FogModeAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetFogMode( m_Mode);
}

void i3FogModeAttr::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pDest, method);

	((i3FogModeAttr *) pDest)->Set( Get());
}

bool i3FogModeAttr::IsSame( i3RenderAttr * pAttr)
{
	return m_Mode == ((i3FogModeAttr *)pAttr)->Get();
}

UINT32 i3FogModeAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	UINT8 val;

	val = (UINT8) i3Gfx::Convert( m_Mode);

	Rc = pResFile->GetStream()->Write( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogModeAttr::OnSave()", "Could not write value.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3FogModeAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3FogModeAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	UINT8 val;

	Rc = pResFile->GetStream()->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogModeAttr::OnLoad()", "Could not read value.");
		return STREAM_ERR;
	}

	m_Mode = i3Gfx::Convert( (I3G_PERSIST_FOG_MODE) val);

	return Rc;
}
