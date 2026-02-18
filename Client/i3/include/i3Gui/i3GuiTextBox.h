#ifndef __I3GUI_TEXTBOX_H__
#define __I3GUI_TEXTBOX_H__

#include "i3GuiEditBox.h"

/*
enum I3GUI_TEXTBOX_STATE
{
	I3GUI_TEXTBOX_NORMAL = 0,
	I3GUI_TEXTBOX_ONMOUSE,	
	I3GUI_TEXTBOX_PROGRESS,
	I3GUI_TEXTBOX_DISABLED,

	MAX_I3GUI_TEXTBOX_STATE
};
*/
// ============================================================================
//
// i3GuiTextBox
//
// ============================================================================
class I3_EXPORT_GUI i3GuiTextBox : public i3GuiEditBox
{
	I3_EXPORT_CLASS_DEFINE( i3GuiTextBox, i3GuiEditBox );

protected:
	bool					m_bUpdated = false;

	WCHAR16	*				m_pText = nullptr;

	INT32					m_nWidth = 0;
	INT32					m_nHeight = 0;

	bool					m_bStart = false;

	REAL32					m_fScrollSpeed = 0.03f;

public:
	i3GuiTextBox();
	virtual ~i3GuiTextBox() {}

	virtual void	UserInput( REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	SetControlShapeTemplate( i3GuiTemplateElement* pShape) override;

	void			ResetTextBox();
	void			SetTextBox( WCHAR16 * pText);
	void			SetImmediatelyTextBox();
	void			SetTextBoxSize( INT32 nWidth, INT32 nHeight)		{ m_nWidth = nWidth;	m_nHeight = nHeight;	}
	void			setScrollSpeed( REAL32 fSpeed);
	void			StartTextBox();

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;
};

#endif //__I3GUI_TEXTBOX_H__
