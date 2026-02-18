#ifndef __I3UI_RADIOBUTTON_H__
#define __I3UI_RADIOBUTTON_H__

#include "i3UIControl.h"
#include "i3UITemplate_RadioButton.h"

// ============================================================================
//
// i3UIRadioButton
//
// ============================================================================
class I3_EXPORT_FRAMEWORK i3UIRadioButton : public i3UIControl
{
	I3_CLASS_DEFINE( i3UIRadioButton );

protected:
	void			_UpdateShape(void);

public:
	i3UIRadioButton(void);
	virtual ~i3UIRadioButton(void);

	bool			getSelected(void)				{ return isControlState( I3UI_CTRL_STATE_SELECTED); }
	void			setSelected( bool bState);

	virtual void	OnBindTemplate(void);
	virtual void	OnEnabled( bool bEnable);
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnKeyUp( UINT32 nKey);

	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

#endif //__I3UI_RADIOBUTTON_H__
