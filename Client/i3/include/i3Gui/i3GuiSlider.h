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
	I3_EXPORT_CLASS_DEFINE( i3GuiSlider, i3GuiControl );

protected:
	bool					m_bToWidth = true;

	REAL32					m_fCurrentValue = 0.0f;
	REAL32					m_fMinValue = 0.0f;
	REAL32					m_fMaxValue = 0.0f;
	REAL32					m_fSliderPos = 0.0f;
	REAL32					m_fSliderScale = 0.0f;

	i3GuiTemplateElement*	m_pShape = nullptr;

public:
	i3GuiSlider();

	virtual void	SetControlShapeTemplate(i3GuiTemplateElement* pShape) override;
	virtual void	UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;

	// Slider의 현재 수치에 관련된 함수입니다.
	REAL32			getCurValue()							{ return m_fCurrentValue;					}
	void			setCurValue( REAL32 fValue);

	void			UpdatePos();
	void			SetSliderPos( REAL32 fPos);

	REAL32			getMinValue()							{ return m_fMinValue;						}
	REAL32			getMaxValue()							{ return m_fMaxValue;						}
	void			setMinMaxValue(REAL32 fMin, REAL32 fMax){ m_fMinValue = fMin; m_fMaxValue = fMax;	}

	// bTrue가 true일 경우에는 슬라이더가 가로로 움직이며, false일 경우에는 세로로 움직입니다.
	void			setToWidth( bool bTrue);

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;
};

#endif //__I3GUI_SLIDER_H__
