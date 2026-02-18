#include "i3GfxType.h"
#include "i3ShaderParamMatrixObject.h"
 
//I3_CLASS_INSTANCE( i3ShaderParamMatrixObject, i3ShaderParam);
I3_CLASS_INSTANCE( i3ShaderParamMatrixObject);

i3ShaderParamMatrixObject::i3ShaderParamMatrixObject(void)
{
	setSemantic( i3ShaderParam::SEMANTIC_CONSTANT);
}

i3ShaderParamMatrixObject::~i3ShaderParamMatrixObject(void)
{
	I3_SAFE_RELEASE( m_pMatrixObj);
}

void i3ShaderParamMatrixObject::SetMatrixObject( i3MatrixObject * pObj)
{
	I3_REF_CHANGE( m_pMatrixObj, pObj);
}

void i3ShaderParamMatrixObject::Update( i3RenderContext * pCtx)
{
	i3ShaderParam::Update( m_pMatrixObj->GetMatrix());
}

UINT32 i3ShaderParamMatrixObject::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3ShaderParamMatrixObject::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
