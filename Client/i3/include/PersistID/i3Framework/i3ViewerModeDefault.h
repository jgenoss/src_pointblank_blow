#if !defined( __I3_VIEWER_MODE_DEFAULT_H)
#define __I3_VIEWER_MODE_DEFAULT_H

#include "i3ViewerMode.h"

enum	I3_VIEWER_CAMMODE
{
	I3_VIEWER_CAM_NONE			= 0,
	I3_VIEWER_CAM_PIVOT_ROTATE,
	I3_VIEWER_CAM_PAN,
	I3_VIEWER_CAM_ROTATE,
};

#define	I3_VIEWER_CAM_DISABLE_PIVOT_ROTATE		0x00000001
#define I3_VIEWER_CAM_DISABLE_PAN				0x00000002
#define I3_VIEWER_CAM_DISABLE_ROTATE			0x00000004

class I3_EXPORT_FRAMEWORK i3ViewerModeDefault : public i3ViewerMode
{
	I3_CLASS_DEFINE( i3ViewerModeDefault);
protected:
	I3_VIEWER_CAMMODE		m_CamMode;
	RT_MATRIX				m_DragStartMatrix;
	RT_VEC3D				m_BasePos;
	RT_REAL32				m_CamDist;
	RT_REAL32				m_CamNear, m_CamFar;
	RT_REAL32				m_ZoomRate;
	UINT32					m_CamModeFlag;

#if defined( I3_WINDOWS)
	POINT				m_DragStartPt;
#endif

protected:

public:
	i3ViewerModeDefault(void);
	virtual ~i3ViewerModeDefault(void);

	RT_REAL32			GetCameraNear(void)				{ return m_CamNear; }
	RT_REAL32			GetCameraFar(void)				{ return m_CamFar; }
	RT_REAL32			GetCameraDistance(void)			{ return m_CamDist; }
	RT_REAL32			GetZoomRate(void)				{ return m_ZoomRate; }
	RT_VEC3D *			GetBasePosition(void)			{ return & m_BasePos; }

	UINT32			GetCameraFlag(void)				{ return m_CamModeFlag; }
	void			SetCameraFlag( UINT32 mode)		{ m_CamModeFlag = mode; }
	void			AddCameraFlag( UINT32 mode)		{ m_CamModeFlag |= mode; }
	void			RemoveCameraFlag( UINT32 mode)	{ m_CamModeFlag &= ~mode; }

	void PivotRotate( RT_REAL32 RotX, RT_REAL32 RotY, RT_REAL32 /*RotZ*/);
	void	SetPivot( VEC3D * pVec);

	void Rotate( RT_REAL32 RotX, RT_REAL32 RotY, RT_REAL32 /*RotZ*/);
	void Pan( RT_REAL32 dx, RT_REAL32 dy, RT_REAL32 dz);

	virtual void	OnChangeBound(void);
	virtual void	OnResetCameraPosition(void);
	virtual void	OnUpdateCamera(void);
	virtual	void	OnInput(void);

#if defined( I3_WINDOWS)
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point);

	virtual void	OnMouseMove( UINT32 nFlag, POINT point);
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point);

	virtual	void	OnKeyDown( UINT32 nKey);
#endif
};

#endif
