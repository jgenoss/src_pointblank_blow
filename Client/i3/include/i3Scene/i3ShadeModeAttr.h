#if !defined( __I3_SHADE_MODE_ATTR_H)
#define __I3_SHADE_MODE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ShadeModeAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3ShadeModeAttr, i3RenderAttr);
protected:
	I3G_SHADE_MODE	m_Mode = I3G_SHADE_PHONG;

public:
	i3ShadeModeAttr(void);

	void			Set( I3G_SHADE_MODE mode)
	{
		m_Mode = mode;
	}

	I3G_SHADE_MODE	Get(void)
	{
		return m_Mode;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;
	virtual UINT32 OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32 OnLoad( i3ResourceFile * pResFile) override;
};

#endif
