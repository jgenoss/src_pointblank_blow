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
#define		I3_CAM_STYLE_PROJECTION		0x00000002		///> FOV만 변경된 Camera
#define		I3_CAM_STYLE_FULLFRAME		0x00000008

class I3_EXPORT_SCENE i3Camera : public i3Transform
{
	I3SG_NODE_CLASS_DEFINE( i3Camera, i3Transform);

public:
	enum I3_CAMERA_MODE
	{
		I3_CAMERA_MODE_PERSPECTIVE = 1,
		I3_CAMERA_MODE_ORTHOGONAL,
	};

protected:
	//Original Camera System Variable
	//persistant
	REAL32					m_FOV = I3_DEG2RAD(45.0f);
	REAL32					m_Near = 10.0f;
	REAL32					m_Far = 10000.0f;
	REAL32					m_Width = 640.0f;
	REAL32					m_Height = 480.0f;
	I3_CAMERA_MODE				m_Mode = I3_CAMERA_MODE_PERSPECTIVE;
	bool						m_bEnable = true;
	UINT32						m_Style = 0;
	REAL32					m_UpdateInterval = 0.0f;

	//volatile
	i3ProjectMatrixAttr *		m_pProjectMatrixAttr;	// [initialize at constructor]
	i3ViewMatrixAttr *			m_pViewMatrixAttr;		// [initialize at constructor]
	i3RenderTargetAttr *		m_pRTAttr = nullptr;
	i3ClearRenderTargetAttr	*	m_pClearAttr = nullptr;
	i3RenderAttrListAttr *		m_pOutput = nullptr;

	i3Node *					m_pSubCamSg = nullptr;

	//volatile
	INT32						m_nidxcurRT = 0;

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

	bool			CreateRenderTarget( REAL32 width, REAL32 height, I3G_IMAGE_FORMAT format = I3G_IMAGE_FORMAT_BGRX_8888, I3G_IMAGE_FORMAT depthFormat = I3G_IMAGE_FORMAT_D16);

	void			SetClearColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a);

	void			SetPerspective( REAL32 fov, REAL32 width, REAL32 height, REAL32 zNear, REAL32 zFar);
	void			SetOrthogonal( REAL32 width, REAL32 height, REAL32 zNear, REAL32 zFar);
	MATRIX *		GetProjectionMatrix(void)					{ return m_pProjectMatrixAttr->GetMatrix(); }
	MATRIX *		GetViewMatrix(void)					{ return m_pViewMatrixAttr->GetMatrix(); }

	i3ProjectMatrixAttr *	GetProjectMatrixAttr(void)			{ return m_pProjectMatrixAttr; }
	i3ViewMatrixAttr *		GetViewMatrixAttr(void)				{ return m_pViewMatrixAttr; }

	void			SetFOV( REAL32 val);
	REAL32		GetFOV(void)								{ return m_FOV; }

	void			setUpdateInterval( REAL32 rInterval)		{ m_UpdateInterval = rInterval;}
	REAL32		getUpdateInterval(void)						{ return m_UpdateInterval;}

	void			SetNearFar( REAL32	Near, REAL32 Far);
	void			SetNear( REAL32	Near)						{ SetNearFar( Near, m_Far); }	 
	void			SetFar( REAL32	Far)						{ SetNearFar( m_Near, Far); }	 
	REAL32		GetNear(void)								{ return m_Near; }
	REAL32		GetFar(void)								{ return m_Far; }

	void			SetEnable( bool bFlag)						{ m_bEnable = bFlag; }
	bool			GetEnable(void)								{ return m_bEnable; }

	// Sub-Camera인 경우, 자신만의 Sg.를 가지고 Rendering하는 경우가 있다.
	// 그것을 위한...
	i3Node *		getSubCamSg(void)							{ return m_pSubCamSg; }
	void			setSubCamSg( i3Node * pNode);

	void			SetWindowSize( REAL32 width, REAL32 height);
	REAL32		GetWindowWidth(void)						{ return m_Width; }
	REAL32		GetWindowHeight(void)						{ return m_Height; }

	i3RenderTargetAttr		*	getRenderTargetAttr(void)		{ return m_pRTAttr;}
	i3ClearRenderTargetAttr	*	getClearRenderTargetAttr( void)		{ return m_pClearAttr;}
	i3RenderAttrListAttr *		getOutputList(void)					{ return m_pOutput; }
	i3Texture *		getColorRenderTarget(void)					{ return m_pRTAttr->GetColorRenderTarget(); }
	i3Texture *		getDepthRenderTarget(void)					{ return m_pRTAttr->GetDepthRenderTarget(); }

	virtual void	CopyTo( i3ElementBase * pNode, I3_COPY_METHOD method) override;
	virtual	UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
