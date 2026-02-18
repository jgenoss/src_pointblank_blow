#ifndef __I3UI_SOCKETBOX_H__
#define __I3UI_SOCKETBOX_H__

#include "i3UICaptionControl.h"

class I3_EXPORT_FRAMEWORK i3UISocketBox : public i3UICaptionControl
{
	I3_EXPORT_CLASS_DEFINE( i3UISocketBox , i3UICaptionControl);

protected:
	virtual void	_UpdateShapeState(void) override;

public:
	virtual void	OnEnabled( bool bEnable) override;
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse(void) override;

	virtual void	OnKeyDown( UINT32 nKey) override;
	virtual void	OnKeyUp( UINT32 nKey) override;

	virtual void	OnSize( REAL32 cx, REAL32 cy) override;
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif //__I3UI_BUTTON_H__
