#ifndef __I3UI_IMAGE_BUTTON_H__
#define __I3UI_IMAGE_BUTTON_H__

#include "i3UIControl.h"

class I3_EXPORT_FRAMEWORK i3UIImageButton : public i3UIControl
{
	I3_CLASS_DEFINE( i3UIImageButton );
protected:
	void			_UpdateShape(void);

public:
	i3UIImageButton();
	virtual ~i3UIImageButton(); 

	virtual void	OnEnabled( bool bEnable);
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys);
	virtual void	OnLeaveMouse(void);

	virtual void	OnKeyDown( UINT32 nKey);
	virtual void	OnKeyUp( UINT32 nKey);

	virtual void	OnSize( REAL32 cx, REAL32 cy);
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};

#endif //__I3UI_BUTTON_H__
