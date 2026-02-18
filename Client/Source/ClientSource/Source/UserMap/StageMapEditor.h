#if !defined( __STAGE_MAP_EDITOR_H)
#define __STAGE_MAP_EDITOR_H

#include "UserMap.h"
#include "MapEditorCamera.h"
#include "Panel_Dashboard.h"
#include "Panel_Toolbar.h"
#include "Panel_Palette.h"
#include "ViewportControl/ViewportControl.h"

enum	VIEWCTRL_MODE
{
	VIEWCTRL_MOVE = 0,
	VIEWCTRL_ROTATE,
	VIEWCTRL_SCALE,

	VIEWCTRL_PLACEOBJECT,
	VIEWCTRL_SELECT,

	VIEWCTRL_COUNT,
};

class CPopupColorSet;
class CPopupSelectTexture;

class CStageMapEditor : public i3Stage
{
	I3_CLASS_DEFINE( CStageMapEditor);
protected:
	CUserMap			*	m_pMap;
	CMapEditorCamera	*	m_pCamera;

	// GUI
	i3GuiRoot *		m_pGuiRoot;

	//////////////////////////////////////
	// Dashboard
	CPanel_Dashboard *	m_pDashboard;

	//////////////////////////////////////
	// Toolbar
	CPanel_Toolbar *	m_pToolbar;
	
	//////////////////////////////////////
	// Palette
	CPanel_Palette *	m_pPalette;

	///////////////////////////////////////////////////////////////
	//Window Input Message

	//Control
	VIEWPORT_CONTROL_CONTEXT	m_CtrlCtx;

	VIEWCTRL_MODE				m_CtrlMode;
	CViewportControl	*		m_pViewportCtrl[ VIEWCTRL_COUNT];
	CViewportControl	*		m_pCtrl;

	i3List						m_listSelect;

protected:

	void			_InitGUI(void);
	void			_InitViewportCtrl(void);

public:
	CStageMapEditor(void);
	virtual ~CStageMapEditor(void);

	void			PopupPalette( REAL32 x, REAL32 y);

	CUserMap *			getMap(void)							{ return m_pMap; }
	CMapEditorCamera*	getEditorCamera(void)					{ return m_pCamera;}

	virtual void	OnCreate( void);

	virtual BOOL	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
	virtual void	OnInput( void);

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point);
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point);

	virtual void	OnMouseMove( UINT32 nFlag, POINT point);
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point);

	virtual	void	OnKeyDown( UINT32 nKey);
	virtual void	OnKeyUp( UINT32 nKey);

	virtual void	OnChangeVideo( i3VideoInfo * pInfo);

	void			SetViewportMode( VIEWCTRL_MODE mode);

	void			StartPlaceObject( USMAP::BLOCK::ID id);
	void			CancelPlaceObject(void);

	// 색깔 지정 및 텍스쳐 선택 다이얼로그
private:
	CPopupColorSet*					m_pPopupColorSet;
	CPopupSelectTexture*			m_pPopupSelectTexture;

public:
	CUserMap_Object *				PickObject( POINT point);
	void							OnSelectObject( CUserMap_Object * pObj);

	void							ScreenToWorld( POINT pos, VEC3D * pStart, VEC3D * pEnd);

	VIEWPORT_CONTROL_CONTEXT	*	getViewportCtrlCtx(void)		{	return &m_CtrlCtx;}
	void							GetViewProjectionMatrix(MATRIX * pViewProj);
};

#endif
