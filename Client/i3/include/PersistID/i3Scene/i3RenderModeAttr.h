#if !defined( __I3_RENDER_MODE_ATTR_H)
#define __I3_RENDER_MODE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3RenderModeAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3RenderModeAttr);

protected:
	I3G_RENDER_MODE		m_Mode;

public:
	i3RenderModeAttr(void);
	virtual ~i3RenderModeAttr(void);

	void			Set( I3G_RENDER_MODE mode)
	{
		m_Mode = mode;
	}

	I3G_RENDER_MODE	Get( void)
	{
		return m_Mode;
	}

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void	Dump(void);
	#endif
};

#endif
