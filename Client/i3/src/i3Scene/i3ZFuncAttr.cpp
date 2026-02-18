#include "i3SceneDef.h"
#include "i3ZFuncAttr.h"

I3_CLASS_INSTANCE( i3ZFuncAttr);

i3ZFuncAttr::i3ZFuncAttr(void)
{
	SetID( I3_ATTRID_Z_FUNC);
}

void i3ZFuncAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetZFunction( m_Function);
}

void i3ZFuncAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3ZFuncAttr *) pObj)->Set( Get());
}

bool i3ZFuncAttr::IsSame( i3RenderAttr * pAttr)
{
	return m_Function == ((i3ZFuncAttr *) pAttr)->Get();
}

UINT32 i3ZFuncAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	UINT8 val;

	val = (UINT8) i3Gfx::Convert( m_Function);

	Rc = pResFile->GetStream()->Write( &val, sizeof(UINT8 ));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ZFuncAttr::OnSave()", "Could not write depth function value.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32	i3ZFuncAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32	i3ZFuncAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	UINT8 val;

	Rc = pResFile->GetStream()->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ZFuncAttr::OnLoad()", "Could not read depth function value.");
		return STREAM_ERR;
	}

	m_Function = i3Gfx::Convert( (I3G_PERSIST_COMP_FUNC) val);

	return Rc;
}

#if defined( I3_DEBUG)
void i3ZFuncAttr::Dump(void)
{
	char conv[64];

	switch( m_Function)
	{
		case I3G_COMP_NEVER		:	strncpy( conv, "NEVER", sizeof(conv)-1);		break;
		case I3G_COMP_LESS		:	strncpy( conv, "LESS", sizeof(conv)-1);		break;
		case I3G_COMP_EQUAL		:	strncpy( conv, "EQUAL", sizeof(conv)-1);		break;
		case I3G_COMP_LEQUAL	:	strncpy( conv, "LEQUAL", sizeof(conv)-1);		break;
		case I3G_COMP_GREATER	:	strncpy( conv, "GREATER", sizeof(conv)-1);	break;
		case I3G_COMP_NOTEQUAL	:	strncpy( conv, "NOTEQUAL", sizeof(conv)-1);	break;
		case I3G_COMP_GEQUAL	:	strncpy( conv, "GEQUAL", sizeof(conv)-1);		break;
		case I3G_COMP_ALWAYS	:	strncpy( conv, "ALWAYS", sizeof(conv)-1);		break;
		default :					strncpy( conv, "Unknown comparision function", sizeof(conv)-1);	break;
	}

	I3TRACE( "i3ZFuncAttr : %s\n", conv);
}
#endif
