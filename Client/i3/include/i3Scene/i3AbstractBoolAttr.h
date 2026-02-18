#if !defined( __I3_ABSTRACT_BOOL_ATTR_H)
#define __I3_ABSTRACT_BOOL_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3AbstractBoolAttr : public i3RenderAttr
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE(i3AbstractBoolAttr, i3RenderAttr);
protected:
	bool	m_bState = false;

public:
	void		Set( bool state)				{	m_bState = state; }
	bool		Get(void)						{ return m_bState; }

	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void Dump(void) override;
	#endif
};

#endif
