#if !defined( __I3UI_TEMPLATE_SCROLLBAR_H)
#define __I3UI_TEMPLATE_SCROLLBAR_H

#include "i3UITemplate.h"
#include "i3UITemplate_Button.h"

enum I3UI_SCROLLBAR_SHAPE
{
	I3UI_SCROLLBAR_SHAPE_BKGND,

	I3UI_SCROLLBAR_SHAPE_COUNT
};

enum I3UI_SCROLLBAR_STYLE
{
	I3UI_SCROLLBAR_STYLE_HORZ = 0,
	I3UI_SCROLLBAR_STYLE_VERT,
};

enum I3UI_SCROLLBAR_BTN
{
	I3UI_SCROLLBAR_BTN_PREV = 0,
	I3UI_SCROLLBAR_BTN_NEXT,
	I3UI_SCROLLBAR_BTN_TRACK,

	I3UI_SCROLLBAR_BTN_COUNT
};

class I3_EXPORT_FRAMEWORK i3UITemplate_ScrollBar : public i3UITemplate
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3UITemplate_ScrollBar, i3UITemplate);

protected:
	I3UI_SCROLLBAR_STYLE	m_Style = I3UI_SCROLLBAR_STYLE_VERT;
	i3UIShape				m_Shapes[I3UI_SCROLLBAR_SHAPE_COUNT];

	// Sub Templates
	i3UITemplate_Button *	m_pButtons[I3UI_SCROLLBAR_BTN_COUNT];

public:
	i3UITemplate_ScrollBar(void);
	virtual ~i3UITemplate_ScrollBar(void);

	virtual INT32			getShapeCount(void) const;
	virtual i3UIShape*		getShape(INT32 nIndex) override;
	virtual const char *	getShapeInfoString(INT32 nIndex) override;

	virtual INT32			getSubTemplateCount(void) override;
	virtual i3UITemplate *	getSubTemplate( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate) override;

	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;

	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;

public:
	I3UI_SCROLLBAR_STYLE	getScrollBarStyle(void)	{	return m_Style;}
};

#endif
