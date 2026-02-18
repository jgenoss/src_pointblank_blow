#include "i3SceneDef.h"
#include "i3FaceCullModeAttr.h"

I3_CLASS_INSTANCE( i3FaceCullModeAttr);

i3FaceCullModeAttr::i3FaceCullModeAttr(void)
{
	SetID( I3_ATTRID_FACECULL_MODE);
}

void i3FaceCullModeAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetFaceCullMode( m_Mode);
}

void i3FaceCullModeAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3FaceCullModeAttr *)pObj)->Set( Get());
}

bool i3FaceCullModeAttr::IsSame( i3RenderAttr * pAttr)
{
	return (m_Mode == ((i3FaceCullModeAttr *)pAttr)->Get());
}

UINT32 i3FaceCullModeAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	UINT8 val;

	val = (UINT8) i3Gfx::Convert( m_Mode);

	Rc = pResFile->GetStream()->Write( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FaceCullModeAttr::OnSave()", "Could not write Face Cull Mode value.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3FaceCullModeAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3FaceCullModeAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	UINT8 val;

	Rc = pResFile->GetStream()->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3FaceCullModeAttr::OnLoad()", "Could not read Face Cull Mode value.");
		return STREAM_ERR;
	}

	m_Mode = i3Gfx::Convert( (I3G_PERSIST_FACE_CULL) val);

	return Rc;
}

#if defined( I3_DEBUG)
void i3FaceCullModeAttr::Dump(void)
{
	static char s_CullStr[3][10] = 
	{
		"None",
		"Back",
		"Front"
	};
	INT32 idx;

	switch( m_Mode)
	{
		case I3G_FACE_CULL_BACK :		idx = 1;	break;
		case I3G_FACE_CULL_FRONT :		idx = 2;	break;
		default :						idx = 0;	break;
	}

	I3TRACE( "%s : %s\n", meta()->class_name(), s_CullStr[ idx]);
}
#endif
