#if !defined( __I3G_SHADER_PARAM_SAMPLER_STATES_H)
#define __I3G_SHADER_PARAM_SAMPLER_STATES_H

#include "i3ShaderParam.h"

class I3_EXPORT_GFX i3ShaderParamSamplerStates : public i3ShaderParam
{
	I3_EXPORT_CLASS_DEFINE( i3ShaderParamSamplerStates, i3ShaderParam);
protected:
	I3G_TEXTURE_BIND_CONTEXT 		m_TextureBindCtx;

public:
	i3ShaderParamSamplerStates(void);
	virtual ~i3ShaderParamSamplerStates(void);

	void			setModifyFlag(UINT32 flag)		{ m_TextureBindCtx.m_ModifyFlag = flag;					}
	void			AddModifyFlag(UINT32 flag)		{ m_TextureBindCtx.m_ModifyFlag |= flag;				}
	void			RemoveModifyFlag(UINT32 flag)	{ m_TextureBindCtx.m_ModifyFlag &= ~flag;				}
	bool			IsFlag( UINT32 flag)			{ return (m_TextureBindCtx.m_ModifyFlag & flag) != 0;	}

	void			SetTexture(i3Texture * pTex);
	void			SetTextureWrap(I3G_TEXTURE_WRAP HWrap, I3G_TEXTURE_WRAP VWrap);
	void			SetTextureFilter(I3G_TEXTURE_FILTER min, I3G_TEXTURE_FILTER mag);

	virtual void	Update( i3RenderContext * pCtx) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
