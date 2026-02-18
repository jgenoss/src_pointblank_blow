#include "i3SceneDef.h"
#include "i3TextureBindAttr.h"

I3_CLASS_INSTANCE( i3TextureBindAttr);

i3TextureBindAttr::i3TextureBindAttr(void)
{
	SetID( I3_ATTRID_TEXTURE_BIND);

	m_BindType = static_cast<WORD>(I3G_TEXTURE_BIND_DIFFUSE);
}

i3TextureBindAttr::~i3TextureBindAttr(void)
{
	I3_SAFE_RELEASE(m_pTexture);
}

void i3TextureBindAttr::SetTexture( i3Texture * pTexture)
{
	I3_REF_CHANGE( m_pTexture, pTexture);
}

//
// Apply함수를 수정해서 해당 셋팅이 안되도록 막는게 가능해 보인다...일단은 툴용도이며..
// Attr의 메모리용량이 부담되지 않는다면 m_bApplyEnable만으로 처리를 종료할수도 있다...
// (2013.06.20.수빈)
// 반사매핑의 경우, 해당 메시 텍스쳐가 아닌 외부텍스쳐가 들어옴...일단은 레퍼런스 인덱스에서 끌 경우 함께 끄도록 처리해본다..
// 반사쪽은 다행이도 파생클래스가 존재하므로, 가상함수로 분리한다..

void i3TextureBindAttr::Apply( i3RenderContext * pContext)
{
	if (m_bChangeTex)
		pContext->SetTextureEnableByBindAttr(GetBindType(), m_bApplyEnable);

	if (m_bApplyEnable)
		pContext->SetTexture( GetBindType(), m_pTexture, m_bChangeTex);			// 일반적으로 처리해도 좋다..상단에 있는 Enable속성이 그대로 적용..
}

void i3TextureBindAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD )
{
	// Texture에 대한 복사는 항상 Reference 방식으로 한다.
	// 실제로 똑같은 Texture를 생성하고 싶어하는 경우가 있을까...???
	// 필요하면 다시 loading해서 써~~!!!
	((i3TextureBindAttr *) pObj)->SetTexture( GetTexture());
	((i3TextureBindAttr *) pObj)->SetBindType( GetBindType());
	((i3TextureBindAttr *) pObj)->m_skipped_tex_filename = m_skipped_tex_filename;		// 이것 필요하다..
	((i3TextureBindAttr *) pObj)->m_bApplyEnable = m_bApplyEnable;						// 이것 필요하다..
	((i3TextureBindAttr *) pObj)->m_bChangeTex = m_bChangeTex;
}

bool i3TextureBindAttr::IsSame( i3RenderAttr * pAttr)
{
	if (((i3TextureBindAttr *)pAttr)->IsTextureChange() != m_bChangeTex)
		return false;
	if (((i3TextureBindAttr *)pAttr)->IsApplyEnable() != m_bApplyEnable)
		return false;
	return ((m_BindType == ((i3TextureBindAttr *)pAttr)->GetBindType()) &&
		(m_pTexture == ((i3TextureBindAttr *) pAttr)->GetTexture()));
}

void i3TextureBindAttr::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pTexture != nullptr)
		List.push_back( m_pTexture);

	i3RenderAttr::OnBuildObjectList( List);
}

UINT32 i3TextureBindAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, val;
	OBJREF ref;
	i3Stream * pStream = pResFile->GetStream();

	// Texture Object ID
	ref = pResFile->GetObjectPersistID( m_pTexture);

	Rc = pStream->Write( &ref, sizeof(ref));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureBindAttr::OnSave()", "Could not write texture object Id.");
		return STREAM_ERR;
	}

	// Stage Index
	val = (UINT32) m_BindType;
	Rc = pStream->Write( &val, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureBindAttr::OnSave()", "Could not write stage index value.");
		return STREAM_ERR;
	}

	return sizeof(UINT32) + sizeof(UINT32);
}

UINT32 i3TextureBindAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3TextureBindAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, val;
	i3Stream * pStream = pResFile->GetStream();
	i3Texture * pTex;

	// Texture Object ID
	Rc = pStream->Read( &val, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureBindAttr::OnSave()", "Could not read texture object Id.");
		return STREAM_ERR;
	}

	if( val != 0)
	{
        pTex = (i3Texture *) pResFile->FindObjectByID( val);

		if ( pTex == nullptr && pResFile->IsCheckedExternNeedLoad() )
			m_skipped_tex_filename = pResFile->FindNameByID(val);
	}
	else
	{
		pTex = nullptr;
	}

	SetTexture( pTex);

	// Stage Index
	Rc = pStream->Read( &val, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureBindAttr::OnSave()", "Could not read stage index value.");
		return STREAM_ERR;
	}

	m_BindType = (WORD)((I3G_TEXTURE_BIND) val);

	return sizeof(UINT32) + sizeof(UINT32);
}

#if defined( I3_DEBUG)
void i3TextureBindAttr::Dump(void)
{
	if( m_pTexture == nullptr)
	{
		i3RenderAttr::Dump();
	}
	else
	{
		if( m_pTexture->hasName())
		{
			I3TRACE( "i3TextureBindAttr : %08X - %s\n", m_pTexture, m_pTexture->GetName());
		}
		else
		{
			I3TRACE( "i3TextureBindAttr : %08X\n", m_pTexture);
		}
	}
}
#endif
