#include "i3SceneDef.h"
#include "i3FogNearFarAttr.h"

I3_CLASS_INSTANCE( i3FogNearFarAttr);

i3FogNearFarAttr::i3FogNearFarAttr(void)
{
	SetID( I3_ATTRID_FOG_NEARFAR);
}

void i3FogNearFarAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetFogNearFar( m_fNear, m_fFar);
}

void i3FogNearFarAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3FogNearFarAttr *) pObj)->SetNear( GetNear());
	((i3FogNearFarAttr *) pObj)->SetFar( GetFar());
}

bool i3FogNearFarAttr::IsSame( i3RenderAttr * pAttr)
{
	i3FogNearFarAttr * pAttr2 = (i3FogNearFarAttr *) pAttr;

	return (m_fNear == pAttr2->GetNear()) && (m_fFar == pAttr2->GetFar());
}

UINT32 i3FogNearFarAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;

	Rc = pResFile->GetStream()->Write( &m_fNear, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogNearFarAttr::OnSave()", "Could not write fog near value.");
		return STREAM_ERR;
	}

	Rc = pResFile->GetStream()->Write( &m_fFar, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogNearFarAttr::OnSave()", "Could not write fog far value.");
		return STREAM_ERR;
	}

	return sizeof(REAL32) + sizeof(REAL32);
}

UINT32 i3FogNearFarAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3FogNearFarAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;

	Rc = pResFile->GetStream()->Read( &m_fNear, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogNearFarAttr::OnLoad()", "Could not Read fog near value.");
		return STREAM_ERR;
	}

	Rc = pResFile->GetStream()->Read( &m_fFar, sizeof(REAL32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FogNearFarAttr::OnLoad()", "Could not Read fog far value.");
		return STREAM_ERR;
	}

	return sizeof(REAL32) + sizeof(REAL32);
}
