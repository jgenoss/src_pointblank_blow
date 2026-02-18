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

	I3_VIEWER_CAM_ROTATE,
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
#define I3_VIEWER_KEY_NUMPAD_PLUS				0x00000040
#define I3_VIEWER_KEY_NUMPAD_MINUS				0x00000080
#define I3_VIEWER_KEY_NUMPAD_MINUS				0x00000080
#define I3_VIEWER_KEY_SHIFT						0x00000100
#define I3_VIEWER_KEY_CONTROL					0x00000200


#if defined( I3_WINDOWS)
#define	WM_FRM_SELECTTARGET						(WM_USER + 9987)
#endif

class I3_EXPORT_FRAMEWORK i3ViewerFramework : public i3Framework
{
	I3_EXPORT_CLASS_DEFINE( i3ViewerFramework, i3Framework);
protected:
	i3Camera *				m_pCurCamera = nullptr;
	i3Transform *			m_pGrid = nullptr;
	i3Timer *				m_pTimer = nullptr;
	i3Transform *			m_pLightTx = nullptr;
	i3LightAttr *			m_pLightAttr = nullptr;
	i3AttrSet *				m_pAttrSet = nullptr;
	i3AttrSet *				m_pPlayerAttrSet = nullptr;
	i3LightingEnableAttr *	m_pLightingEnableAttr = nullptr;
	i3LightingEnableAttr *	m_pPlayerLightingEnableAttr = nullptr;
	
	i3Light *				m_pPointLight = nullptr;
	bool					m_bActivePointLight = false;

	REAL32					m_LightRho = 0.0f;
	REAL32					m_LightTheta = 0.0f;
	REAL32					m_LightDist = 0.0f;

	bool					m_bShowGrid = false;

	VEC3D				m_BoundMin;
	VEC3D				m_BoundMax;

	bool					m_bDebugControl = false;

	I3_VIEWER_CAM_MOVE_MODE	m_CamMoveMode = I3_VIEWER_CAM_MOVE_3DSMAX;
	I3_VIEWER_CAMMODE		m_CamMode = I3_VIEWER_CAM_NONE;
	MATRIX				m_DragStartMatrix;
	VEC3D				m_BasePos;
	REAL32				m_CamDist = 28000.0f;
	REAL32				m_CamNear = 10.0f;
	REAL32				m_CamFar = 10000.0f;
	REAL32				m_ZoomRate = 10.0f;
	UINT32					m_CamModeFlag = 0;
	UINT32					m_CamMoveKey = 0;
	UINT32					m_KeyBoardKey = 0;
	VEC3D					m_FPSCamRot;
	VEC3D					m_OldFPSCamRot;

	MATRIX					m_SavedCamMatrix[I3_VIEWER_CAM_MOVE_MAX];

	i3Node *				m_pLoadedRoot = nullptr;
	i3Node *				m_pLoadedScene = nullptr;

	i3Node *				m_pLoadedPlayerRoot = nullptr;

	REAL32				m_AnimDuration = 0.0f;
	REAL32				m_timePlay = 0.0f;
	bool					m_bPlaying = true;

	i3TextNode *			m_pDebugTextNode = nullptr;
	i3Font *				m_pDebugFont = nullptr;
	i3::wstring				m_wstrDebugText;
	REAL32					m_timeDebugText = 1.0f;
	i3Timer *				m_pDebugTimer = nullptr;

	i3::rc_string			m_FileName;

#if defined( I3_WINDOWS)
	POINT				m_DragStartPt = { 0, 0 };

	bool				m_bPanelVisible = true;
#endif

protected:
	REAL32			GetCameraNear(void)				{ return m_CamNear; }
	REAL32			GetCameraFar(void)				{ return m_CamFar; }
	REAL32			GetCameraDistance(void)			{ return m_CamDist; }
	REAL32			GetZoomRate(void)				{ return m_ZoomRate; }
	VEC3D *			GetBasePosition(void)			{ return & m_BasePos; }

	void				SetCameraMoveMode( I3_VIEWER_CAM_MOVE_MODE mode);

	void				_SetGridSize( i3VertexArray * pVA, i3GeometryAttr * pGeoAttr, REAL32 sx, REAL32 sy);

public:
	i3ViewerFramework(void);
	virtual ~i3ViewerFramework(void);

	bool		GetGridShow(void)					{ return m_bShowGrid; }
	void		SetGridShow( bool bShow);

	bool		getLightingEnable(void)				{ return m_pLightingEnableAttr->Get(); }
	void		setLightingEnable( bool bFlag)		{ m_pLightingEnableAttr->Set( bFlag); }

