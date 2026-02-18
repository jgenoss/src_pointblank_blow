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

enum I3GUI_BUTTON_STYLE
{
	I3GUI_BUTTON_STYLE_PUSH_2STATE = 0,			// Normal, Push
	I3GUI_BUTTON_STYLE_PUSH_3STATE,				// Normal, OnMouse, Pushed,
	I3GUI_BUTTON_STYLE_PUSH_4STATE,				// Normal, OnMouse, Pushed, Disabled
};

// ============================================================================
//
// i3GuiButton : ¹öÆ°
//
// ============================================================================
class I3_EXPORT_GUI i3GuiButton : public i3GuiControl
{
	I3_EXPORT_CLASS_DEFINE( i3GuiButton, i3GuiControl );
protected:	
	INT32			m_nID;							// [initialize at constructor]
	bool			m_bFirstStroke = false;
	bool			m_bDownClickAction = false;

public:
	i3GuiButton();

	void			SetStateAs( I3GUI_BUTTON_STYLE style);

	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	void SetDownClickAction(bool bDownClick);
};

#endif //__I3GUI_BUTTON_H__
