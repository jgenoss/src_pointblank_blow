#if !defined( __I3UI_TEMPLATE_DOUBLETEXT_H__)
#define __I3UI_TEMPLATE_DOUBLETEXT_H__

#include "i3UITemplate.h"

enum I3UI_DOUBLETEXT_SHAPE
{
	I3UI_DOUBLETEXT_SHAPE_BKGND = 0,

	I3UI_DOUBLETEXT_SHAPE_COUNT
};

enum I3UI_DOUBLETEXT_SUBTEMPLATE
{
	I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT0 = 0,
	I3UI_DOUBLETEXT_SUBTEMPLATE_TEXT1,

	I3UI_DOUBLETEXT_SUBTEMPLATE_COUNT
};

class i3UITemplate_StaticText;

class I3_EXPORT_FRAMEWORK i3UITemplate_DoubleText : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_DoubleText, i3UITemplate);

private:
	i3UIShape	m_pShape[I3UI_DOUBLETEXT_SHAPE_COUNT];

	i3UITemplate_StaticText *	m_pTStaticText1;
	i3UITemplate_StaticText *	m_pTStaticText2;

public:
	i3UITemplate_DoubleText();
	virtual ~i3UITemplate_DoubleText();

	virtual INT32			getShapeCount()	const	{	return I3UI_DOUBLETEXT_SHAPE_COUNT;}
	virtual i3UIShape*		getShape(INT32 nIndex) override;
	
	virtual const char *	getShapeInfoString(INT32 nIndex) override;

	virtual INT32			getSubTemplateCount(void) override;
	virtual i3UITemplate *	getSubTemplate( INT32 idx) override;
	virtual const char *	getSubTemplateComment( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate) override;

	virtual const char *	getComment(void) override {	return "Double Text";	}
	virtual const char *	getTemplateName(void) override {	return "Double Text";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr,INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;
};

#endif
