#ifndef	__I3UITEMPLATE_LVBOX_H__
#define __I3UITEMPLATE_LVBOX_H__

#include "i3UITemplate.h"

class i3UITemplate_VScrollBar;
class i3UITemplate_LVHeader;

enum I3UI_LVBOX_SUBTEMPLATE
{
	I3UI_LVBOX_SUBTEMPLATE_VSCROLL = 0,
	//I3UI_LVBOX_SUBTEMPLATE_HEADER,
	I3UI_LVBOX_SUBTEMPLATE_COUNT
};

enum I3UI_LVBOX_SHAPE
{
	I3UI_LVBOX_SHAPE_LEFTTOP = 0,
	I3UI_LVBOX_SHAPE_TOP, 
	I3UI_LVBOX_SHAPE_RIGHTTOP, 
	
	I3UI_LVBOX_SHAPE_LEFT, 
	I3UI_LVBOX_SHAPE_CENTER, 
	I3UI_LVBOX_SHAPE_RIGHT,

	I3UI_LVBOX_SHAPE_LEFTBOTTOM,
	I3UI_LVBOX_SHAPE_BOTTOM,
	I3UI_LVBOX_SHAPE_RIGHTBOTTOM,
	
	I3UI_LVBOX_SHAPE_COUNT
};

class I3_EXPORT_FRAMEWORK i3UITemplate_LVBox : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_LVBox, i3UITemplate);

protected:
	i3UIShape	m_pShape[I3UI_LVBOX_SHAPE_COUNT];

	i3UITemplate_VScrollBar*	m_pTScrollBar;		// [initialize at constructor]

public:
	i3UITemplate_LVBox();
	virtual ~i3UITemplate_LVBox();

	// SubTemplate
	virtual INT32			getSubTemplateCount(void) override;
	virtual i3UITemplate *	getSubTemplate( INT32 idx) override;
	virtual const char *	getSubTemplateComment( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate) override;
	
	// Inherited Functions
	virtual INT32			getShapeCount()	const				{	return I3UI_LVBOX_SHAPE_COUNT;	}
	virtual i3UIShape*		getShape(INT32 nIndex) override {	I3_BOUNDCHK( nIndex, I3UI_LVBOX_SHAPE_COUNT);	return & m_pShape[nIndex];	}
	virtual const char *	getShapeInfoString(INT32 nIndex) override;
	virtual const char *	getComment(void) override {	return "ListView Box(Comment)";	}
	virtual const char *	getTemplateName(void) override {	return "ListView Box";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override {	return I3UI_CATEGORY_LISTVIEWBOX;	}
	virtual i3ClassMeta	*	GetInstanceMeta() override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual	bool			GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut) override;
};

#endif // __I3UITEMPLATE_LVBOX_H__