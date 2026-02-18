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
	I3_CLASS_DEFINE( i3GuiTextBox );

protected:
	BOOL					m_bUpdated;

	WCHAR16	*				m_pText;

	INT32					m_nWidth;
	INT32					m_nHeight;

	BOOL					m_bStart;

	REAL32					m_fScrollSpeed;

public:
	i3GuiTextBox();
	virtual ~i3GuiTextBox();  

	virtual void	UserInput( REAL32 fX, REAL32 fY, UINT32 state);
	virtual void	OnUpdate( REAL32 rDeltaSeconds);
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	SetControlShapeTemplate( i3GuiTemplateElement* pShape);

	void			ResetTextBox();
	void			SetTextBox( WCHAR16 * pText);
	void			SetImmediatelyTextBox();
	void			SetTextBoxSize( INT32 nWidth, INT32 nHeight)		{ m_nWidth = nWidth;	m_nHeight = nHeight;	}
	void			setScrollSpeed( REAL32 fSpeed);
	void			StartTextBox();

	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

#endif //__I3GUI_TEXTBOX_H__
