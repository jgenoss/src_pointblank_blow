#ifndef __I3GUI_CHECKBOX_H__
#define __I3GUI_CHECKBOX_H__

#include "i3GuiControl.h"

/*
enum I3GUI_CHECKBOX_STATE
{
	I3GUI_CHECKBOX_NORMAL = 0,
	I3GUI_CHECKBOX_CLICKED,
	I3GUI_CHECKBOX_FOCUS,
	I3GUI_CHECKBOX_DISABLED,

	MAX_I3GUI_CHECKBOX_STATE
};
*/

enum I3GUI_CHECK_NOTIFY_TYPE
{
	I3GUI_CHECK_NOTIFY_NULL = 0,
	I3GUI_CHECK_NOTIFY_CLICKED,
	I3GUI_CHECK_NOTIFY_ONMOUSE,

	MAX_I3GUI_CHECK_NOTIFY
};

// ============================================================================
//
// i3GuiCheckBox 
//
// ============================================================================
class I3_EXPORT_GUI i3GuiCheckBox : public i3GuiControl
{
	I3_CLASS_DEFINE( i3GuiCheckBox );

protected:
	BOOL					m_bCheck;
	INT32					m_nCheckIdx;

public:
	i3GuiCheckBox();
	virtual ~i3GuiCheckBox();  

	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state);
	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape);

	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	BOOL			getCheckBoxState()						{ return m_bCheck;		}
	void			setCheckBoxState( BOOL bState);

	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );

	virtual void	SetShapeEnable( INT32 index, BOOL bEnable);
};

#endif //__I3GUI_CHECKBOX_H__
