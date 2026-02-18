#include "i3GfxType.h"
#include "i3ShaderParamVector4.h"

//I3_CLASS_INSTANCE( i3ShaderParamVector4, i3ShaderParam);
I3_CLASS_INSTANCE( i3ShaderParamVector4);

i3ShaderParamVector4::i3ShaderParamVector4(void)
{
	setSemantic( i3ShaderParam::SEMANTIC_CONSTANT);
}

i3ShaderParamVector4::~i3ShaderParamVector4(void)
{
	I3MEM_SAFE_FREE_POINTER( m_pVector ); 
}

bool i3ShaderParamVector4::Create(INT32 nCount)
{
	m_pVector = (VEC4D *) i3MemAlloc( sizeof(VEC4D) * nCount);

	if(m_pVector == nullptr)
	{
		return false;
	}

	return true;
}

void i3ShaderParamVector4::Update( i3RenderContext * pCtx)
{
	i3ShaderParam::Update( m_pVector);
}

UINT32 i3ShaderParamVector4::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3ShaderParamVector4::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
