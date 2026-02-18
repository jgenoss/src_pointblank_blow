#if !defined( __I3_LEVEL_VIEWPORT_H)
#define __I3_LEVEL_VIEWPORT_H

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

#include "i3LevelFramework.h"

#include "../i3TDKViewerCtrl.h"

class i3LevelScene;


class I3_EXPORT_TDK i3LevelViewport : public i3TDKViewerCtrl
{
	DECLARE_DYNCREATE( i3LevelViewport);
protected:
	i3LevelFramework *		m_pFramework = nullptr;

public:
	i3LevelViewport(void);
	virtual ~i3LevelViewport(void);

	BOOL			Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID, i3LevelScene * pScene);
	afx_msg			void OnDestroy();
	void			Cleanup();		
		// 이런 함수 추가..(2012.09.13.수빈...i3Renderer디바이스가 소거되기전에 렌더링관련객체(m_pFramework)가 미리 소거되야함)

	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	void			OnSave( i3::pack::Level_VIEWPORT * pInfo);
	void			OnLoad( i3::pack::Level_VIEWPORT * pInfo);

	VEC3D			GetScreenPos( const VEC3D & wsPos);

	i3LevelFramework *			getFramework(void)						{ return m_pFramework; }
	static i3LevelViewport *		GetCurrentFocusViewport(void);

protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif