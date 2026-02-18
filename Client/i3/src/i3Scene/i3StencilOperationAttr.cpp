#include "i3SceneDef.h"
#include "i3StencilOperationAttr.h"

I3_CLASS_INSTANCE( i3StencilOperationAttr);

i3StencilOperationAttr::i3StencilOperationAttr(void)
{
	SetID( I3_ATTRID_STENCIL_OPERATION);
}

void i3StencilOperationAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetStencilOperation( m_SFail, m_SPass, m_ZFail);
}

void i3StencilOperationAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3StencilOperationAttr * pDest = (i3StencilOperationAttr *) pObj;

	i3RenderAttr::CopyTo( pObj, method);

	pDest->SetSFail( GetSFail());
	pDest->SetSPass( GetSPass());
	pDest->SetZFail( GetZFail());
}

bool i3StencilOperationAttr::IsSame( i3RenderAttr * pAttr)
{
	i3StencilOperationAttr * pAttr2 = (i3StencilOperationAttr *) pAttr;

	return ((m_SFail == pAttr2->GetSFail()) &&
			(m_SPass == pAttr2->GetSPass()) &&
			(m_ZFail == pAttr2->GetZFail()));
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED STENCILOP_INFO
	{
		UINT8		m_SFail = 0;
		UINT8		m_SPass = 0;
		UINT8		m_ZFail = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3StencilOperationAttr::OnSave( i3ResourceFile * pResFile)
{
	pack::STENCILOP_INFO Info;
	UINT32 Rc;

	Info.m_SFail = (UINT8) i3Gfx::Convert( m_SFail);
	Info.m_SPass = (UINT8) i3Gfx::Convert( m_SPass);
	Info.m_ZFail = (UINT8) i3Gfx::Convert( m_ZFail);

	Rc = pResFile->GetStream()->Write( &Info, sizeof(pack::STENCILOP_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3StecilOperationAttr::ONSave()", "Could not write stecnil operation informations.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3StencilOperationAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3StencilOperationAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	pack::STENCILOP_INFO	Info;

	Rc = pResFile->GetStream()->Read( &Info, sizeof(pack::STENCILOP_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3StencilOperationAttr::OnLoad()", "Could not read stencil operation informations.");
		return STREAM_ERR;
	}
	
	m_SFail = i3Gfx::Convert( (I3G_PERSIST_STENCIL_OP) Info.m_SFail);
	m_SPass = i3Gfx::Convert( (I3G_PERSIST_STENCIL_OP) Info.m_SPass);
	m_ZFail = i3Gfx::Convert( (I3G_PERSIST_STENCIL_OP) Info.m_ZFail);

	return Rc;
}
