#if !defined( __I3_BLEND_MODE_ATTR_H)
#define __I3_BLEND_MODE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3BlendModeAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3BlendModeAttr);

protected:
	I3G_BLEND		m_SrcBlend;
	I3G_BLEND		m_DestBlend;
	I3G_BLEND_OP	m_BlendOp;

public:
	i3BlendModeAttr(void);
	virtual ~i3BlendModeAttr(void);

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


	virtual void	Apply( i3RenderContext * pContext);
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void	Dump(void);
	#endif
};

#endif
