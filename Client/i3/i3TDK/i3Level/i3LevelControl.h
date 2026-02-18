#if !defined( __I3_Level_CONTROL_H)
#define __I3_Level_CONTROL_H

#include "i3LevelElement3D.h"

class I3_EXPORT_TDK i3LevelControl : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE( i3LevelControl, i3LevelElement3D);

protected:
	bool					m_bNotify = true;
	i3LevelElement3D *		m_pOwner = nullptr;
	i3ElementBase *			m_pProxy = nullptr;

public:
	i3LevelControl(void);

	void				setNotifyEnable( bool bFlag)		{ m_bNotify = bFlag; }
	bool				getNotifyEnable(void)				{ return m_bNotify;}

	i3LevelElement3D *	GetOwner(void)						{ return m_pOwner; }
	void				SetOwner( i3LevelElement3D * pElm);

	i3ElementBase *		getProxy(void)						{ return m_pProxy; }
	void				setProxy( i3ElementBase * pObj)		{ m_pProxy = pObj; }

	virtual void	OnDetachScene( i3LevelScene * pScene) override;
	virtual void	OnMoving(void) override;
	virtual void	OnScaling(void) override;
	virtual void	OnRotating(void) override;
	virtual void	OnSelected( BOOL bSelected) override;
};

#endif