#include "i3GfxType.h"
#include "i3ShaderCache.h"
#include "i3PostProcessManager.h"
#include "i3PostProcessHaze.h"
#include "i3GfxGlobalVariableDX.h"
I3_CLASS_INSTANCE(i3PostProcessHaze);

i3PostProcessHaze::i3PostProcessHaze()
{	
	m_pTexBindCtx.setEnable(FALSE);
	m_pTexBindCtx.setFilter(I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_LINEAR);
	m_pTexBindCtx.setWrap(I3G_TEXTURE_WRAP_MIRROR);
	m_pTexBindCtx.setTexture(NULL);
	m_pTexBindCtx.setBorderColor(0x00000000);
	
	SetHazePath("Gui/Hud/HazeMask.tga");
}

i3PostProcessHaze::~i3PostProcessHaze()
{
	I3_SAFE_RELEASE(m_pShader);
	I3_SAFE_RELEASE(m_pHazeTex);
	I3_SAFE_RELEASE(m_pVA);
}

void i3PostProcessHaze::Create(i3PostProcessManager * pManager)
{
	i3Shader* pNewShader = i3ShaderCache::LoadShader("i3Haze.hlsl");		// ADDREF°¡ ¾øÀ½..
	I3_REF_CHANGE(m_pShader, pNewShader);

	if (m_pHazeTex == NULL)
	{
		m_pHazeTex = i3Texture::new_object();
		if (!m_pHazeTex->Create(m_TextureName))
		{
			I3PRINTLOG(I3LOG_FATAL, "Adding Texture failed. %s", m_TextureName);
		}
	}

	VEC3D _pos[4];
	VEC2D _tex[4];
	i3VertexFormat format;

	if (m_pVA == NULL)
	{
		m_pVA = i3VertexArray::new_object();
		I3ASSERT(m_pVA != NULL);

		format.SetHasPosition(true);
		format.SetTextureCoordSetCount(1);
		format.SetTextureCoordCount(2);

		m_pVA->Create(&format, 4, 0);
	}

	{
		//	Position
		i3Vector::Set(&_pos[0], -1.0f, 1.0f, 0.0f);
		i3Vector::Set(&_pos[1], -1.0f, -1.0f, 0.0f);
		i3Vector::Set(&_pos[2], 1.0f, 1.0f, 0.0f);
		i3Vector::Set(&_pos[3], 1.0f, -1.0f, 0.0f);

		i3Vector::Set(&_tex[0], 0.0f, 0.0f);
		i3Vector::Set(&_tex[1], 0.0f, 1.0f);
		i3Vector::Set(&_tex[2], 1.0f, 0.0f);
		i3Vector::Set(&_tex[3], 1.0f, 1.0f);
	}

	m_pVA->Lock(0);
	m_pVA->SetPositions(0, 4, _pos);
	m_pVA->SetTextureCoords(0, 0, 4, _tex);
	m_pVA->Unlock();
}


void i3PostProcessHaze::InitEffect()
{
	m_UVPos.x = m_UVPos.y = 0;

	m_fWaveVolume = 0.0f;
	m_FadeState = FADEINOUT::enum_Fade_None;
	i3PostProcess::SetEnable(false);
}

void i3PostProcessHaze::StartEffect()
{
	i3PostProcess::SetEnable(true);

	m_FadeState = FADEINOUT::enum_Fade_In;
}

void i3PostProcessHaze::EndEffect()
{
	m_FadeState = FADEINOUT::enum_Fade_Out;
}

void i3PostProcessHaze::ApplyPostProcess(i3PostProcessManager * pManager, i3RenderContext * pCtx, i3RenderTarget * pInput, i3RenderTarget * pOutput)
{
	pCtx->SetShader(m_pShader);

	m_pTexBindCtx.setTexture(pInput->GetColorTexture());
	m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInputBase", &m_pTexBindCtx);

	m_pTexBindCtx.setTexture(m_pHazeTex);
	m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_texInputBlend", &m_pTexBindCtx);
	m_pShader->SetParam(I3G_SHADER_TYPE_PIXEL, "g_WaveVolume", m_fWaveVolume);

	//m_UVPos.x += pManager->GetDeltaTime() / m_fSpeed;
	m_UVPos.y += pManager->GetDeltaTime() / m_fSpeed;

	m_pShader->SetParam(I3G_SHADER_TYPE_VERTEX, "g_UV", &m_UVPos);

	pManager->DrawFullScreenQuad(pOutput, pCtx, m_pVA);

	UpdateFadeInOut(pManager->GetDeltaTime());
}

void i3PostProcessHaze::SetHazePath(const char* chPath)
{
	i3::generic_string_ncopy(m_TextureName, chPath, NAME_SIZE);
}

void i3PostProcessHaze::UpdateFadeInOut(REAL32 deltatime)
{
	if (m_FadeState == FADEINOUT::enum_Fade_In)
	{
		m_fWaveVolume += deltatime;
		if (m_fWaveVolume > 1.0f)
		{
			m_FadeState = FADEINOUT::enum_Fade_None;
			m_fWaveVolume = 1.0f;
		}
	}
	else if (m_FadeState == FADEINOUT::enum_Fade_Out)
	{
		m_fWaveVolume -= deltatime;
		if (m_fWaveVolume < 0.0f)
		{
			InitEffect();
		}
	}
}