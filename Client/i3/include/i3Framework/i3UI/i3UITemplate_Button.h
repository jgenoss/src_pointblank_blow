#if !defined( __I3UITEMPLATE_BUTTON_H__)
#define __I3UITEMPLATE_BUTTON_H__

#include "i3UITemplate.h"

enum I3UI_BUTTON_SHAPE
{
	I3UI_BUTTON_SHAPE_NORMAL = 0,
	I3UI_BUTTON_SHAPE_PUSHED,
	I3UI_BUTTON_SHAPE_ONMOUSED,
	I3UI_BUTTON_SHAPE_DISABLED,
	I3UI_BUTTON_SHAPE_SELECTEDNORMAL,

	I3UI_BUTTON_SHAPE_COUNT
};

class I3_EXPORT_FRAMEWORK i3UITemplate_Button : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_Button, i3UITemplate);

protected:
	i3UIShape	m_pShape[I3UI_BUTTON_SHAPE_COUNT];

public:	
	virtual INT32			getShapeCount()	const	{	return I3UI_BUTTON_SHAPE_COUNT;}
	virtual i3UIShape*		getShape(INT32 nIndex) override
	{
		I3_BOUNDCHK( nIndex, I3UI_BUTTON_SHAPE_COUNT);

		return & m_pShape[nIndex];
	}

	virtual const char *	getShapeInfoString(INT32 nIndex) override;
	virtual const char *	getComment(void) override {	return "4-State Button";	}
	virtual const char *	getTemplateName(void) override {	return "4-State Button";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;

	virtual	bool			GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut) override;
};

#endif // __I3UITEMPLATE_BUTTON_H__




