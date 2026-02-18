#include "i3SceneDef.h"
#include "i3AbstractBoolAttr.h"

I3_CLASS_INSTANCE( i3AbstractBoolAttr);

bool i3AbstractBoolAttr::IsSame( i3RenderAttr * pAttr)
{
	return m_bState == ((i3AbstractBoolAttr *) pAttr)->Get();
}

UINT32 i3AbstractBoolAttr::OnSave( i3ResourceFile * pResFile)
{ 
	UINT32 Rc;
	UINT8	val;

	if( m_bState)
		val = 1;
	else
		val = 0;

	Rc = pResFile->GetStream()->Write( &val, sizeof(val));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3AbstractBoolAttr::OnSave()", "Could not write boolean value.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3AbstractBoolAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3AbstractBoolAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	UINT8 val;

	Rc = pResFile->GetStream()->Read( &val, sizeof(val));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3AbstractBoolAttr::OnLoad()", "Could not read boolean value.");
		return STREAM_ERR;
	}

	if( val == 0)
		m_bState = false;
	else
		m_bState = true;

	return Rc;
}

void i3AbstractBoolAttr::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3AbstractBoolAttr* >(pDest));

	((i3AbstractBoolAttr *)pDest)->Set( Get());
}

#if defined( I3_DEBUG)
void i3AbstractBoolAttr::Dump(void)
{
	static char s_BoolStr[2][10] =
	{
		"FALSE",
		"TRUE"
	};

	I3TRACE( "%s : %s\n", meta()->class_name(), s_BoolStr[ m_bState]);
}
#endif

