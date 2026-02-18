#if !defined( __I3UI_TEMPLATE_FRAMEWND_H)
#define __I3UI_TEMPLATE_FRAMEWND_H

#include "i3UITemplate.h"
#include "i3UITemplate_VScrollBar.h"
#include "i3UITemplate_HScrollBar.h"

enum I3UI_FRAMEWND_SUBTEMPLATE
{
	I3UI_FRAMEWND_SUBTEMPLATE_VSCROLL = 0,
	I3UI_FRAMEWND_SUBTEMPLATE_HSCROLL,

	I3UI_FRAMEWND_SUBTEMPLATE_COUNT
};

enum I3UI_FRAMEWND_SHAPE
{
	I3UI_FRAMEWND_SHAPE_TOPLEFT = 0,
	I3UI_FRAMEWND_SHAPE_TOP,
	I3UI_FRAMEWND_SHAPE_TOPRIGHT,

	I3UI_FRAMEWND_SHAPE_LEFT,
	I3UI_FRAMEWND_SHAPE_CLIENT,
	I3UI_FRAMEWND_SHAPE_RIGHT,

	I3UI_FRAMEWND_SHAPE_BOTTOMLEFT,
	I3UI_FRAMEWND_SHAPE_BOTTOM,
	I3UI_FRAMEWND_SHAPE_BOTTOMRIGHT,

	I3UI_FRAMEWND_SHAPE_DISABLE_CLIENT,

	I3UI_FRAMEWND_SHAPE_COUNT
};

class I3_EXPORT_FRAMEWORK i3UITemplate_FrameWnd : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_FrameWnd, i3UITemplate);
protected:
	i3UIShape				m_Shapes[I3UI_FRAMEWND_SHAPE_COUNT];

	i3UITemplate_VScrollBar*		m_pTVScrollBar;
	i3UITemplate_HScrollBar*		m_pTHScrollBar;

public:
	i3UITemplate_FrameWnd(void);
	virtual ~i3UITemplate_FrameWnd(void);

	virtual INT32			getShapeCount(void) const;
	virtual i3UIShape*		getShape(INT32 nIndex) override;
	virtual const char *	getShapeInfoString(INT32 nIndex) override;

	virtual INT32			getSubTemplateCount(void) override;
	virtual i3UITemplate *	getSubTemplate( INT32 idx) override;
	virtual const char *	getSubTemplateComment( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate) override;

	virtual const char *	getComment(void) override {	return "Frame Window(comment)";	}
	virtual const char *	getTemplateName(void) override {	return "Frame Window";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;

	virtual	bool			GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut) override;
};

#endif
