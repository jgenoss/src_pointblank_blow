#if !defined( __I3G_SHADER_PARAM_SAMPLER_H)
#define __I3G_SHADER_PARAM_SAMPLER_H

#include "i3ShaderParam.h"

class I3_EXPORT_GFX i3ShaderParamSampler : public i3ShaderParam
{
	I3_EXPORT_CLASS_DEFINE( i3ShaderParamSampler, i3ShaderParam);
protected:
	i3Texture *		m_pTex = nullptr;

public:
	i3ShaderParamSampler(void);
	virtual ~i3ShaderParamSampler(void);

	i3Texture *		getTexture(void)		{ return m_pTex; }
	void			SetTexture( i3Texture * pObj);

	virtual void	Update( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
