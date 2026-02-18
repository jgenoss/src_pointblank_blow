#if !defined( __I3_VIEWER_FRAMEWORK_H)
#define __I3_VIEWER_FRAMEWORK_H

#include "i3Framework.h"

enum	I3_VIEWER_CAM_MOVE_MODE
{
	I3_VIEWER_CAM_MOVE_3DSMAX = 0,
	I3_VIEWER_CAM_MOVE_FPS,
	I3_VIEWER_CAM_MOVE_MAX
};

enum	I3_VIEWER_CAMMODE
{
	I3_VIEWER_CAM_NONE			= 0,
	I3_VIEWER_CAM_PIVOT_ROTATE,
	I3_VIEWER_CAM_PAN,
	I3_VIEWER_CAM_ZOOM,
};

#define	I3_VIEWER_CAM_DISABLE_PIVOT_ROTATE		0x00000001
#define I3_VIEWER_CAM_DISABLE_PAN				0x00000002
#define I3_VIEWER_CAM_DISABLE_ROTATE			0x00000004

#define	I3_VIEWER_KEY_LEFT						0x00000001
#define	I3_VIEWER_KEY_RIGHT						0x00000002
#define I3_VIEWER_KEY_UP						0x00000004
#define I3_VIEWER_KEY_DOWN						0x00000008
#define I3_VIEWER_KEY_FRONT						0x00000010
#define I3_VIEWER_KEY_BACK						0x00000020

#if defined( I3_WINDOWS)
#define	WM_FRM_SELECTTARGET						(WM_USER + 9987)
#endif

class I3_EXPORT_FRAMEWORK i3ViewerFramework : public i3Framework
{
	I3_CLASS_DEFINE( i3ViewerFramework);
protected:
	i3Camera *				m_pCurCamera;
	i3Transform *			m_pGrid;
	i3Timer *				m_pTimer;
	i3Transform *			m_pLightTx;
	i3LightAttr *			m_pLightAttr;
	i3AttrSet *				m_pAttrSet;
	i3LightingEnableAttr *	m_pLightingEnableAttr;
	i3Node *				m_pLightIconGeo;
	
	i3Light *				m_pPointLight;
	bool					m_bActivePointLight;

	REAL32					m_LightRho, m_LightTheta, m_LightDist;

#if defined( I3G_DX)
	i3Glow				*	m_pShaderGlow;				// Glow효과 쉐이더
#endif

	bool					m_bShowGrid;

	RT_VEC3D				m_BoundMin;
	RT_VEC3D				m_BoundMax;

	BOOL					m_bDebugControl;

	I3_VIEWER_CAM_MOVE_MODE	m_CamMoveMode;
	I3_VIEWER_CAMMODE		m_CamMode;
	RT_MATRIX				m_DragStartMatrix;
	RT_VEC3D				m_BasePos;
	RT_REAL32				m_CamDist;
	RT_REAL32				m_CamNear, m_CamFar;
	RT_REAL32				m_ZoomRate;
	UINT32					m_CamModeFlag;
	UINT32					m_CamMoveKey;
	VEC3D					m_FPSCamRot;
	VEC3D					m_OldFPSCamRot;

	RT_MATRIX				m_SavedCamMatrix[I3_VIEWER_CAM_MOVE_MAX];

	i3Node *				m_pLoadedRoot;
	i3Node *				m_pLoadedScene;

	RT_REAL32				m_AnimDuration;
	RT_REAL32				m_timePlay;
	bool					m_bPlaying;

	i3TextNode *			m_pDebugTextNode;
	i3Font *				m_pDebugFont;
	char					m_szDebugText[2048];

	// Rasterize Chain
	i3RasterizeChain_SimpleMotionBlur	*		m_pRasterizeBlur;
	i3RasterizeChain_DownScale	*				m_pRasterizeDownScale;
	i3RasterizeChain_BrightPathExtract	*		m_pRasterizeBrigthPath;
	i3RasterizeChain_GaussianBlur	*			m_pRasterizeGaussianBlur;
	i3RasterizeChain_BloomCombine	*			m_pRasterizeBloomCombine;

#if defined( I3_WINDOWS)
	POINT				m_DragStartPt;
	POINT				m_CurDragPt;

	HWND				m_hwndPanel;
	bool				m_bPanelVisible;
	INT32				m_PanelWidth;
	INT32				m_PanelHeight;
#endif

protected:
	void					SearchCameras(void);
	RT_REAL32			GetCameraNear(void)				{ return m_CamNear; }
	RT_REAL32			GetCameraFar(void)				{ return m_CamFar; }
	RT_REAL32			GetCameraDistance(void)			{ return m_CamDist; }
	RT_REAL32			GetZoomRate(void)				{ return m_ZoomRate; }
	RT_VEC3D *			GetBasePosition(void)			{ return & m_BasePos; }

	void				SetCameraMoveMode( I3_VIEWER_CAM_MOVE_MODE mode);

	void				_UpdateLightTx(void);

public:
	i3ViewerFramework(void);
	virtual ~i3ViewerFramework(void);

	bool		GetGridShow(void)					{ return m_bShowGrid; }
	void		SetGridShow( bool bShow);

	bool		getLightingEnable(void)				{ return m_pLightingEnableAttr->Get() != FALSE; }
	void		setLightingEnable( bool bFlag)		{ m_pLightingEnableAttr->Set( bFlag); }

#if defined( I3G_DX)
	void		setGlowEffectEnable( bool bFlag)	{ m_pShaderGlow->SetGlowEnable( bFlag); }
	bool		getGlowEffectEnable(void)			{ return m_pShaderGlow->GetGlowEnable() == TRUE; }

