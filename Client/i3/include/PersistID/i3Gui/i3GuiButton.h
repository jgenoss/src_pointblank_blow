#ifndef __I3GUI_BUTTON_H__
#define __I3GUI_BUTTON_H__

#include "i3GuiControl.h"

/*
enum I3GUI_BUTTON_STATE
{
	I3GUI_BUTTON_NORMAL = 0,
	I3GUI_BUTTON_PUSHED,
	I3GUI_BUTTON_FOCUS,
	I3GUI_BUTTON_ONMOUSE,
	I3GUI_BUTTON_DISABLED,

	MAX_I3GUI_BUTTON_STATE
};
*/

enum I3GUI_BUTTON_NOTIFY_TYPE
{
	I3GUI_BUTTON_NOTIFY_NULL = 0,
	I3GUI_BUTTON_NOTIFY_CLICKED,	
	I3GUI_BUTTON_NOTIFY_ONMOUSE,
	MAX_I3GUI_BUTTON_NOTIFY
};

// ============================================================================
//
// i3GuiButton : ¹öÆ°
//
// ============================================================================
class I3_EXPORT_GUI i3GuiButton : public i3GuiControl
{
	I3_CLASS_DEFINE( i3GuiButton );
protected:	
	INT32			m_nID;	
	BOOL			m_bFirstStroke;
	BOOL			m_bDownClickAction;

public:
	i3GuiButton();
	virtual ~i3GuiButton(); 	

	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state);
	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	void SetDownClickAction(BOOL bDownClick);
};

#endif //__I3GUI_BUTTON_H__
