#include "i3GfxType.h"
#include "i3ShaderCache.h"
#include "i3PostProcessManager.h"
#include "i3PostProcessNightVision.h"

I3_CLASS_INSTANCE(i3PostProcessNightVision);

i3PostProcessNightVision::i3PostProcessNightVision()
{
	m_maskTextureList.reserve(2);
	
	m_pCtx.setWrap(I3G_TEXTURE_WRAP_WRAP);
}

i3PostProcessNightVision::~i3PostProcessNightVision()
{
	I3_SAFE_RELEASE(m_pShader);
	I3_SAFE_RELEASE(m_pVA);

	for(size_t i = 0; i < m_maskTextureList.size(); i++)
	{
		i3Texture * pTex = m_maskTextureList[i];
		I3_SAFE_RELEASE(pTex);
	}
	m_maskTextureList.clear();
}

void i3PostProcessNightVision::Create(i3PostProcessManager * pManager)
{

	i3Shader* pNewShader = i3ShaderCache::LoadShader("i3Nightvision.hlsl");		// ADDREF°¡ ¾øÀ½..
	I3_REF_CHANGE(m_pShader, pNewShader);

	{
		VEC3D _pos[4];
		VEC2D _tex[4];
		VEC2D _tex2[4];
		i3VertexFormat format;
		REAL32 divide = g_pRenderContext->getWindowHeight() / 32.0f;

		if(m_pVA == nullptr)
		{
			m_pVA = i3VertexArray::new_object();		
			I3ASSERT( m_pVA != nullptr);

			format.SetHasPosition( true);
			format.SetHasColor( false);
			format.SetTextureCoordSetCount( 2);
			format.SetTextureCoordCount( 2);	

			m_pVA->setRestoreType( I3G_RESTORE_MEM);
			m_pVA->Create( &format, 4, 0);

			{
				//	Position
				i3Vector::Set( &_pos[0], -1.0f, 1.0f, 0.0f);
				i3Vector::Set( &_pos[1], -1.0f, -1.0f, 0.0f);
				i3Vector::Set( &_pos[2], 1.0f, 1.0f, 0.0f);
				i3Vector::Set( &_pos[3], 1.0f, -1.0f, 0.0f);

				//	Texture
				i3Vector::Set( &_tex[0], 0.0f, 0.0f);
				i3Vector::Set( &_tex[1], 0.0f, 1.0f);
				i3Vector::Set( &_tex[2], 1.0f, 0.0f);			
				i3Vector::Set( &_tex[3], 1.0f, 1.0f);

				// Texture2
				INT32 i;

				for( i = 0; i < 4; i++)
				{
					i3Vector::Scale( &_tex2[i], &_tex[i], divide);
				}
			}

			m_pVA->Lock( 0);
			m_pVA->SetPositions( 0, 4, _pos);
			m_pVA->SetTextureCoords( 0, 0, 4, _tex);
			m_pVA->SetTextureCoords( 1, 0, 4, _tex2);
			m_pVA->Unlock();
		}
	}
}

void i3PostProcessNightVision::ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput)
{
	pCtx->SetShader(m_pShader);

	m_pCtx.setTexture(m_maskTextureList[m_nMaskingType]);

	m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texMask", &m_pCtx);
	m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_fNVMixRate", m_fMixRate);

	m_pCtx.setTexture(pInput->GetColorTexture());
	m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInput", &m_pCtx);

	pManager->DrawFullScreenQuad(pOutput, pCtx, m_pVA);
}

void i3PostProcessNightVision::AddMaskingTexture(const char * filename)
{
	i3Texture * pTexture = i3Texture::new_object();

	if(pTexture->Create(filename))
	{
		m_maskTextureList.push_back(pTexture);
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL, "Adding Mask Texture failed. %s", filename);
	}
}