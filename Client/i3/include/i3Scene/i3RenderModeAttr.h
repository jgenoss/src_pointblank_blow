#if !defined( __I3_RENDER_MODE_ATTR_H)
#define __I3_RENDER_MODE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3RenderModeAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3RenderModeAttr, i3RenderAttr);

protected:
	I3G_RENDER_MODE		m_Mode;

public:
	i3RenderModeAttr(void);

	void			Set( I3G_RENDER_MODE mode)
	{
		m_Mode = mode;
	}

	I3G_RENDER_MODE	Get( void)
	{
		return m_Mode;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump(void) override;
	#endif
};

#endif
