#include "i3SceneDef.h"
#include "i3FogDensityAttr.h"

I3_CLASS_INSTANCE( i3FogDensityAttr);

i3FogDensityAttr::i3FogDensityAttr(void)
{
	SetID( I3_ATTRID_FOG_DENSITY);
}

void i3FogDensityAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetFogDensity( m_fDensity);
}

void i3FogDensityAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3FogDensityAttr *) pObj)->Set( Get());
}

bool i3FogDensityAttr::IsSame( i3RenderAttr * pAttr)
{
	return m_fDensity == ((i3FogDensityAttr *)pAttr)->Get();
}

UINT32 i3FogDensityAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;

	Rc = pResFile->GetStream()->Write( &m_fDensity, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogDensityAttr::OnSave()", "Could not write value.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3FogDensityAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3FogDensityAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;

	Rc = pResFile->GetStream()->Read( &m_fDensity, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogDensityAttr::OnLoad()", "Could not read value.");
		return STREAM_ERR;
	}

	return Rc;
}