	REAL32		getGlowEffectBlurDispersion(void)	{ return m_pShaderGlow->GetBlurDispersion(); }
	void		setGlowEffectBlurDispersion( REAL32 val)
	{
		m_pShaderGlow->SetBlurDispersion( val);
	}
#endif


	i3Camera *	GetCurrentCamera(void)					{ return m_pCurCamera; }
	void		SetCurrentCamera( i3Camera * pCam);

	void		SetCameraProperty( REAL32 dist, REAL32 near, REAL32 far);

	void		SetBound( RT_VEC3D * pMin, RT_VEC3D * pMax);
	RT_VEC3D *	GetBoundMin(void)						{ return & m_BoundMin; }
	RT_VEC3D *	GetBoundMax(void)						{ return & m_BoundMax; }

	void		ScreenToWorld( INT32 scrX, INT32 scrY, RT_VEC3D * pVec1, RT_REAL32 zDistance);
	void		ScreenToWorld( INT32 x1, INT32 y1, INT32 x2, INT32 y2, RT_VEC3D * p1, RT_REAL32 zDist);

	void		ToggleDebugControl(void)		{ m_bDebugControl = !m_bDebugControl; }
	void		EnableDebugControl(void)		{ m_bDebugControl = TRUE; }
	void		DisableDebugControl(void)		{ m_bDebugControl = FALSE; }

	UINT32		GetCameraFlag(void)				{ return m_CamModeFlag; }
	void		SetCameraFlag( UINT32 mode)		{ m_CamModeFlag = mode; }
	void		AddCameraFlag( UINT32 mode)		{ m_CamModeFlag |= mode; }
	void		RemoveCameraFlag( UINT32 mode)	{ m_CamModeFlag &= ~mode; }

	void		PivotRotate( RT_REAL32 RotX, RT_REAL32 RotY, RT_REAL32 /*RotZ*/);
	void		SetAutoPivot(void);
	void		SetPivot( VEC3D * pVec);

	void		Pan( RT_REAL32 dx, RT_REAL32 dy, RT_REAL32 dz);

	bool		LoadI3S( const char * pszPath);
	void		SetSceneGraph( i3Node * pNode);
	i3Node *	GetLoadedScene(void)			{ return m_pLoadedScene; }

	virtual void MoveCamera( RT_VEC3D Pos);
	virtual void RotCamera( RT_VEC3D Rot); 
	virtual void Zoom( RT_REAL32 rate);

	virtual void	OnCreate(void);
	virtual void	OnChangeScene(void);
	virtual void	OnChangeBound(void);
	virtual void	OnUpdateCameraTransform(void);
	virtual void	OnUpdateCameraProjection(void);
	virtual void	OnResetCameraPosition(void);
	virtual void	OnInput(void);
	virtual void	OnRender(void);
	virtual void	OnSize( POINT2D * pSize);
	virtual void	OnUpdate( RT_REAL32 timeDelta);
	virtual i3Node *	OnQueryAttachNode( i3GameRes * pRes);

	virtual bool	OnStartPlay(void);
	virtual bool	OnStartPause(void);

	void			UpdateAnimationDuration(void);
	RT_REAL32		GetDuration(void)						{ return m_AnimDuration; }
	RT_REAL32		GetCurrentPlayTime(void)				{ return m_timePlay; }
	void			SetAnimationTime( REAL32 absTime);

	void			Play(void);
	void			Pause(void);
	bool			IsPlaying(void)							{ return m_bPlaying; }
	void			ShowDebugText(void);	
	void			SetDebugTextEnable( bool bFlag);		
	bool			GetDebugTextEnable(void);

	void			ToggleRasterizeChain_SimpleMotionBlur(void);
	void			ToggleRasterizeChain_DownScale(void);
	void			ToggleRasterizeChain_BrightPassExtract(void);
	void			ToggleRasterizeChain_GaussianBlur(void);
	void			ToggleRasterizeChain_BloomCombine(void);
	BOOL			isEnable_SimpleMotionBlur(void)					{ return m_pRasterizeBlur->getEnable();				}
	BOOL			isEnable_DownScale(void)						{ return m_pRasterizeDownScale->getEnable();		}		
	BOOL			isEnable_BrightPassExtract(void)				{ return m_pRasterizeBrigthPath->getEnable();		}
	BOOL			isEnable_GaussianBlur(void)						{ return m_pRasterizeGaussianBlur->getEnable();		}
	BOOL			isEnable_BloomCombine(void)						{ return m_pRasterizeBloomCombine->getEnable();		}

#if defined( I3_WINDOWS)
	void			SetPanelVisible( bool bShow);
	void			SetPanelHWND( HWND hwnd)					{ m_hwndPanel = hwnd; }

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point);

	virtual void	OnMouseMove( UINT32 nFlag, POINT point);
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point);

	virtual	void	OnKeyDown( UINT32 nKey);
	virtual void	OnKeyUp( UINT32 nKey);

	void			SaveScreenShot(void);

	void UpdatePanelDuration(void);

	#if defined( I3_DEBUG)
		// 자신과 관련된 Target들을 LevelTool 등의 Editor에서 선택하도록 요구할 때
		// 호출된다.
		// 선택이 종료되면 hwndReport 윈도우에게 WM_FRM_SELECTTARGET 메시지가
		// 전달된다.
		virtual void			SelectTarget( HWND hwndReport);
		virtual void			UnselectAllTarget(void);
	#endif
#endif
};

#endif
