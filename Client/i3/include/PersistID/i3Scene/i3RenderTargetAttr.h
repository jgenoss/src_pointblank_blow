#if !defined( __I3_SCENE_RENDER_TARGET_ATTR_H)
#define __I3_SCENE_RENDER_TARGET_ATTR_H

#include "i3RenderAttr.h"

enum I3SG_RENDERTARGET_MODE
{
	I3SG_RENDERTARGET_MODE_CREATE = 0,
	I3SG_RENDERTARGET_MODE_SET,
};

class I3_EXPORT_SCENE i3RenderTargetAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3RenderTargetAttr);
protected:
	i3Texture *				m_pRT;
	i3Texture *				m_pDepthRT;

	INT32					m_Width;
	INT32					m_Height;
	I3G_IMAGE_FORMAT		m_Format;
	I3G_IMAGE_FORMAT		m_DepthFormat;

	I3SG_RENDERTARGET_MODE	m_Mode;

public :
	i3RenderTargetAttr(void);
	virtual ~i3RenderTargetAttr(void);

	INT32				GetWidth(void)					{ return m_Width; }
	void				SetWidth(INT32 w)				{ m_Width = w; }

	INT32				GetHeight(void)					{ return m_Height; }
	void				SetHeight( INT32 h)				{ m_Height = h; }

	I3G_IMAGE_FORMAT	GetFormat(void)					{ return m_Format; }
	void				SetFormat( I3G_IMAGE_FORMAT f)	{ m_Format = f; }

	I3G_IMAGE_FORMAT	GetDepthFormat(void)				{ return m_DepthFormat; }
	void				SetDepthFormat( I3G_IMAGE_FORMAT f)	{ m_DepthFormat = f; }

	I3SG_RENDERTARGET_MODE	GetMode(void)				{ return m_Mode; }
	void				SetMode( I3SG_RENDERTARGET_MODE mode)	{ m_Mode = mode; }

	i3Texture *			GetColorRenderTarget(void)				{ return m_pRT; }
	void				SetColorRenderTarget( i3Texture * pTex);

	i3Texture *			GetDepthRenderTarget(void)				{ return m_pDepthRT; }
	void				SetDepthRenderTarget( i3Texture * pTex);

	void				CreateRenderTarget(void);
	void				CreateRenderTarget( INT32 Width, INT32 Height, I3G_IMAGE_FORMAT format, I3G_IMAGE_FORMAT depthFormat);
	void				BindRenderTarget( i3Texture * pColorRT, i3Texture * pDepthRT);

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

#if defined( I3_DEBUG)
	virtual void	Dump(void);
#endif
};

#endif
