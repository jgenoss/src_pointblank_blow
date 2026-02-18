#include "i3GfxType.h"
#include "i3ShaderParamSampler.h"
 
//I3_CLASS_INSTANCE( i3ShaderParamSampler, i3ShaderParam);
I3_CLASS_INSTANCE( i3ShaderParamSampler);

i3ShaderParamSampler::i3ShaderParamSampler(void)
{
	setSemantic( i3ShaderParam::SEMANTIC_CONSTANT);
}

i3ShaderParamSampler::~i3ShaderParamSampler(void)
{
	I3_SAFE_RELEASE( m_pTex);
}

void i3ShaderParamSampler::SetTexture( i3Texture * pObj)
{
	I3_REF_CHANGE( m_pTex, pObj);
}

void i3ShaderParamSampler::Update( i3RenderContext * pCtx)
{
	i3ShaderParam::Update( m_pTex);
}

UINT32 i3ShaderParamSampler::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3ShaderParamSampler::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
