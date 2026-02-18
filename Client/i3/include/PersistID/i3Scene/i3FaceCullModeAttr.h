#if !defined( __I3_FACE_CULL_MODE_ATTR_H)
#define __I3_FACE_CULL_MODE_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3FaceCullModeAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3FaceCullModeAttr);

protected:
	I3G_FACE_CULL		m_Mode;

public:
	i3FaceCullModeAttr(void);
	virtual ~i3FaceCullModeAttr(void);

	void				Set( I3G_FACE_CULL mode)
	{
		m_Mode = mode;
	}

	I3G_FACE_CULL		Get(void)
	{
		return m_Mode;
	}

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo(i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void	Dump(void);
	#endif
};

#endif
