#ifndef	__I3UITEMPLATE_LVITEM_H__
#define __I3UITEMPLATE_LVITEM_H__

#include "i3UITemplate.h"

class i3UITemplate_LVCell;
class i3UITemplate_LVSep;

enum I3UI_LVITEM_SHAPE
{
	I3UI_LVITEM_SHAPE_DISABLED = 0,
	I3UI_LVITEM_SHAPE_NORMAL,
	I3UI_LVITEM_SHAPE_ONMOUSE,
	I3UI_LVITEM_SHAPE_SELECTED,

	I3UI_LVITEM_SHAPE_COUNT
};

enum I3UI_LVITEM_SUBTEMPLATE
{
	I3UI_LVITEM_SUBTEMPLATE_CELL = 0,
	I3UI_LVITEM_SUBTEMPLATE_SEP,

	I3UI_LVITEM_SUBTEMPLATE_COUNT
};

class I3_EXPORT_FRAMEWORK i3UITemplate_LVItem : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_LVItem, i3UITemplate);

protected:
	i3UIShape	m_pShape[I3UI_LVITEM_SHAPE_COUNT];

	i3UITemplate_LVCell*	m_pTCell;				// [initialize at constructor]
	i3UITemplate_LVSep*		m_pTSep;				// [initialize at constructor]
public:
	i3UITemplate_LVItem();
	virtual ~i3UITemplate_LVItem();
	
	// Subtemplate
	virtual INT32			getSubTemplateCount(void) override {	return I3UI_LVITEM_SUBTEMPLATE_COUNT;	}
	virtual i3UITemplate *	getSubTemplate( INT32 idx) override;
	virtual const char *	getSubTemplateComment( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate) override;

	// Inherited Functions
	virtual INT32			getShapeCount()	const				{	return I3UI_LVITEM_SHAPE_COUNT;	}
	virtual i3UIShape*		getShape(INT32 nIndex) override {	I3_BOUNDCHK( nIndex, I3UI_LVITEM_SHAPE_COUNT);	return & m_pShape[nIndex];	}
	virtual const char *	getShapeInfoString(INT32 nIndex) override;
	virtual const char *	getComment(void) override {	return "ListView Item(Comment)";	}
	virtual const char *	getTemplateName(void) override {	return "ListView Item";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override {	return I3UI_CATEGORY_LISTVIEWITEM;	}
	virtual i3ClassMeta	*	GetInstanceMeta() override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual	bool			GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut) override;
};

#endif // __I3UITEMPLATE_LVITEM_H__