#include "i3SceneDef.h"
#include "i3NormalMapBindAttr.h"

I3_CLASS_INSTANCE( i3NormalMapBindAttr);

i3NormalMapBindAttr::i3NormalMapBindAttr(void)
{
	SetID( I3_ATTRID_NORMALMAP_BIND);

	m_BindType = static_cast<WORD>(I3G_TEXTURE_BIND_NORMAL);
}

UINT32 i3NormalMapBindAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	OBJREF ref;
	i3Stream * pStream = pResFile->GetStream();

	// Texture Object ID
	ref = pResFile->GetObjectPersistID( m_pTexture);
	
	Rc = pStream->Write( &ref, sizeof(ref));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NormalMapBindAttr::OnSave()", "Could not write texture object Id.");
		return STREAM_ERR;
	}

	return sizeof(UINT32) + sizeof(UINT32);
}

UINT32 i3NormalMapBindAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3NormalMapBindAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, val;
	i3Stream * pStream = pResFile->GetStream();
	i3Texture * pTex;

	// Texture Object ID
	Rc = pStream->Read( &val, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NormalMapBindAttr::OnSave()", "Could not read texture object Id.");
		return STREAM_ERR;
	}

	if( val != 0)
	{
        pTex = (i3Texture *) pResFile->FindObjectByID( val);

		if ( pTex == nullptr)
		{
			if( pResFile->IsCheckedExternNeedLoad() )
				m_skipped_tex_filename = pResFile->FindNameByID(val);
		}
		else
		{
			// Normal Map사용시 i3Texture의 UsageFlag설정
			pTex->addTextureFlag( I3I_TEXFLAG_NORMAL);
		}
	}
	else
	{
		pTex = nullptr;
	}

	SetTexture( pTex);
	
	return sizeof(UINT32) + sizeof(UINT32);
}

#if defined( I3_DEBUG)
void i3NormalMapBindAttr::Dump(void)
{
	if( m_pTexture == nullptr)
	{
		i3RenderAttr::Dump();
	}
	else
	{
		if( m_pTexture->hasName())
		{
			I3TRACE( "i3NormalMapBindAttr : %08X - %s\n", m_pTexture, m_pTexture->GetName());
		}
		else
		{
			I3TRACE( "i3NormalMapBindAttr : %08X\n", m_pTexture);
		}
	}
}
#endif
