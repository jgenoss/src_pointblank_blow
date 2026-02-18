#ifndef __I3UITEMPLATE_CHECKBUTTON_H__
#define __I3UITEMPLATE_CHECKBUTTON_H__

#include "i3UITemplate.h"

enum I3UI_CHECKBUTTON_SHAPE
{
	I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED = 0,
	I3UI_CHECKBUTTON_SHAPE_ENABLED_CHECKED,
	
	I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED_ONMOUSE, 
	I3UI_CHECKBUTTON_SHAPE_ENABLED_CHECKED_ONMOUSE,

	I3UI_CHECKBUTTON_SHAPE_DISABLED_UNCHECKED,
	I3UI_CHECKBUTTON_SHAPE_DISABLED_CHECKED,

	I3UI_CHECKBUTTON_SHAPE_CAPTION,

	I3UI_CHECKBUTTON_SHAPE_COUNT
};

class I3_EXPORT_FRAMEWORK i3UITemplate_CheckButton : public i3UITemplate
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3UITemplate_CheckButton, i3UITemplate);

protected:
	i3UIShape	m_pShape[ I3UI_CHECKBUTTON_SHAPE_COUNT];

	INT32		m_nCaptionOffset = 10;		//margin
	INT32		m_nCaptionWidth = 150;
	INT32		m_nCaptionHeight = 50;

	bool		m_bRightText = true;
	bool		m_bFreeSize = false;
public:
	virtual INT32			getShapeCount()	const	{ return I3UI_CHECKBUTTON_SHAPE_COUNT;	}
	
	virtual i3UIShape*		getShape(INT32 nIndex) override;
	virtual const char *	getShapeInfoString(INT32 nIndex) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual	bool			GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut) override;

	void					SetRightText( bool bRight)		{ m_bRightText = bRight;	}
	void					SetFreeSize(bool bFree)			{ m_bFreeSize = bFree;		}

public:
	virtual void		Create() override;
};
	
#endif	// __I3UITEMPLATE_CHECKBUTTON_H__