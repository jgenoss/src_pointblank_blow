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
	I3_CLASS_DEFINE( i3GuiRadioButton );

protected:
	INT32					m_ButtonCount;
	INT32					m_CheckedIndex;

public:
	i3GuiRadioButton();
	virtual ~i3GuiRadioButton();  

	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state);
	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape);

	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	//
	void			setCheckedIndex( INT32 index)			{ m_CheckedIndex = index;	}
	INT32			getCheckedIndex()						{ return m_CheckedIndex;	}
	void			setButtonCount( INT32 nCount)			{ m_ButtonCount = nCount;	}
	INT32			getButtonCount()						{ return m_ButtonCount;		}

	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

#endif //__I3GUI_RADIOBUTTON_H__
