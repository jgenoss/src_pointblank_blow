#if !defined( __I3UI_TEMPLATE_SPIN_H)
#define __I3UI_TEMPLATE_SPIN_H

#include "i3UITemplate.h"
#include "i3UITemplate_Button.h"

enum I3UI_SPIN_SHAPE
{
	I3UI_SPIN_SHAPE_BKGND = 0,

	I3UI_SPIN_SHAPE_COUNT
};

enum I3UI_SPIN_BTN
{
	I3UI_SPIN_BTN_PREV = 0,
	I3UI_SPIN_BTN_NEXT,

	I3UI_SPIN_BTN_COUNT
};

class I3_EXPORT_FRAMEWORK i3UITemplate_Spin : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_Spin, i3UITemplate);

protected:
	i3UIShape				m_Shapes[I3UI_SPIN_SHAPE_COUNT];

	// Sub Templates
	i3UITemplate_Button *	m_pButtons[I3UI_SPIN_BTN_COUNT];

public:
	i3UITemplate_Spin(void);
	virtual ~i3UITemplate_Spin(void);

	virtual INT32			getShapeCount(void) const;
	virtual i3UIShape*		getShape(INT32 nIndex) override;
	virtual const char *	getShapeInfoString(INT32 nIndex) override;

	virtual const char *	getComment(void) override;
	virtual const char *	getTemplateName(void) override;

	virtual INT32			getSubTemplateCount(void) override;
	virtual i3UITemplate *	getSubTemplate( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate) override;

	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;

	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;
};

#endif	//__I3UI_TEMPLATE_SPIN_H
