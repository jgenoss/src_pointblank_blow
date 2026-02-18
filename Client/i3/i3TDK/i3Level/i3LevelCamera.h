#if !defined( __I3_Level_CAMERA_H)
#define __I3_Level_CAMERA_H

#include "i3Scene.h"
#include "i3LevelElement3D.h"
#include "i3LevelViewportControl.h"

class i3LevelFramework;
class i3LevelViewport;

class I3_EXPORT_TDK i3LevelCamera : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE( i3LevelCamera, i3LevelElement3D);

protected:
	i3Camera *		m_pCamera;		//기본 카메라 클래스 [initialize at constructor]
	UINT32			m_CamModeFlag = 0;
	
public:
	void			UpdateCamera( i3LevelFramework * pFramework);

public:
	i3LevelCamera(void);
	virtual ~i3LevelCamera(void);

	i3Camera *		GetCameraNode(void)							{ return m_pCamera; }
	void			SetCameraNode( i3Camera * pCam);

	void			SetViewport( i3LevelViewport * pView);

	void			SetPerspective( REAL32 fov, REAL32 width, REAL32 height, REAL32 zNear, REAL32 zFar)
	{
		m_pCamera->SetPerspective( fov, width, height, zNear, zFar);
	}

	void			SetOrthogonal( REAL32 width, REAL32 height, REAL32 zNear, REAL32 zFar)
	{
		m_pCamera->SetOrthogonal( width, height, zNear, zFar);
	}

	MATRIX *		GetViewMatrix(void)							{ return m_pCamera->GetViewMatrixAttr()->GetMatrix(); }
	MATRIX	*		GetProjectionMatrix(void)					{ return m_pCamera->GetProjectionMatrix(); }

	void			SetMode( i3Camera::I3_CAMERA_MODE mode)				{ m_pCamera->SetMode( mode); }
	i3Camera::I3_CAMERA_MODE	GetMode(void)							{ return m_pCamera->GetMode(); }

	void			SetFOV( REAL32 val)							{ m_pCamera->SetFOV( val); }
	REAL32			GetFOV(void)								{ return m_pCamera->GetFOV(); }

	void			SetNearFar( REAL32	Near, REAL32 Far)		{ m_pCamera->SetNearFar( Near, Far); }
	void			SetNear( REAL32	Near)						{ m_pCamera->SetNearFar( Near, m_pCamera->GetFar()); }	 
	void			SetFar( REAL32	Far)						{ SetNearFar( m_pCamera->GetNear(), Far); }	 
	REAL32			GetNear(void)								{ return m_pCamera->GetNear(); }
	REAL32			GetFar(void)								{ return m_pCamera->GetFar(); }

	void			SetEnable( bool bFlag)						{ m_pCamera->SetEnable( bFlag); }
	bool			GetEnable(void)								{ return m_pCamera->GetEnable(); }

	UINT32			GetCameraFlag(void)						{ return m_CamModeFlag; }
	void			SetCameraFlag( UINT32 mode)				{ m_CamModeFlag = mode; }
	void			AddCameraFlag( UINT32 mode)				{ m_CamModeFlag |= mode; }
	void			RemoveCameraFlag( UINT32 mode)			{ m_CamModeFlag &= ~mode; }
	bool			IsCameraFlag( UINT32 mode)				{ return (m_CamModeFlag & mode) == mode; }

	virtual void	OnMoving(void) override;
	virtual void	OnScaling(void) override;
	virtual void	OnRotating(void) override;

public:
	virtual void OnDrive( REAL32 time);

public:
	virtual void		SaveDragStartMatrix( I3_LEVEL_EDIT_CONTEXT& ctx );
	
	virtual void		CameraRotate( I3_LEVEL_EDIT_CONTEXT& ctx, REAL32 RotX, REAL32 RotY );
	virtual void		CameraPivotRotate( I3_LEVEL_EDIT_CONTEXT& ctx, REAL32 RotX, REAL32 RotY );
	virtual void		CameraPan( I3_LEVEL_EDIT_CONTEXT& ctx, REAL32 dx, REAL32 dy, REAL32 dz );		
	virtual void		CameraZoom( I3_LEVEL_EDIT_CONTEXT& ctx, float fzoom );
	virtual void		CameraMove( I3_LEVEL_EDIT_CONTEXT& ctx, REAL32 dx, REAL32 dy, REAL32 dz );
};

#endif