#ifndef __I3UITEMPLATE_COMBOBOX_H__
#define __I3UITEMPLATE_COMBOBOX_H__

#include "i3UITemplate.h"
#include "i3UITemplate_EditBox.h"
#include "i3UITemplate_Button.h"
#include "i3UITemplate_ListBox.h"

class I3_EXPORT_FRAMEWORK i3UITemplate_ComboBox : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_ComboBox, i3UITemplate );

protected:
	i3UIShape	m_pShape[I3UI_COMBOBOX_SHAPE_COUNT];

	i3UITemplate_EditBox*		m_pTEditBox = i3UITemplate_EditBox::new_object();
	i3UITemplate_Button*		m_pTButton = i3UITemplate_Button::new_object();
	i3UITemplate_ListBox*		m_pTListBox = i3UITemplate_ListBox::new_object();

public:
	virtual ~i3UITemplate_ComboBox();
	
	virtual INT32			getShapeCount()	const	{	return I3UI_COMBOBOX_SHAPE_COUNT;}
	virtual i3UIShape*		getShape(INT32 nIndex) override;
	
	virtual const char *	getShapeInfoString(INT32 nIndex) override;

	virtual INT32			getSubTemplateCount(void) override;
	virtual i3UITemplate *	getSubTemplate( INT32 idx) override;
	virtual const char *	getSubTemplateComment( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate) override;

	virtual const char *	getComment(void) override {	return "Combo Box";	}
	virtual const char *	getTemplateName(void) override {	return "Combo Box";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;
	
};

#endif // __I3UITEMPLATE_COMBOBOX_H__