	i3Camera *	GetCurrentCamera(void)					{ return m_pCurCamera; }
	void		SetCurrentCamera( i3Camera * pCam);

	void		SetCameraProperty( REAL32 dist, REAL32 near, REAL32 far);

	void		SetBound( VEC3D * pMin, VEC3D * pMax);
	VEC3D *	GetBoundMin(void)						{ return & m_BoundMin; }
	VEC3D *	GetBoundMax(void)						{ return & m_BoundMax; }

	virtual void	ScreenToWorld( INT32 scrX, INT32 scrY, VEC3D * pVec1, REAL32 zDistance);
	virtual void	ScreenToWorld( INT32 x1, INT32 y1, INT32 x2, INT32 y2, VEC3D * p1, REAL32 zDist);

	void		ToggleDebugControl(void)		{ m_bDebugControl = !m_bDebugControl; }
	void		EnableDebugControl(void)		{ m_bDebugControl = true; }
	void		DisableDebugControl(void)		{ m_bDebugControl = false; }

	i3Font *	getDebugFont(void)				{ return m_pDebugFont; }

	UINT32		GetCameraFlag(void)				{ return m_CamModeFlag; }
	void		SetCameraFlag( UINT32 mode)		{ m_CamModeFlag = mode; }
	void		AddCameraFlag( UINT32 mode)		{ m_CamModeFlag |= mode; }
	void		RemoveCameraFlag( UINT32 mode)	{ m_CamModeFlag &= ~mode; }

	void		PivotRotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/);
	void		SetAutoPivot(void);
	void		SetPivot( VEC3D * pVec);

	void		Pan( REAL32 dx, REAL32 dy, REAL32 dz);

	virtual bool	LoadI3S( const char * pszPath);
	virtual bool	CloseI3S(void);
	virtual void	SetSceneGraph( i3Node * pNode, bool bReset = true);
	i3Node *	GetLoadedScene(void)			{ return m_pLoadedScene; }

	virtual void UpdateLightTx(void);

	virtual void MoveCamera( VEC3D Pos);
	virtual void RotCamera( VEC3D Rot); 
	virtual void Zoom( REAL32 rate);

	virtual void	OnCreate(void) override;
	virtual void	OnChangeScene(void) override;
	virtual void	OnChangeBound(void) override;
	virtual void	OnUpdateCameraTransform(void) override;
	virtual void	OnUpdateCameraProjection(void) override;
	virtual void	OnResetCameraPosition(void) override;
	virtual void	OnInput(void) override;
	virtual void	OnRender(void) override;
	virtual void	OnSize( i3::pack::POINT2D * pSize) override;
	virtual void	OnUpdate( REAL32 timeDelta) override;
	virtual i3Node *	OnQueryAttachNode( i3GameRes * pRes) override;
	virtual i3Node *	OnPlayerQueryAttachNode( i3GameRes * pRes) override;

	virtual bool	OnStartPlay(void);
	virtual bool	OnStartPause(void);

	void			UpdateAnimationDuration(void);
	REAL32		GetDuration(void)						{ return m_AnimDuration; }
	REAL32		GetCurrentPlayTime(void)				{ return m_timePlay; }
	void			SetAnimationTime( REAL32 absTime);

	bool			isShadowEnabled(void);
	void			setShadowEnable( bool bFlag);

	void			setShadowMapSize( INT32 cx);

	i3Light	*		getDefLight(void)						{ return m_pLightAttr->GetLightObject(); }

	void			Play(void);
	void			Pause(void);
	bool			IsPlaying(void)							{ return m_bPlaying; }
	void			ShowDebugText(void);	
	void			SetDebugTextEnable( bool bFlag);		
	bool			GetDebugTextEnable(void);

#if defined( I3_WINDOWS)
	void			SetPanelVisible( bool bShow);

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point) override;

	virtual void	OnMouseMove( UINT32 nFlag, POINT point) override;
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point) override;

	virtual	void	OnKeyDown( UINT32 nKey) override;
	virtual void	OnKeyUp( UINT32 nKey) override;

	void			SaveScreenShot(void);

	//void UpdatePanelDuration(void);


	// 자신과 관련된 Target들을 LevelTool 등의 Editor에서 선택하도록 요구할 때
	// 호출된다.
	// 선택이 종료되면 hwndReport 윈도우에게 WM_FRM_SELECTTARGET 메시지가
	// 전달된다.
	virtual void			SelectTarget( HWND hwndReport);
	virtual void			UnselectAllTarget(void);
#endif
};

#endif
