#if !defined( __I3_FACE_CULL_MODE_ATTR_H)
#define __I3_FACE_CULL_MODE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FaceCullModeAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3FaceCullModeAttr, i3RenderAttr);

protected:
	I3G_FACE_CULL		m_Mode = I3G_FACE_CULL_BACK;

public:
	i3FaceCullModeAttr(void);

	void				Set( I3G_FACE_CULL mode)
	{
		m_Mode = mode;
	}

	I3G_FACE_CULL		Get(void)
	{
		return m_Mode;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo(i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump(void) override;
	#endif
};

#endif
