#ifndef __I3_USER_ATTR_H__
#define __I3_USER_ATTR_H__

#include "i3RenderAttr.h"

typedef void (*I3USERATTRPROC)( i3RenderContext * pContext, void *pParam );

class I3_EXPORT_SCENE i3UserAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3UserAttr);

protected:
	I3USERATTRPROC		m_pFunc;
	void				*m_pParam;

public:
	i3UserAttr(void);
	virtual ~i3UserAttr(void);

	void SetCallBack( I3USERATTRPROC pFunc, void *pParam );

	virtual void Apply( i3RenderContext * pContext);

	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif //__I3_USER_ATTR_H__
