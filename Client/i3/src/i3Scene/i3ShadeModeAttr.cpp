#include "i3SceneDef.h"
#include "i3ShadeModeAttr.h"

I3_CLASS_INSTANCE( i3ShadeModeAttr);

i3ShadeModeAttr::i3ShadeModeAttr(void)
{
	SetID( I3_ATTRID_SHADE_MODE);
}


void i3ShadeModeAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetShadeMode( m_Mode);
}

void i3ShadeModeAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3ShadeModeAttr *) pObj)->Set( Get());
}

bool i3ShadeModeAttr::IsSame( i3RenderAttr * pAttr)
{
	return m_Mode == ((i3ShadeModeAttr *)pAttr)->Get();
}

UINT32 i3ShadeModeAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	UINT8 val;

	val = (UINT8) i3Gfx::Convert( m_Mode);

	Rc = pResFile->GetStream()->Write( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ShadeModeAttr::OnSave()", "Could not write shade mode value.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3ShadeModeAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3ShadeModeAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	UINT8 val;

	Rc = pResFile->GetStream()->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ShadeModeAttr::OnLoad()", "Could not read shade mode value.");
		return STREAM_ERR;
	}

#if defined (I3G_DX)
	m_Mode = i3Gfx::Convert( (I3G_PERSIST_SHADE_MODE) val);
#endif

	return Rc;
}
