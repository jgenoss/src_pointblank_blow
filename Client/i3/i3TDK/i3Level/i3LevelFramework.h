#if !defined( __I3_LEVEL_FRAMEWORK_H)
#define __I3_LEVEL_FRAMEWORK_H

#include "i3Scene.h"
#include "i3LevelElement.h"
#include "i3LevelCamera.h"
#include "i3LevelCameraFree.h"
#include "i3LevelCameraWalk.h"
#include "i3LevelCameraTop.h"
#include "i3LevelCameraFront.h"
#include "i3LevelCameraScene.h"
#include "i3LevelAxis.h"
#include "i3LevelAxisMove.h"
#include "i3LevelAxisRotate.h"
#include "i3LevelAxisScale.h"

#include "i3LevelViewportControl.h"
#include "i3LevelViewportControlObjectSelect.h"
#include "i3LevelViewportControlObjectMove.h"
#include "i3LevelViewportControlObjectRotate.h"
#include "i3LevelViewportControlObjectScale.h"
#include "i3LevelViewportControlObjectBrush.h"
#include "i3LevelViewportControlPathCreate.h"
#include "i3LevelViewportControlRespawnCreate.h"
#include "i3LevelViewportControlSelectTarget.h"

#include "../i3TDKUpdateSystem.h"

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace i3
{
	namespace pack
	{
		struct Level_VIEWPORT
		{
			UINT8			m_magic[4] = { 'T', 'V', 'I', '1' };
			UINT32			m_ID = 0;
			UINT8			m_CtrlMode = 0;
			UINT8			m_CurrentCamera = 0;
			BOOL			m_bPlay = FALSE;
			INT32			m_FPS = 0;
			REAL32			m_TimeScale = 0.0f;
			i3::pack::VEC3D	m_AxisMask;

			i3::pack::VEC3D	m_FreeCamPos;
			i3::pack::VEC3D	m_FreeCamRotate;
			i3::pack::VEC3D	m_FreeCamScale;

			i3::pack::VEC3D	m_TopCamPos;
			i3::pack::VEC3D	m_TopCamRotate;
			i3::pack::VEC3D	m_TopCamScale;

			i3::pack::VEC3D	m_WalkCamPos;
			i3::pack::VEC3D	m_WalkCamRotate;
			i3::pack::VEC3D	m_WalkCamScale;

			UINT8			m_Reserved[128] = { 0 };
		};
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

class i3LevelViewport;


enum LEVEL_EDIT_MODE
{
	LEM_NORMAL = 0,
	LEM_NAVMESH_POINT,
	LEM_NAVMESH_POLYGON,

	LEM_NAVMESH_POINT_ADD,
	LEM_NAVMESH_POLYGON_CUT,
	
	MAX_LEVEL_EDIT_MODE,
};

enum LEVEL_CONTROL_MODE
{
	LCM_SELECT = 0,
	LCM_MOVE,
	LCM_ROTATE,
	LCM_SCALE,
	LCM_OBJBRUSH,
	LCM_PATHEDIT,

	MAX_LEVEL_CONTROL_MODE,
};


typedef void (*fnLevelEditModeChanged)( LEVEL_EDIT_MODE _mode );

class I3_EXPORT_TDK i3LevelFramework : public i3ViewerFramework
{
	I3_EXPORT_CLASS_DEFINE( i3LevelFramework, i3ViewerFramework);
public:
	enum CAMERA_MODE
	{
		CAMERA_MODE_FREE,
		CAMERA_MODE_TOP,
		CAMERA_MODE_WALK,
		CAMERA_MODE_SCENE,
		CAMERA_MODE_FRONT,
	};

	enum CONTROL_MODE_DETAIL
	{
		CTRL_MODE_NORMAL = 0,
		CTRL_MODE_SELECT,
		CTRL_MODE_OBJECT_MOVE,
		CTRL_MODE_OBJECT_ROTATE,
		CTRL_MODE_OBJECT_SCALE,
		CTRL_MODE_OBJECT_BRUSH,
		CTRL_MODE_PATH_CREATE,
		CTRL_MODE_RESPAWN_CREATE,
		CTRL_MODE_SELECT_TARGET,

		// navmesh
		CTRL_MODE_NAVMESH_POINT_SELECT,
		CTRL_MODE_NAVMESH_POINT_MOVE,
		CTRL_MODE_NAVMESH_POINT_ADD,

		CTRL_MODE_NAVMESH_POLYGON_SELECT,
		CTRL_MODE_NAVMESH_POLYGON_MOVE,

		CTRL_MODE_NAVMESH_POLYGON_CUT,

		CTRL_MODE_MAX
	};

public:
	LEVEL_EDIT_MODE			GetLevelEditMode() const	{ return m_LevelEditMode; }
	LEVEL_CONTROL_MODE		GetLevelCtrlMode() const	{ return m_LevelCtrlMode; }

	bool		SetLevelEditMode( LEVEL_EDIT_MODE _mode );
	bool		SetLevelCtrlMode( LEVEL_CONTROL_MODE _mode );	

protected:
	CONTROL_MODE_DETAIL		_GetCtrlMode() const			
	{
		return m_CtrlMode;  
	}

	bool		_SetCtrlMode( CONTROL_MODE_DETAIL _mode );

protected:
	LEVEL_EDIT_MODE					m_LevelEditMode;						// [initialize at constructor] 
	LEVEL_CONTROL_MODE				m_LevelCtrlMode;						// [initialize at constructor]

	LEVEL_EDIT_MODE					m_prevLevelEditMode;					// [initialize at constructor]

	CONTROL_MODE_DETAIL				m_OldCtrlMode = CTRL_MODE_NORMAL;
	CONTROL_MODE_DETAIL				m_CtrlMode;								// [initialize at constructor]

	i3LevelViewportControl *		m_pCtrl = nullptr;
	i3LevelViewportControl *		m_pCtrlTable[CTRL_MODE_MAX];			// [initialize at constructor]


protected:
	i3LevelScene *			m_pScene = nullptr;

	i3Node *				m_pRoot;					// [initialize at constructor]
	i3AttrSet *				m_pSceneRoot;				// [initialize at constructor]
	i3Node *				m_pAxisRoot;				// [initialize at constructor]
	i3TextNodeDX *			m_pTextNode = nullptr;
	i3TextNodeDX *			m_pTextNodeNavMesh = nullptr;

	i3LevelCameraFree *		m_pFreeCamera = nullptr;
	i3LevelCameraWalk *		m_pWalkCamera = nullptr;
	i3LevelCameraTop *		m_pTopCamera = nullptr;
	i3LevelCameraFront *	m_pFrontCamera = nullptr;

	i3LevelCamera *			m_pCurCamera = nullptr;
	i3LevelSceneCamera *	m_pSceneCamera = nullptr;
	i3LevelRes	*			m_pSelectedRes = nullptr;
	CAMERA_MODE				m_CameraMode = CAMERA_MODE_FREE;
	bool					m_bCollisionEdit = false;

	I3_VIEWER_CAMMODE		m_CamEditMode = I3_VIEWER_CAM_NONE;

	I3_LEVEL_EDIT_CONTEXT	m_Ctx;	

	i3LevelViewport *		m_pViewport = nullptr;

	REAL32					m_CameraZoomScale = 2.5f;

	BOOL					m_bPlay = FALSE;		// Play 중인지, Pause 상태인지를 나타내는 변수
	REAL32					m_TimeScale = 1.0f;
	BOOL					m_bActive = FALSE;		// WM_ACTIVATE에 의해, Active 상태인가를 나타내는 변수
	BOOL					m_bOnTimer = FALSE;			// WM_TIMER Handler 처리 중인가를 나타내는 변수

	i3LevelAxis *			m_pCurAxis = nullptr;
	BOOL					m_bPause = FALSE;

	I3LV_SPACE::TYPE		m_SpaceMode = I3LV_SPACE::WORLD;
	
	i3LevelHitMap			m_LevelHitMap;
	friend	class i3LevelViewportControl;

public:
	
	void	SetFnEditModeChanged( fnLevelEditModeChanged pf )
	{
		m_pFnEditModeChanged = pf;
	}

protected:
	fnLevelEditModeChanged		m_pFnEditModeChanged = nullptr;

public:
	i3LevelFramework(void);
	virtual ~i3LevelFramework(void);

	i3LevelAxis*	GetLevelAxis()
	{
		return m_pCurAxis;
	}

	void			setViewport( i3LevelViewport * pPort)	
	{ 
		m_pViewport = pPort; 
		m_Ctx.m_pViewport = pPort;
	}

	i3LevelViewport *	getViewport(void)					{ return m_pViewport; }

	void			CreateAxis(void);
	void			ResizeAxis(void);

	I3LV_SPACE::TYPE	getSpaceMode(void) const		{ return m_SpaceMode; }
	void				setSpaceMode( I3LV_SPACE::TYPE type);
	
	BOOL			CollisionGizmo( CPoint point );

	void			SaveMatrix(void);
	void			RestoreMatrix(void);

	void			PendingUpdate(void);
	void			RestoreUpdate(void);

	void			PopupMenu(void);

	void			SetMouseCursor( HCURSOR hCursor);

	i3LevelRes *	GetSelectedRes(void)					{ return m_pSelectedRes; }
	void			SetSelectedRes( i3LevelRes * pRes)		{ m_pSelectedRes = pRes; }

	void			SetScene( i3LevelScene * pScene);		
	i3LevelScene *	getScene(void)							{ return m_pScene;}

	void			SetClearColor( COLOR * pCol);
	void			GetClearColor( COLOR * pCol);

	CAMERA_MODE		GetCameraMode(void)						{ return m_CameraMode; }

	void			SetSceneCamera( i3LevelSceneCamera * pSceneCamera) { m_pSceneCamera = pSceneCamera; } 
	void			SetCameraMode( CAMERA_MODE mode);

	bool			getCollisionEditMode(void)				{ return m_bCollisionEdit; }
	void			SetCollisionEditMode( bool bFlag);
	void			ToggleCollisionEditMode(void)			{ SetCollisionEditMode( ! m_bCollisionEdit); }

	void			SetCollisionVisible( bool bFlag);

	BOOL			IsActive(void)							{ return m_bActive; }
	void			SetActive( BOOL bFlag);

	REAL32			GetTimeScale(void)						{ return m_TimeScale; }
	void			SetTimeScale( REAL32 tm)				{ m_TimeScale = tm; }

	i3LevelCamera *	GetCurrentCamera(void)					{ return m_pCurCamera; }
	void			SetCurrentCamera( i3LevelCamera * pCam);

	VEC3D *			GetBasePosition(void)					{ return & m_Ctx.m_BasePos; }
	VEC3D *			GetAxisMask(void)						{ return & m_Ctx.m_AxisMask; }
	void			SetAxisMask( VEC3D * pAxisMask)			{ i3Vector::Copy( &m_Ctx.m_AxisMask, pAxisMask); }

	void			PivotRotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/);
	void			Rotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/);
	void			Pan( REAL32 dx, REAL32 dy, REAL32 dz);
	void			Fit(void);
	void			Fit( i3LevelElement3D * pElm);

	bool			GetPickedPos( CPoint point, VEC3D * pPos);

	void			ScreenToWorld( INT32 scrX, INT32 scrY, VEC3D * pVec1, REAL32 zDistance);
	void			ScreenToWorld( INT32 x1, INT32 y1, INT32 x2, INT32 y2, VEC3D * pDiff, REAL32 zDist);
	void			GetViewProjectionMatrix( MATRIX * pMatrix);

	void			SetGroup(void);
	void			ReleaseGroup(void);

	void			ReassignGeneratedResName(void);

	void			Copy(void);
	void			Cut(void);
	void			Paste( bool bAutoPlace = true);

	void			OnSelect( i3LevelElement3D * pElm);
	void			OnNavSelect();
	void			OnUpdateEvent( I3_TDK_UPDATE_INFO * pInfo);

	virtual void	OnCreate(void) override;
	virtual void	OnUpdate( REAL32 tm) override;
	virtual void	OnUpdateCameraProjection(void) override;

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point) override;

	virtual void	OnMouseMove( UINT32 nFlag, POINT point) override;
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point) override;

	virtual	void	OnKeyDown( UINT32 nKey) override;
	virtual void	OnKeyUp( UINT32 nKey) override;

	virtual void	OnActivate( bool bActivate) override;
	virtual void	OnKillFocus() override;

	virtual void		SelectTarget( HWND hwndReport) override;
	virtual void		UnselectAllTarget(void) override;

	virtual void	MoveCamera( VEC3D Pos) override;
	
	void			EndSelectTarget(void);

	void			OnSaveInfo( i3::pack::Level_VIEWPORT * pInfo);
	void			OnLoadInfo( i3::pack::Level_VIEWPORT * pInfo);

	i3LevelHitMap* GetLevelHitMap() {return &m_LevelHitMap;}

protected:
	void		_OnUpdate_Camera( REAL32 tm );
};

#endif
