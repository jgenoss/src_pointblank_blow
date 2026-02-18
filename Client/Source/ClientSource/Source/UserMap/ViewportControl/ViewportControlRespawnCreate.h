#if !defined( __VIEWPORT_CONTROL_RESPAWN_CREATE_H)
#define __VIEWPORT_CONTROL_RESPAWN_CREATE_H

#include "ViewportControl.h"
//#include "i3LevelRespawn.h"

class CViewportControlRespawnCreate : public CViewportControl
{
	I3_CLASS_DEFINE( CViewportControlRespawnCreate);
protected:
	//i3LevelRespawn *		m_pRespawnList;
	BOOL					m_bCreated;

protected:
	void	GetPlacePos( POINT point, VEC3D * pDest);

public:
	CViewportControlRespawnCreate(void);
	virtual ~CViewportControlRespawnCreate(void);

	virtual void OnLButtonDown( UINT nFlags, POINT point);
	virtual void OnLButtonUp( UINT nFlags, POINT point);
	virtual void OnRButtonDown( UINT nFlags, POINT point);
	virtual void OnMouseMove( UINT nFlags, POINT point);
};

#endif