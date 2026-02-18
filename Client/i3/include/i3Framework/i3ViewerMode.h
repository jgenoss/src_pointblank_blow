#if !defined( __I3_VIEWER_MODE_H)
#define __I3_VIEWER_MODE_H

#include "i3Base.h"

class i3Viewer;

class I3_EXPORT_FRAMEWORK i3ViewerMode : public i3ElementBase
{
	I3_ABSTRACT_CLASS_DEFINE( i3ViewerMode);
protected:
	i3Viewer *	m_pViewer;

	BOOL					m_bDebugControl;

public:
	i3ViewerMode(void);
	virtual ~i3ViewerMode(void);

	void		SetViewer( i3Viewer * pViewer)			{ m_pViewer = pViewer; }

	void			ToggleDebugControl(void)		{ m_bDebugControl = !m_bDebugControl; }
	void			EnableDebugControl(void)		{ m_bDebugControl = TRUE; }
	void			DisableDebugControl(void)		{ m_bDebugControl = FALSE; }

	virtual		void	OnCreate(void);
	virtual		void	OnAttach(void);
	virtual		void	OnDetach(void);
	virtual		void	OnChangeScene(void);
	virtual		void	OnChangeBound(void);
	virtual		void	OnChangeCamera(void);
	virtual		void	OnUpdateCamera(void);
	virtual		void	OnResetCameraPosition(void);
	virtual		void	OnInput(void);

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
	virtual void	OnKeyUp( UINT32 nKey);
#endif
};

#endif
