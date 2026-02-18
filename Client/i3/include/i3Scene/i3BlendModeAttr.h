#if !defined( __I3_BLEND_MODE_ATTR_H)
#define __I3_BLEND_MODE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3BlendModeAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3BlendModeAttr, i3RenderAttr);

protected:
	I3G_BLEND		m_SrcBlend = I3G_BLEND_ONE;
	I3G_BLEND		m_DestBlend = I3G_BLEND_ZERO;
	I3G_BLEND_OP	m_BlendOp = I3G_BLEND_OP_ADD;

public:
	i3BlendModeAttr(void);
	void			SetSource( I3G_BLEND blend)
	{
		m_SrcBlend = blend;
	}

	I3G_BLEND		GetSource(void)
	{
		return m_SrcBlend;
	}

	void			SetDestination( I3G_BLEND blend)
	{
		m_DestBlend = blend;
	}

	I3G_BLEND		GetDestination(void)
	{
		return m_DestBlend;
	}

	void			SetBlendOperation( I3G_BLEND_OP op)
	{
		m_BlendOp = op;
	}

	I3G_BLEND_OP	GetBlendOperation(void)
	{
		return m_BlendOp;
	}


	virtual void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool	IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump(void) override;
	#endif
};

#endif
