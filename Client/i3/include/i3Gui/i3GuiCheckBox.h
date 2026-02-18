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
	I3_EXPORT_CLASS_DEFINE( i3GuiCheckBox, i3GuiControl );

protected:
	bool					m_bCheck = false;
	INT32					m_nCheckIdx = 0;

public:
	i3GuiCheckBox();

	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;

	bool			getCheckBoxState()						{ return m_bCheck;		}
	void			setCheckBoxState( bool bState);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	SetShapeEnable( INT32 index, bool bEnable) override;
};

#endif //__I3GUI_CHECKBOX_H__
