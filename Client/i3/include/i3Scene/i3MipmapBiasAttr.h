#if !defined( __I3_MIMAP_BIAS_ATTR_H)
#define __I3_MIMAP_BIAS_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3MipmapBiasAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE(i3MipmapBiasAttr, i3RenderAttr);

protected:
	REAL32		m_fBias = 0.0f;

public:
	i3MipmapBiasAttr(void);

	void			Set(REAL32 fbias)
	{
		 m_fBias = fbias;
	}

	REAL32		Get(void)
	{
		return m_fBias;
	}

	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

};

#endif
