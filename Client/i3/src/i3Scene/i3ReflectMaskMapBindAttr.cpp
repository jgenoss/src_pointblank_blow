#include "i3SceneDef.h"
#include "i3ReflectMaskMapBindAttr.h"

I3_CLASS_INSTANCE( i3ReflectMaskMapBindAttr);

i3ReflectMaskMapBindAttr::i3ReflectMaskMapBindAttr(void)
{
	SetID( I3_ATTRID_REFLECTMASKMAP_BIND);
	m_BindType = static_cast<WORD>(I3G_TEXTURE_BIND_REFLECT_MASK);
}


UINT32 i3ReflectMaskMapBindAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	OBJREF ref;
	i3Stream * pStream = pResFile->GetStream();

	// Texture Object ID
	ref = pResFile->GetObjectPersistID( m_pTexture);
	
	Rc = pStream->Write( &ref, sizeof(ref));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ReflectMaskMapBindAttr::OnSave()", "Could not write texture object Id.");
		return STREAM_ERR;
	}

	return sizeof(UINT32) + sizeof(UINT32);
}

UINT32 i3ReflectMaskMapBindAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3ReflectMaskMapBindAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, val;
	i3Stream * pStream = pResFile->GetStream();
	i3Texture * pTex;

	// Texture Object ID
	Rc = pStream->Read( &val, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ReflectMaskMapBindAttr::OnSave()", "Could not read texture object Id.");
		return STREAM_ERR;
	}

	if( val != 0)
	{
        pTex = (i3Texture *) pResFile->FindObjectByID( val);

		if ( pTex == nullptr && pResFile->IsCheckedExternNeedLoad() )
			m_skipped_tex_filename = pResFile->FindNameByID(val);

		//I3ASSERT( pTex != nullptr);		// 외부 파일의 경우 없을 수도 있다.
	}
	else
	{
		pTex = nullptr;
	}

	SetTexture( pTex);

	return sizeof(UINT32) + sizeof(UINT32);
}

void i3ReflectMaskMapBindAttr::Apply( i3RenderContext * pContext)
{
	if (m_bChangeTex)
	{
		pContext->SetTextureEnableByBindAttr( I3G_TEXTURE_BIND_REFLECT , m_bApplyEnable);
		pContext->SetTextureEnableByBindAttr( I3G_TEXTURE_BIND_REFLECT_MASK , m_bApplyEnable);
	}
	
	if (m_bApplyEnable)
		pContext->SetTexture( GetBindType(), m_pTexture, m_bChangeTex);			// 일반적으로 처리해도 좋다..상단에 있는 Enable속성이 그대로 적용..
}

#if defined( I3_DEBUG)
void i3ReflectMaskMapBindAttr::Dump(void)
{
	if( m_pTexture == nullptr)
	{
		i3RenderAttr::Dump();
	}
	else
	{
		if( m_pTexture->hasName())
		{
			I3TRACE( "i3ReflectMaskMapBindAttr : %08X - %s\n", m_pTexture, m_pTexture->GetName());
		}
		else
		{
			I3TRACE( "i3ReflectMaskMapBindAttr : %08X\n", m_pTexture);
		}
	}
}
#endif
