#if !defined( __I3UITEMPLATE_EDITBOX_H__)
#define __I3UITEMPLATE_EDITBOX_H__

#include "i3UITemplate.h"
#include "i3UITemplate_VScrollBar.h"

enum I3UI_EDITBOX_SUBTEMPLATE
{
	I3UI_EDITBOX_SUBTEMPLATE_VSCROLL = 0,

	I3UI_EDITBOX_SUBTEMPLATE_COUNT
};

enum I3UI_EDITBOX_SHAPE
{
	I3UI_EDITBOX_SHAPE_TOPLEFT = 0,
	I3UI_EDITBOX_SHAPE_TOP,
	I3UI_EDITBOX_SHAPE_TOPRIGHT,

	I3UI_EDITBOX_SHAPE_LEFT,
	I3UI_EDITBOX_SHAPE_CLIENTAREA,
	I3UI_EDITBOX_SHAPE_RIGHT,

	I3UI_EDITBOX_SHAPE_BOTTOMLEFT,
	I3UI_EDITBOX_SHAPE_BOTTOM,
	I3UI_EDITBOX_SHAPE_BOTTOMRIGHT,

	I3UI_EDITBOX_SHAPE_DISABLE_CLIENTAREA,

	I3UI_EDITBOX_SHAPE_COUNT,
};

class I3_EXPORT_FRAMEWORK i3UITemplate_EditBox : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_EditBox, i3UITemplate);

protected:
	i3UIShape	m_pShape[I3UI_EDITBOX_SHAPE_COUNT];

	i3UITemplate_VScrollBar*		m_pTScrollBar;

public:
	i3UITemplate_EditBox();
	virtual ~i3UITemplate_EditBox();
	
	virtual INT32			getShapeCount()	const	{	return I3UI_EDITBOX_SHAPE_COUNT;}
	virtual i3UIShape*		getShape(INT32 nIndex) override
	{
		I3_BOUNDCHK( nIndex, I3UI_EDITBOX_SHAPE_COUNT);

		return & m_pShape[nIndex];
	}

	virtual INT32			getSubTemplateCount(void) override;
	virtual i3UITemplate *	getSubTemplate( INT32 idx) override;
	virtual const char *	getSubTemplateComment( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate) override;

	virtual const char *	getShapeInfoString(INT32 nIndex) override;

	virtual const char *	getComment(void) override {	return "Edit Box";	}
	virtual const char *	getTemplateName(void) override {	return "Edit Box";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;

	virtual	bool			GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut) override;
};

#endif // __I3UITEMPLATE_EDITBOX_H__




