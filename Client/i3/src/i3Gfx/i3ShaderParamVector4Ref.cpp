#include "i3GfxType.h"
#include "i3ShaderParamVector4Ref.h"

I3_CLASS_INSTANCE( i3ShaderParamVector4Ref);

i3ShaderParamVector4Ref::i3ShaderParamVector4Ref(void)
{
	setSemantic( i3ShaderParam::SEMANTIC_CONSTANT);
}

i3ShaderParamVector4Ref::~i3ShaderParamVector4Ref(void)
{
}

void i3ShaderParamVector4Ref::Update( i3RenderContext * pCtx)
{
	i3ShaderParam::Update( m_pVector);
}

UINT32 i3ShaderParamVector4Ref::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3ShaderParamVector4Ref::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
