#if !defined( __I3_SG_CLEAR_RENDERTARGET_ATTR_H)
#define __I3_SG_CLEAR_RENDERTARGET_ATTR_H

#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3ClearRenderTargetAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3ClearRenderTargetAttr);

protected:
	COLOR				m_ClearColor;
	REAL32				m_ClearZ;
	UINT32				m_ClearStencil;

	I3G_CLEAR			m_AddMask;
	I3G_CLEAR			m_RemoveMask;

#if defined (I3G_DX)
	INT32				m_nClearRectCount;
	D3DRECT				m_ClearRectArray[8];
#endif

public:
	i3ClearRenderTargetAttr(void);
	virtual ~i3ClearRenderTargetAttr(void);

	COLOR *				GetClearColor(void)					{ return &m_ClearColor; }
	void				SetClearColor( COLOR * pCol)		{ i3Color::Set( &m_ClearColor, pCol); }

	REAL32				GetClearZ(void)						{ return m_ClearZ; }
	void				SetClearZ( REAL32 z)				{ m_ClearZ = z; }

	UINT32				GetClearStencil(void)				{ return m_ClearStencil; }
	void				SetClearStencil( UINT32 val)		{ m_ClearStencil = val; }

	I3G_CLEAR			GetAddClearMask(void)				{ return m_AddMask; }
	void				SetAddClearMask( I3G_CLEAR mask)	{ m_AddMask = mask; }

	I3G_CLEAR			GetRemoveClearMask(void)			{ return m_RemoveMask; }
	void				SetRemoveClearMask( I3G_CLEAR mask)	{ m_RemoveMask = mask; }

#if defined (I3G_DX)
	INT32				getClearRectCount( void)			{ return m_nClearRectCount;	}
	D3DRECT*			getClearRectArray( void)			{ return m_ClearRectArray;	}
	void				SetClearRect( D3DRECT* pRect);
	void				AddClearRect( D3DRECT* pRect);
#endif

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);
};

#endif
