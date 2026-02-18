#if !defined( __I3G_SHADER_PARAM_SAMPLER_H)
#define __I3G_SHADER_PARAM_SAMPLER_H

#include "i3ShaderParam.h"

class I3_EXPORT_GFX i3ShaderParamSampler : public i3ShaderParam
{
	I3_CLASS_DEFINE( i3ShaderParamSampler);
protected:
	i3Texture *		m_pTex;

public:
	i3ShaderParamSampler(void);
	virtual ~i3ShaderParamSampler(void);

	i3Texture *		getTexture(void)		{ return m_pTex; }
	void			SetTexture( i3Texture * pObj);

	virtual void	Update( i3RenderContext * pCtx);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
