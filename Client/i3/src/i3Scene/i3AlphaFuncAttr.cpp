#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3AlphaFuncAttr.h"

I3_CLASS_INSTANCE( i3AlphaFuncAttr);

i3AlphaFuncAttr::i3AlphaFuncAttr(void)
{
	SetID( I3_ATTRID_ALPHA_FUNC);
}

void i3AlphaFuncAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetAlphaRefValue( m_AlphaRef);

	pContext->SetAlphaFunction( m_AlphaFunc);
}

void i3AlphaFuncAttr::CopyTo( i3ElementBase * pDestAttr, I3_COPY_METHOD method)
{
	i3AlphaFuncAttr * pDest = (i3AlphaFuncAttr *) pDestAttr;

	i3RenderAttr::CopyTo( pDestAttr, method);

	pDest->SetRefValue( GetRefValue());
	pDest->SetFunction( GetFunction());
}

bool i3AlphaFuncAttr::IsSame( i3RenderAttr * pAttr)
{
	i3AlphaFuncAttr * pAttr2 = (i3AlphaFuncAttr *) pAttr;

	return ((m_AlphaRef == pAttr2->m_AlphaRef) && (m_AlphaFunc == pAttr2->m_AlphaFunc));
}

UINT32 i3AlphaFuncAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	UINT8	pstComp;
	i3Stream * pStream = pResFile->GetStream();

	// Alpha Reference Value
	Rc = pStream->Write( &m_AlphaRef, 4 ); // 이거 버그임 !!! 그러나, 일단 기존 버전과 호환성 유지를 위해서 남겨둠.
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3AlphaFuncAttr::OnSave()", "Could not write alpha reference value.");
		return STREAM_ERR;
	}
	Result = Rc;

	// Alpha Function value
	pstComp = (UINT8) i3Gfx::Convert( m_AlphaFunc);

	Rc = pStream->Write( &pstComp, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3AlphaFuncAttr::OnSave()", "Could not write alpha function value.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3AlphaFuncAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3AlphaFuncAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	UINT8	pstComp;
	i3Stream * pStream = pResFile->GetStream();

	// Alpha Reference Value
	UINT32 nTemp;
	Rc = pStream->Read( &nTemp, 4 ); // 이거 버그임 !!! 그러나, 일단 기존 버전과 호환성 유지를 위해서 남겨둠.
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3AlphaFuncAttr::OnLoad()", "Could not read alpha reference value.");
		return STREAM_ERR;
	}
	m_AlphaRef = *((UINT8*)&nTemp);
	Result = Rc;

	// Alpha Function value
	Rc = pStream->Read( &pstComp, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3AlphaFuncAttr::OnLoad()", "Could not read alpha function value.");
		return STREAM_ERR;
	}
	Result += Rc;

	m_AlphaFunc = i3Gfx::Convert( (I3G_PERSIST_COMP_FUNC) pstComp);

	//I3TRACE( "i3AlphaFuncAttr::OnLoad()\n" );
	//I3TRACE( "m_AlphaRef = %d\n", m_AlphaRef );
	//I3TRACE( "pstComp = %d\n", pstComp );
	//I3TRACE( "m_AlphaFunc = %d\n", m_AlphaFunc );
	return Result;
}

#if defined( I3_DEBUG)
void i3AlphaFuncAttr::Dump(void)
{
}
#endif
