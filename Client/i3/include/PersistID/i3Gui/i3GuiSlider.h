#ifndef __I3GUI_SLIDER_H__
#define __I3GUI_SLIDER_H__

#include "i3GuiControl.h"

/*
enum I3GUI_SLIDER_STATE
{
	I3GUI_SLIDER_NORMAL = 0,
	I3GUI_SLIDER_ONMOUSE,
	I3GUI_SLIDER_DRAG,
	I3GUI_SLIDER_DISABLED,

	MAX_I3GUI_SLIDER_STATE
};
*/

enum I3GUI_SLIDER_NOTIFY_TYPE
{
	I3GUI_SLIDER_NOTIFY_NULL = 0,
	I3GUI_SLIDER_NOTIFY_DRAG,	
	MAX_I3GUI_SLIDER_NOTIFY
};


// ============================================================================
//
// i3GuiSlider
//
// ============================================================================
class I3_EXPORT_GUI i3GuiSlider : public i3GuiControl
{
	I3_CLASS_DEFINE( i3GuiSlider );

protected:
	BOOL					m_bToWidth;

	REAL32					m_fCurrentValue;
	REAL32					m_fMinValue;
	REAL32					m_fMaxValue;
	REAL32					m_fSliderPos;
	REAL32					m_fSliderScale;

	i3GuiTemplateElement*	m_pShape;

public:
	i3GuiSlider();
	virtual ~i3GuiSlider();  

	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape);
	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state);
	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	// Slider의 현재 수치에 관련된 함수입니다.
	REAL32			getCurValue()							{ return m_fCurrentValue;					}
	void			setCurValue( REAL32 fValue);

	void			UpdatePos();
	void			SetSliderPos( REAL32 fPos);

	REAL32			getMinValue()							{ return m_fMinValue;						}
	REAL32			getMaxValue()							{ return m_fMaxValue;						}
	void			setMinMaxValue(REAL32 fMin, REAL32 fMax){ m_fMinValue = fMin; m_fMaxValue = fMax;	}

	// bTrue가 TRUE일 경우에는 슬라이더가 가로로 움직이며, FALSE일 경우에는 세로로 움직입니다.
	void			setToWidth( BOOL bTrue);

	virtual void	OnBuildObjectList( i3List * pList );
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

#endif //__I3GUI_SLIDER_H__
