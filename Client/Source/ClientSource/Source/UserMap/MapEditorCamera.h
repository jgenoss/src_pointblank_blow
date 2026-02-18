#if !defined( __MAPEDITOR_CAMERA_H__)
#define __MAPEDITOR_CAMERA_H__

#include "ViewportControl/ViewportControl.h"

class CMapEditorCamera : public i3EventReceiver
{
	I3_CLASS_DEFINE( CMapEditorCamera);

protected:
	i3Camera * m_pCamera;

	VEC3D		m_vMouseDelta;
	UINT32		m_nMouseButtonState;
	UINT32		m_nKeyPressState;

	bool		m_LBDragging;
	bool		m_RBDragging;
	bool		m_bMoving;

	POINT		m_posDragStart;
	POINT		m_posOld;

public:
	CMapEditorCamera(void);
	virtual ~CMapEditorCamera(void);


protected:
	void	_SetMoveMode( bool bEnable);
	void	_SetCursorPosition( POINT point);

public:
	void	InitCamera( i3Camera * pCam);
	i3Camera * getCamera(void)				{	return m_pCamera;}

	void	OnChangeVideo( i3VideoInfo * pInfo);

	void	OnMoveXY( REAL32 X, REAL32 Y);
	void	OnMoveAt( REAL32 rDelta);
	void	OnMoveZRotateX( REAL32 X, REAL32 Z);
	void	OnRotateXY( REAL32 X, REAL32 Z);

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

	virtual void	OnInput(void);
			void	OnMouseMove( i3InputMouse * pMouse, REAL32 rDeltaSeconds);
			void	OnKeyInput( i3InputKeyboard * pKey, REAL32 rDeltaSeconds);
};

#endif //#if !defined( __MAPEDITOR_CAMERA_H__)