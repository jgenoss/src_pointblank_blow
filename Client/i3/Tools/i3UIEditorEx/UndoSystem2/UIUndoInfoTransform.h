#if !defined( __I3UICONTROL_UNDO_INFO_TRANSFORM_H__)
#define __I3UICONTROL_UNDO_INFO_TRANSFORM_H__

#include "UIUndoInfo.h"

//#if defined( I3_DISTRIBUTE)

class UIUndoInfoTransform : public UIUndoInfo
{
	I3_CLASS_DEFINE( UIUndoInfoTransform, UIUndoInfo);
protected:
	VEC2D			m_vSize;
	VEC3D			m_vPos;

public:
	UIUndoInfoTransform(void);
	virtual ~UIUndoInfoTransform(void);

	virtual void	setObject( i3UIControl * pObj) override;
	virtual bool	Undo( i3UIScene * pScene) override;
	virtual bool	Redo( i3UIScene * pScene) override;

	//void			SetTransform( MATRIX * pmtx);
	virtual void	PrintUndoInfo( void) override;
};

//#endif	// I3_DISTRIBUTE

#endif	// __I3UICONTROL_UNDO_INFO_TRANSFORM_H__
