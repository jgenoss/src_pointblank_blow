#include "i3GfxType.h"
#include "i3RenderContext.h"
#include "i3ShaderParamSamplerStates.h"
 
//I3_CLASS_INSTANCE( i3ShaderParamSamplerStates, i3ShaderParam);
I3_CLASS_INSTANCE( i3ShaderParamSamplerStates);

i3ShaderParamSamplerStates::i3ShaderParamSamplerStates(void)
{
	setSemantic( i3ShaderParam::SEMANTIC_CONSTANT);

	m_TextureBindCtx.m_bEnable = true;
	m_TextureBindCtx.m_ModifyFlag = 0xFFFFFFFF;
	m_TextureBindCtx.m_MinFilter = I3G_TEXTURE_FILTER_POINT;
	m_TextureBindCtx.m_MagFilter = I3G_TEXTURE_FILTER_POINT;
}

i3ShaderParamSamplerStates::~i3ShaderParamSamplerStates(void)
{
	I3_SAFE_RELEASE( m_TextureBindCtx.m_pTexture);
}

void i3ShaderParamSamplerStates::SetTexture(i3Texture * pTex)
{
	I3_REF_CHANGE( m_TextureBindCtx.m_pTexture, pTex);
	
	AddModifyFlag(I3G_TEXTURE_MODIFY_TEXTURE);
}

void i3ShaderParamSamplerStates::SetTextureWrap(I3G_TEXTURE_WRAP HWrap, I3G_TEXTURE_WRAP VWrap)
{
	m_TextureBindCtx.m_HWrap = HWrap;
	m_TextureBindCtx.m_VWrap = VWrap;

	AddModifyFlag(I3G_TEXTURE_MODIFY_WRAP);
}

void i3ShaderParamSamplerStates::SetTextureFilter(I3G_TEXTURE_FILTER min, I3G_TEXTURE_FILTER mag)
{
	m_TextureBindCtx.m_MinFilter = min;
	m_TextureBindCtx.m_MagFilter = mag;

	AddModifyFlag(I3G_TEXTURE_MODIFY_FILTER);
}

void i3ShaderParamSamplerStates::Update( i3RenderContext * pCtx)
{
	i3ShaderParam::Update( &m_TextureBindCtx);

	pCtx->NotifyTextureBindChange();

	AddModifyFlag(I3G_TEXTURE_MODIFY_TEXTURE);
	AddModifyFlag(I3G_TEXTURE_MODIFY_WRAP);
	AddModifyFlag(I3G_TEXTURE_MODIFY_FILTER);
}

UINT32 i3ShaderParamSamplerStates::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3ShaderParamSamplerStates::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
