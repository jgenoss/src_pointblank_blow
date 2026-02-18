#if !defined( __I3G_SHADER_PARAM_VECTOR4REF_H)
#define __I3G_SHADER_PARAM_VECTOR4REF_H

#include "i3ShaderParam.h"

class I3_EXPORT_GFX i3ShaderParamVector4Ref : public i3ShaderParam
{
	I3_EXPORT_CLASS_DEFINE( i3ShaderParamVector4Ref, i3ShaderParam);
protected:
	VEC4D *			m_pVector = nullptr;

public:
	i3ShaderParamVector4Ref(void);
	virtual ~i3ShaderParamVector4Ref(void);

	VEC4D *			getVector(void)					{ return m_pVector;					}
	void			setVector( VEC4D * pVec)		{ m_pVector = pVec;					}

	virtual void	Update( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
