#include "i3SceneDef.h"
#include "i3RenderTargetAttr.h"

I3_CLASS_INSTANCE( i3RenderTargetAttr);

i3RenderTargetAttr::i3RenderTargetAttr(void)
{
	SetID( I3_ATTRID_RENDERTARGET);
}

i3RenderTargetAttr::~i3RenderTargetAttr(void)
{
	I3_SAFE_RELEASE( m_pRT);
	I3_SAFE_RELEASE( m_pDepthRT);
}

void i3RenderTargetAttr::BindRenderTarget( i3Texture * pColorRT, i3Texture * pDepthRT)
{
	I3_REF_CHANGE( m_pRT, pColorRT);
	I3_REF_CHANGE( m_pDepthRT, pDepthRT);

	if( m_pRT != nullptr)
	{
		m_Width = m_pRT->GetWidth();
		m_Height = m_pRT->GetHeight();
		m_Format = m_pRT->GetFormat();
	}
	else
	{
		m_Width = m_Height = 0;
		m_Format = I3G_IMAGE_FORMAT_DEFAULT_COLOR;
	}

	if( m_pDepthRT)
	{
		m_DepthFormat = m_pDepthRT->GetFormat();
	}
	else
	{
		m_DepthFormat = I3G_IMAGE_FORMAT_D24X8;
	}
}

void i3RenderTargetAttr::CreateRenderTarget(void)
{
	I3_SAFE_RELEASE( m_pRT);

	{
		m_pRT = i3Texture::new_object();
		I3ASSERT( m_pRT != nullptr);
#if defined( I3_DEBUG)
		m_pRT->SetName( "RenderTargetTex");
#endif

		if( m_Format != I3G_IMAGE_FORMAT_NONE)
			m_pRT->Create( m_Width, m_Height, 1, m_Format, I3G_USAGE_RENDERTARGET);
	}

	{
		m_pDepthRT = i3Texture::new_object();
		I3ASSERT( m_pDepthRT != nullptr);
#if defined( I3_DEBUG)
		m_pDepthRT->SetName( "DepthRenderTarget");
#endif

		if( m_DepthFormat != I3G_IMAGE_FORMAT_NONE)
			m_pDepthRT->Create( m_Width, m_Height, 1, m_DepthFormat, 0);
	}
}

void i3RenderTargetAttr::CreateRenderTarget( INT32 Width, INT32 Height, I3G_IMAGE_FORMAT format, I3G_IMAGE_FORMAT depthFormat)
{
	SetWidth( Width);
	SetHeight( Height);
	SetFormat( format);
	SetDepthFormat( depthFormat);

	CreateRenderTarget();
}

void i3RenderTargetAttr::SetColorRenderTarget( i3Texture * pTex)
{
	I3_REF_CHANGE( m_pRT, pTex);
}

void i3RenderTargetAttr::SetDepthRenderTarget( i3Texture * pTex)
{
	I3_REF_CHANGE( m_pDepthRT, pTex);
}

void i3RenderTargetAttr::Apply( i3RenderContext * pContext)
{
	if((m_Mode == I3SG_RENDERTARGET_MODE_CREATE) && (m_pRT == nullptr))
	{
		CreateRenderTarget();
	}

	if( m_pRT == nullptr)
	{
		// Restore to original render target.
		pContext->SetCurrentRenderTarget( (i3RenderTarget *) nullptr);
	}
	else
	{
		pContext->SetColorRenderTarget( m_pRT);

		if( m_pDepthRT != nullptr)
		{
			pContext->SetDepthRenderTarget( m_pDepthRT);
		}
	}
}

void i3RenderTargetAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	if( i3::same_of<i3RenderTargetAttr*>(pObj))
	{
		i3RenderTargetAttr * pDest = (i3RenderTargetAttr *) pObj;
		
		pDest->SetWidth( GetWidth());
		pDest->SetHeight( GetHeight());
		pDest->SetFormat( GetFormat());
		pDest->SetDepthFormat( GetDepthFormat());
		pDest->SetMode( GetMode());

		if( method == I3_COPY_INSTANCE)
		{
			pDest->CreateRenderTarget();
		}
		else
		{
			pDest->SetColorRenderTarget( m_pRT);
			pDest->SetDepthRenderTarget( m_pDepthRT);
		}
	}
}

bool i3RenderTargetAttr::IsSame( i3RenderAttr * pAttr)
{
	if( !i3::same_of<i3RenderTargetAttr*>(pAttr))
		return false;

	return m_pRT == ((i3RenderTargetAttr *) pAttr)->m_pRT;
}

void i3RenderTargetAttr::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( GetMode() == I3SG_RENDERTARGET_MODE_SET)
	{
		if( m_pRT != nullptr)
		{
			m_pRT->OnBuildObjectList( List);
		}

		if( m_pDepthRT != nullptr)
		{
			m_pDepthRT->OnBuildObjectList( List);
		}
	}

	i3RenderAttr::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PACKED RENDERTARGET_ATTR
	{
		UINT8		m_ID[4] = { 'R', 'T', 'A', '2' };
		INT32		m_Width = 0;
		INT32		m_Height = 0;
		INT32		m_Format = 0;
		INT32		m_DepthFormat = 0;
		INT32		m_Mode = 0;
		OBJREF		m_RT = 0;
		OBJREF		m_DepthRT = 0;
		UINT8		pad[16] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3RenderTargetAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::RENDERTARGET_ATTR Data;

	Result = i3RenderAttr::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Data.m_Width = GetWidth();
	Data.m_Height = GetHeight();
	Data.m_Format = GetFormat();
	Data.m_DepthFormat = GetDepthFormat();
	Data.m_Mode = GetMode();

	if( GetMode() == I3SG_RENDERTARGET_MODE_SET)
	{
		Data.m_RT		= (OBJREF) pResFile->GetObjectPersistID( m_pRT);
		Data.m_DepthRT	= (OBJREF) pResFile->GetObjectPersistID( m_pDepthRT);
	}

	Rc = pStream->Write( &Data, sizeof( Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3RenderTargetAttr::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3RenderTargetAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3RenderTargetAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::RENDERTARGET_ATTR Data;

	Result = i3RenderAttr::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3RenderTargetAttr::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	SetWidth( Data.m_Width);
	SetHeight( Data.m_Height);
	SetMode( (I3SG_RENDERTARGET_MODE) Data.m_Mode);
	SetFormat( (I3G_IMAGE_FORMAT) Data.m_Format);
	SetDepthFormat( (I3G_IMAGE_FORMAT) Data.m_DepthFormat);

	if( GetMode() == I3SG_RENDERTARGET_MODE_SET)
	{
		i3Texture * pTex;

		if( Data.m_RT != 0)
		{
			pTex = (i3Texture *) pResFile->FindObjectByID( Data.m_RT);
			I3ASSERT( pTex != nullptr);

			SetColorRenderTarget( pTex);
		}

		if( Data.m_DepthRT != 0)
		{
			pTex = (i3Texture *) pResFile->FindObjectByID( Data.m_DepthRT);
			I3ASSERT( pTex != nullptr);

			SetDepthRenderTarget( pTex);
		}
	}

	return Result;
}

#if defined( I3_DEBUG)
void i3RenderTargetAttr::Dump(void)
{
	I3TRACE( "i3RenderTargetAttr : W(%d), H(%d)\n", GetWidth(), GetHeight());
}
#endif
