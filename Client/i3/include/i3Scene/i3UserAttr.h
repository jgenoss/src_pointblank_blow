#ifndef __I3_USER_ATTR_H__
#define __I3_USER_ATTR_H__

#include "i3RenderAttr.h"

typedef void (*I3USERATTRPROC)( i3RenderContext * pContext, void *pParam );

class I3_EXPORT_SCENE i3UserAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3UserAttr, i3RenderAttr);

protected:
	I3USERATTRPROC		m_pFunc = nullptr;
	void				*m_pParam = nullptr;

public:
	i3UserAttr(void);

	void SetCallBack( I3USERATTRPROC pFunc, void *pParam );

	virtual void Apply( i3RenderContext * pContext) override;

	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif //__I3_USER_ATTR_H__
