#if !defined( __I3_ABSTRACT_BOOL_ATTR_H)
#define __I3_ABSTRACT_BOOL_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3AbstractBoolAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3AbstractBoolAttr);

protected:
	BOOL	m_bState;

public:
	i3AbstractBoolAttr(void);
	virtual ~i3AbstractBoolAttr(void);

	void		Set( BOOL state)				{	m_bState = state; }
	BOOL		Get(void)						{ return m_bState; }

	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void Dump(void);
	#endif
};

#endif
