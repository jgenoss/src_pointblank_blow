#ifndef __I3UI_LISTVIEW_SEP_H__
#define __I3UI_LISTVIEW_SEP_H__

#include "i3UIControl.h"

class I3_EXPORT_FRAMEWORK i3UIListView_Sep : public i3UIControl
{
	I3_EXPORT_CLASS_DEFINE( i3UIListView_Sep, i3UIControl);

	friend class i3UIListView_Header;
	friend class i3UIListView_Item;
	friend class i3UIListView_Box;

protected:

	virtual void		_UpdateShapeState(void) override;
	void			_Prepare( void)		{ prepareSprites(); }

public:
	i3UIListView_Sep();

	bool			isPtInRect( POINT point);

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual void	OnMouseMove( UINT32 nFlag, POINT point) override;
	virtual void	OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys) override;
	virtual void	OnLeaveMouse( void) override;

	virtual i3UIControl*	OnSelect( void) override;
};

#endif //__I3UI_LISTVIEW_SEP_H__
