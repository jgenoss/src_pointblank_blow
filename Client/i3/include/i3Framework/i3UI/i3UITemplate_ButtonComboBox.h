#ifndef __I3UITEMPLATE_BUTTONCOMBOBOX_H__
#define __I3UITEMPLATE_BUTTONCOMBOBOX_H__

#include "i3UITemplate.h"
#include "i3UITemplate_EditBox.h"
#include "i3UITemplate_Button.h"
#include "i3UITemplate_ButtonComposed3.h"
#include "i3UITemplate_ListBox.h"

class I3_EXPORT_FRAMEWORK i3UITemplate_ButtonComboBox : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_ButtonComboBox, i3UITemplate );

protected:
	i3UIShape	m_pShape[I3UI_COMBOBOX_SHAPE_COUNT];

	i3UITemplate_ButtonComposed3*	m_pTEditBox = i3UITemplate_ButtonComposed3::new_object();
	i3UITemplate_Button*		m_pTButton = i3UITemplate_Button::new_object();
	i3UITemplate_ListBox*		m_pTListBox = i3UITemplate_ListBox::new_object();

public:
	virtual ~i3UITemplate_ButtonComboBox();
	
	virtual INT32			getShapeCount()	const	{	return I3UI_COMBOBOX_SHAPE_COUNT;}
	virtual i3UIShape*		getShape(INT32 nIndex);
	
	virtual const char *	getShapeInfoString(INT32 nIndex);

	virtual INT32			getSubTemplateCount(void);
	virtual i3UITemplate *	getSubTemplate( INT32 idx);
	virtual const char *	getSubTemplateComment( INT32 idx);
	virtual void			setSubTemplate( INT32 idx, i3UITemplate * pTemplate);

	virtual const char *	getComment(void)		{	return "ButtonCombo Box";	}
	virtual const char *	getTemplateName(void)	{	return "ButtonCombo Box";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void);

	virtual bool			GetControlRect( i3::pack::RECT * pRect);
	virtual void			InitializeLayout(void);
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0);
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0);

	virtual i3ClassMeta	*	GetInstanceMeta();
	
};

#endif // __I3UITEMPLATE_COMBOBOX_H__
