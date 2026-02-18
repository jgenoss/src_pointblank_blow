#ifndef __I3GUI_RADIOBUTTON_H__
#define __I3GUI_RADIOBUTTON_H__

#include "i3GuiControl.h"

/*
enum I3GUI_RADIOBUTTON_STATE
{
	I3GUI_RADIOBUTTON_NORMAL = 0,
	I3GUI_RADIOBUTTON_FOCUS,
	I3GUI_RADIOBUTTON_DISABLED,

	MAX_I3GUI_RADIOBUTTON_STATE
};
*/

enum I3GUI_RADIO_NOTIFY_TYPE
{
	I3GUI_RADIO_NOTIFY_NULL = 0,
	I3GUI_RADIO_NOTIFY_CLICKED,

	MAX_I3GUI_RADIO_NOTIFY
};

// ============================================================================
//
// i3GuiRadioButton
//
// ============================================================================
class I3_EXPORT_GUI i3GuiRadioButton : public i3GuiControl
{
	I3_EXPORT_CLASS_DEFINE( i3GuiRadioButton, i3GuiControl );

protected:
	INT32					m_ButtonCount = 0;
	INT32					m_CheckedIndex = 0;

public:
	i3GuiRadioButton();

	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;

	//
	void			setCheckedIndex( INT32 index)			{ m_CheckedIndex = index;	}
	INT32			getCheckedIndex()						{ return m_CheckedIndex;	}
	void			setButtonCount( INT32 nCount)			{ m_ButtonCount = nCount;	}
	INT32			getButtonCount()						{ return m_ButtonCount;		}

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;
};

#endif //__I3GUI_RADIOBUTTON_H__
