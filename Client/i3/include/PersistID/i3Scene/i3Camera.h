#if !defined( __I3_SG_CAMERA_H)
#define __I3_SG_CAMERA_H

#include "i3Node.h"
#include "i3Transform.h"
#include "i3ProjectMatrixAttr.h"
#include "i3ViewMatrixAttr.h"
#include "i3RenderTargetAttr.h"
#include "i3ClearRenderTargetAttr.h"
#include "i3RenderAttrListAttr.h"

#define		I3_CAM_STYLE_SUB			0x00000001

class I3_EXPORT_SCENE i3Camera : public i3Transform
{
	I3SG_NODE_CLASS_DEFINE( i3Camera);

public:
	enum I3_CAMERA_MODE
	{
		I3_CAMERA_MODE_PERSPECTIVE = 1,
		I3_CAMERA_MODE_ORTHOGONAL,
	};

protected:
	//Original Camera System Variable
	//persistant
	RT_REAL32					m_FOV;
	RT_REAL32					m_Near;
	RT_REAL32					m_Far;
	RT_REAL32					m_Width;
	RT_REAL32					m_Height;
	I3_CAMERA_MODE				m_Mode;
	BOOL						m_bEnable;
	UINT32						m_Style;
	RT_REAL32					m_UpdateInterval;

	//volatile
	i3ProjectMatrixAttr *		m_pProjectMatrixAttr;
	i3ViewMatrixAttr *			m_pViewMatrixAttr;
	i3RenderTargetAttr *		m_pRTAttr[2];
	i3ClearRenderTargetAttr	*	m_pClearAttr;
	i3RenderAttrListAttr *		m_pOutput;

	//volatile
	INT32						m_nidxcurRT;

protected:
	void UpdateProjectionMatrix(void);

public:

	i3Camera(void);
	virtual ~i3Camera(void);

	void			SetMode( I3_CAMERA_MODE mode);
	I3_CAMERA_MODE	GetMode(void)							{ return m_Mode; }

	UINT32			getStyle(void)							{ return m_Style; }
	void			setStyle( UINT32 style)					{ m_Style = style; }
	void			addStyle( UINT32 style)					{ m_Style |= style; }
	void			removeStyle( UINT32 style)				{ m_Style &= ~style; }

	bool			CreateRenderTarget( RT_REAL32 width, RT_REAL32 height, I3G_IMAGE_FORMAT format = I3G_IMAGE_FORMAT_BGRX_8888, I3G_IMAGE_FORMAT depthFormat = I3G_IMAGE_FORMAT_D16);

	void			SetPerspective( RT_REAL32 fov, RT_REAL32 width, RT_REAL32 height, RT_REAL32 zNear, RT_REAL32 zFar);
	void			SetOrthogonal( RT_REAL32 width, RT_REAL32 height, RT_REAL32 zNear, RT_REAL32 zFar);
	RT_MATRIX *		GetProjectionMatrix(void)					{ return m_pProjectMatrixAttr->GetMatrix(); }

	i3ProjectMatrixAttr *	GetProjectMatrixAttr(void)			{ return m_pProjectMatrixAttr; }
	i3ViewMatrixAttr *		GetViewMatrixAttr(void)				{ return m_pViewMatrixAttr; }

	void			SetFOV( RT_REAL32 val);
	RT_REAL32		GetFOV(void)								{ return m_FOV; }

	void			setUpdateInterval( RT_REAL32 rInterval)		{ m_UpdateInterval = rInterval;}
	RT_REAL32		getUpdateInterval(void)						{ return m_UpdateInterval;}

	void			SetNearFar( RT_REAL32	Near, RT_REAL32 Far);
	void			SetNear( RT_REAL32	Near)						{ SetNearFar( Near, m_Far); }	 
	void			SetFar( RT_REAL32	Far)						{ SetNearFar( m_Near, Far); }	 
	RT_REAL32		GetNear(void)								{ return m_Near; }
	RT_REAL32		GetFar(void)								{ return m_Far; }

	void			SetEnable( BOOL bFlag);
	BOOL			GetEnable(void)								{ return m_bEnable; }

	void			SetWindowSize( RT_REAL32 width, RT_REAL32 height);
	RT_REAL32		GetWindowWidth(void)						{ return m_Width; }
	RT_REAL32		GetWindowHeight(void)						{ return m_Height; }

	void			SwapRT(void)								{ m_nidxcurRT = m_nidxcurRT^1;}
	i3RenderTargetAttr * getCurRTAttr(void)						{ return m_pRTAttr[m_nidxcurRT];}

	i3RenderTargetAttr		*	getRenderTargetAttr(INT32 idx)		{ return m_pRTAttr[idx];}
	i3ClearRenderTargetAttr	*	getClearRenderTargetAttr( void)		{ return m_pClearAttr;}
	i3RenderAttrListAttr *		getOutputList(void)					{ return m_pOutput; }

	virtual void	CopyTo( i3ElementBase * pNode, I3_COPY_METHOD method);
	virtual	UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};

#endif
