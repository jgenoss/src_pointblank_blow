#if !defined( __I3_VIEWPORT_ATTR_H)
#define __I3_VIEWPORT_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ViewportAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3ViewportAttr, i3RenderAttr);

protected:
	INT32	m_nStartX = 0;
	INT32	m_nStartY = 0;
	INT32	m_nWidth = -1;
	INT32	m_nHeight = -1;

public:
	i3ViewportAttr(void);
	
	virtual void Apply( i3RenderContext * pContext) override;
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool IsSame( i3RenderAttr * pAttr) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	void		SetViewportRect( INT32 nStartX, INT32 nStartY, INT32 nWidth, INT32 nHeight);
	void		setStartPosXY( INT32 nX, INT32 nY)	{	m_nStartX = nX, m_nStartY = nY;			}
	void		setStartPosX( INT32 nX)				{	m_nStartX = nX;							}
	void		setStartPosY( INT32 nY)				{	m_nStartY = nY;							}
	void		setWidth( INT32 nWidth)				{	m_nWidth = nWidth;						}
	void		setHeight( INT32 nHeight)			{	m_nHeight = nHeight;					}

	INT32		getStartPosX( void)					{	return m_nStartX;						}
	INT32		getStartPosY( void)					{	return m_nStartY;						}
	INT32		getWidth( void)						{	return m_nWidth;						}
	INT32		getHeight( void)					{	return m_nHeight;						}
};


#endif // __I3_VIEWPORT_ATTR_H
