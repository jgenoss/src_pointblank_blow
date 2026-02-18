#if !defined( __I3_LEVEL_DESIGN_FRAMEWORK_H)
#define __I3_LEVEL_DESIGN_FRAMEWORK_H


class i3LevelDesignFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( i3LevelDesignFramework);
public:
	enum CAMERA_MODE
	{
		CAMERA_MODE_FREE,
		CAMERA_MODE_TOP,
		CAMERA_MODE_WALK,
		CAMERA_MODE_SCENE
	};

	enum EDIT_MODE
	{
		EDIT_MODE_NORMAL,
		EDIT_MODE_SELECT,
		EDIT_MODE_OBJECT_MOVE,
		EDIT_MODE_OBJECT_ROTATE,
		EDIT_MODE_OBJECT_SCALE,
		EDIT_MODE_OBJECT_BRUSH
	};
	
protected:
	CAMERA_MODE				m_CameraMode;
	EDIT_MODE				m_EditMode;
	EDIT_MODE				m_SavedEditMode;

	i3Node *				m_pRoot;
	i3AttrSet *				m_pSceneRoot;
	i3Node *				m_pAxisRoot;

	i3LevelCameraFree *		m_pFreeCamera;
	i3LevelCameraWalk *		m_pWalkCamera;
	i3LevelCameraTop *		m_pTopCamera;
	i3LevelCamera *			m_pCurCamera;
	i3LevelSceneCamera *	m_pSceneCamera; 
	i3LevelRes	*			m_pSelectedRes;

	i3LevelScene *			m_pScene;

	MATRIX					m_DragStartMatrix;
	POINT					m_DragStartPt;
	POINT					m_OldDragPt;
	VEC3D					m_BasePos;
	BOOL					m_bDragging;

	VEC3D					m_AxisMask;

	i3LevelAxisMove *			m_pMoveAxis;
	i3LevelAxis *				m_pCurAxis;

	HCURSOR					m_hCursorSel;
	HCURSOR					m_hCursorMove;
	HCURSOR					m_hCursorRotate;
	HCURSOR					m_hCursorScale;
	HCURSOR					m_hCursorBrush;

protected:
	void			CreateMoveAxis(void);
	void			SaveMatrix(void);
	void			RestoreMatrix(void);
	void			BeginMultiSelect( POINT pt);
	void			MultiSelecting( POINT pt);
	void			EndMultiSelect( POINT pt);
	void			DrawSelectRect( POINT pt);

	void			PopupMenu(void);

	void			SetMouseCursor( HCURSOR hCursor);

public:
	i3LevelDesignFramework(void);
	virtual ~i3LevelDesignFramework(void);

	void			setScene( i3LevelScene * pScene);

	void			SetSceneCamera( i3LevelSceneCamera * pSceneCamera) { m_pSceneCamera = pSceneCamera; } 
	void			SetCameraMode( CAMERA_MODE mode);

	EDIT_MODE		GetEditMode(void)						{ return m_EditMode; }
	void			SetEditMode( EDIT_MODE mode);

	i3LevelCamera *	GetCurrentCamera(void)					{ return m_pCurCamera; }
	void			SetCurrentCamera( i3LevelCamera * pCam);

	VEC3D *			GetBasePosition(void)					{ return & m_BasePos; }

	void			PlaceNewObject( BOOL bClone, CPoint point);

	void			OnSelect( i3LevelElement3D * pElm);
	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	void			OnSave( I3_PERSIST_Level_VIEWPORT * pInfo);
	void			OnLoad( I3_PERSIST_Level_VIEWPORT * pInfo);

	virtual void	OnCreate(void);

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
};

#endif
