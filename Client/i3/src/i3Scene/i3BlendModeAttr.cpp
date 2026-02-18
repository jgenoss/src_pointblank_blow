#include "i3SceneDef.h"
#include "i3BlendModeAttr.h"

I3_CLASS_INSTANCE( i3BlendModeAttr);

i3BlendModeAttr::i3BlendModeAttr(void)
{
	SetID( I3_ATTRID_BLEND_MODE);
}

void i3BlendModeAttr::Apply( i3RenderContext * pContext)
{
	if( pContext->getSkipBlendModeAttr()) return;

	pContext->SetSrcBlend( m_SrcBlend);
	pContext->SetDestBlend( m_DestBlend);
	pContext->SetBlendOperation( m_BlendOp);
}

void i3BlendModeAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3BlendModeAttr * pDest = (i3BlendModeAttr *) pObj;

	i3RenderAttr::CopyTo( pObj, method);

	pDest->m_SrcBlend = m_SrcBlend;
	pDest->m_DestBlend = m_DestBlend;
	pDest->m_BlendOp = m_BlendOp;
}

bool i3BlendModeAttr::IsSame( i3RenderAttr * pAttr)
{
	i3BlendModeAttr * pAttr2 = (i3BlendModeAttr *) pAttr;

	return ((m_SrcBlend == pAttr2->GetSource()) && (m_DestBlend == pAttr2->GetDestination()));
}

UINT32	i3BlendModeAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	UINT8 val;
	i3Stream * pStream = pResFile->GetStream();

	val = (UINT8) i3Gfx::Convert( m_SrcBlend);
	Rc = pStream->Write( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BlendModeAttr::OnSave()", "Could not write source blend value.");
		return STREAM_ERR;
	}
	Result += Rc;

	val = (UINT8) i3Gfx::Convert( m_DestBlend);
	Rc = pStream->Write( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BlendModeAttr::OnSave()", "Could not write destination blend value.");
		return STREAM_ERR;
	}
	Result += Rc;

	val = (UINT8) i3Gfx::Convert( m_BlendOp);
	Rc = pStream->Write( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BlendModeAttr::OnSave()", "Could not write blend operation value.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3BlendModeAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3BlendModeAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	UINT8 val;
	i3Stream * pStream = pResFile->GetStream();

	Rc = pStream->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BlendModeAttr::OnLoad()", "Could not read source blend value.");
		return STREAM_ERR;
	}
	Result += Rc;

	m_SrcBlend = i3Gfx::Convert( (I3G_PERSIST_BLEND) val);

	Rc = pStream->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BlendModeAttr::OnLoad()", "Could not read destination blend value.");
		return STREAM_ERR;
	}
	Result += Rc;

	m_DestBlend = i3Gfx::Convert( (I3G_PERSIST_BLEND) val);

	Rc = pStream->Read( &val, sizeof(UINT8));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3BlendModeAttr::OnLoad()", "Could not read blend operation value.");
		return STREAM_ERR;
	}
	Result += Rc;

	m_BlendOp = i3Gfx::Convert( (I3G_PERSIST_BLEND_OP) val);

	return Result;
}

#if defined( I3_DEBUG)
void i3BlendModeAttr::Dump(void)
{
	char conv[128];

	strncpy( conv, "i3BlendModeAttr :", sizeof(conv) - 1);

	switch( GetSource())
	{
		default						:	strncat( conv, "SRC:ZERO  ", sizeof(conv)-1);	break;
		case I3G_BLEND_ONE			:	strncat( conv, "SRC:ONE  ", sizeof(conv)-1);	break;
		case I3G_BLEND_SRCCOLOR		:	strncat( conv, "SRC:SRCCOLOR  ", sizeof(conv)-1);	break;
		case I3G_BLEND_INVSRCCOLOR	:	strncat( conv, "SRC:INVSRCCOLOR  ", sizeof(conv)-1);	break;
		case I3G_BLEND_SRCALPHA		:	strncat( conv, "SRC:SRCALPHA  ", sizeof(conv)-1);	break;
		case I3G_BLEND_INVSRCALPHA	:	strncat( conv, "SRC:INVSRCALPHA  ", sizeof(conv)-1);	break;
		case I3G_BLEND_DESTALPHA	:	strncat( conv, "SRC:DESTALPHA  ", sizeof(conv)-1);	break;
		case I3G_BLEND_INVDESTALPHA	:	strncat( conv, "SRC:INVDESTALPHA  ", sizeof(conv)-1);	break;
		case I3G_BLEND_FACTOR :			strncat( conv, "SRC:FACTOR  ", sizeof(conv)-1);	break;
		case I3G_BLEND_INVFACTOR :		strncat( conv, "SRC:INVFACTOR  ", sizeof(conv)-1);	break;

		#if !defined( I3G_PSP)
			case I3G_BLEND_DESTCOLOR	:	strncat( conv, "SRC:DESTCOLOR  ", sizeof(conv)-1);	break;
			case I3G_BLEND_INVDESTCOLOR	:	strncat( conv, "SRC:INVDESTCOLOR  ", sizeof(conv)-1);	break;

			#if !defined( I3G_OGLES)
			case I3G_BLEND_SRCALPHASAT	:	strncat( conv, "SRC:SRCALPHASAT  ", sizeof(conv)-1);	break;
			#endif
		#endif
	}

	switch( GetDestination())
	{
		default						:	strncat( conv, "DEST:ZERO  ", sizeof(conv)-1);	break;
		case I3G_BLEND_ONE			:	strncat( conv, "DEST:ONE  ", sizeof(conv)-1);	break;
		case I3G_BLEND_SRCCOLOR		:	strncat( conv, "DEST:SRCCOLOR  ", sizeof(conv)-1);	break;
		case I3G_BLEND_INVSRCCOLOR	:	strncat( conv, "DEST:INVSRCCOLOR  ", sizeof(conv)-1);	break;
		case I3G_BLEND_SRCALPHA		:	strncat( conv, "DEST:SRCALPHA  ", sizeof(conv)-1);	break;
		case I3G_BLEND_INVSRCALPHA	:	strncat( conv, "DEST:INVSRCALPHA  ", sizeof(conv)-1);	break;
		case I3G_BLEND_DESTALPHA	:	strncat( conv, "DEST:DESTALPHA  ", sizeof(conv)-1);	break;
		case I3G_BLEND_INVDESTALPHA	:	strncat( conv, "DEST:INVDESTALPHA  ", sizeof(conv)-1);	break;
		case I3G_BLEND_FACTOR :			strncat( conv, "DEST:FACTOR  ", sizeof(conv)-1);	break;
		case I3G_BLEND_INVFACTOR :		strncat( conv, "DEST:INVFACTOR  ", sizeof(conv)-1);	break;

		#if !defined( I3G_PSP)
		case I3G_BLEND_DESTCOLOR	:	strncat( conv, "DEST:DESTCOLOR  ", sizeof(conv)-1);	break;
		case I3G_BLEND_INVDESTCOLOR	:	strncat( conv, "DEST:INVDESTCOLOR  ", sizeof(conv)-1);	break;

		#if !defined( I3G_OGLES)
		case I3G_BLEND_SRCALPHASAT	:	strncat( conv, "DEST:SRCALPHASAT  ", sizeof(conv)-1);	break;
		#endif
		#endif
	}

	I3TRACE( "%s\n", conv);
}
#endif
