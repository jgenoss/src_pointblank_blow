#include "i3SceneDef.h"
#include "i3StencilFuncAttr.h"

I3_CLASS_INSTANCE( i3StencilFuncAttr);

i3StencilFuncAttr::i3StencilFuncAttr(void)
{
	SetID( I3_ATTRID_STENCIL_FUNC);
}


void i3StencilFuncAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetStencilFunction( m_Func);

	pContext->SetStencilRefValue( m_StencilRef);

	pContext->SetStencilReadMask( m_ReadMask);

	pContext->SetStencilWriteMask( m_WriteMask);
}

void i3StencilFuncAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3StencilFuncAttr * pDest = (i3StencilFuncAttr *) pObj;

	i3ElementBase::CopyTo( pObj, method);

	pDest->SetFunction( GetFunction());
	pDest->SetReadMask( GetReadMask());
	pDest->SetWriteMask( GetWriteMask());
	pDest->SetRefValue( GetRefValue());
}

bool i3StencilFuncAttr::IsSame( i3RenderAttr * pAttr)
{
	i3StencilFuncAttr * pAttr2 = (i3StencilFuncAttr *) pAttr;

	return ((m_Func == pAttr2->GetFunction()) &&
		(m_StencilRef == pAttr2->GetRefValue()) &&
		(m_ReadMask == pAttr2->GetReadMask()) &&
		(m_WriteMask == pAttr2->GetWriteMask()));
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED STENCILFUNC_INFO
	{
		UINT8		m_Func = 0;
		UINT8		m_RefValue = 0;
		UINT8		m_ReadMask = 0;
		UINT8		m_WriteMask = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3StencilFuncAttr::OnSave( i3ResourceFile * pResFile)
{
	pack::STENCILFUNC_INFO Info;
	UINT32 Rc;

	Info.m_Func = (UINT8) i3Gfx::Convert( m_Func);
	Info.m_RefValue = m_StencilRef;
	Info.m_ReadMask = m_ReadMask;
	Info.m_WriteMask = m_WriteMask;

	Rc = pResFile->GetStream()->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3StencilFuncAttr::OnSave()", "Could not write stencil function information.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3StencilFuncAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3StencilFuncAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	pack::STENCILFUNC_INFO Info;
	UINT32 Rc;

	Rc = pResFile->GetStream()->Read( &Info, sizeof(pack::STENCILFUNC_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3StencilFuncAttr::OnLoad()", "Could not read stencil function information.");
		return STREAM_ERR;
	}

	m_Func = i3Gfx::Convert( (I3G_PERSIST_COMP_FUNC) Info.m_Func);
	m_StencilRef = Info.m_RefValue;
	m_ReadMask = Info.m_ReadMask;
	m_WriteMask = Info.m_WriteMask;

	return Rc;
}
