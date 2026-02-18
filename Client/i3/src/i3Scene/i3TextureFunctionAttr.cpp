#include "i3SceneDef.h"
#include "i3TextureFunctionAttr.h"

I3_CLASS_INSTANCE( i3TextureFunctionAttr);

i3TextureFunctionAttr::i3TextureFunctionAttr(void)
{
	SetID( I3_ATTRID_TEXTURE_FUNCTION);
}


void i3TextureFunctionAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3TextureFunctionAttr *)pObj)->SetBindType( GetBindType());
	((i3TextureFunctionAttr *)pObj)->SetFunction( GetFunction());
}

void i3TextureFunctionAttr::Apply( i3RenderContext * pContext)
{
	//pContext->SetTextureFunction( GetBindType(), GetFunction());
}

bool i3TextureFunctionAttr::IsSame( i3RenderAttr * pAttr)
{
	return (((i3TextureFunctionAttr *)pAttr)->GetBindType() == GetBindType()) &&
			(((i3TextureFunctionAttr *)pAttr)->GetFunction() == GetFunction());
}

UINT32 i3TextureFunctionAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT8 val;

	val = (UINT8) GetBindType();
	Rc = pStream->Write( &val, sizeof(val));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureFunctionAttr::OnSave()", "Could not write texture stage index.");
		return STREAM_ERR;
	}
	Result += Rc;

	val = (UINT8)i3Gfx::Convert( GetFunction() );
	Rc = pStream->Write( &val, sizeof(val));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureFunctionAttr::OnLoad()", "Could not write texture function.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3TextureFunctionAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3TextureFunctionAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT8 val;

	Rc = pStream->Read( &val, sizeof(val));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureFunctionAttr::OnLoad()", "Could not read texture stage index.");
		return STREAM_ERR;
	}
	Result += Rc;
	SetBindType( (I3G_TEXTURE_BIND) val);

	Rc = pStream->Read( &val, sizeof(val));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureFunctionAttr::OnLoad()", "Could not read texture function.");
		return STREAM_ERR;
	}
	Result += Rc;
	I3G_TEXTURE_FUNCTION func = i3Gfx::Convert( I3G_PERSIST_TEXTURE_FUNCTION( val ) );
	SetFunction( func );

	return Result;
}

#if defined( I3_DEBUG)
static char s_TexFuncString[][30] = 
{
	"Replace",
	"Modulate",
	"Add",
	"Specular Highlight"
};

void i3TextureFunctionAttr::Dump(void)
{
	I3TRACE( "%s : %d, %s\n", meta()->class_name(), GetBindType(), s_TexFuncString[ GetFunction()]);
}
#endif
