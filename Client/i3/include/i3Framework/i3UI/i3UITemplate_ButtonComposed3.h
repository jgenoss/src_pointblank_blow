#if !defined( __I3UITEMPLATE_BUTTON_COMPOSED3_H__)
#define __I3UITEMPLATE_BUTTON_COMPOSED3_H__

#include "i3UITemplate.h"

enum I3UI_BUTTON_COMP3_SHAPE
{
	I3UI_BUTTON_COMP3_SHAPE_NORMAL_LEFT	= 0,
	I3UI_BUTTON_COMP3_SHAPE_NORMAL_CENTER,
	I3UI_BUTTON_COMP3_SHAPE_NORMAL_RIGHT,

	I3UI_BUTTON_COMP3_SHAPE_PUSHED_LEFT,
	I3UI_BUTTON_COMP3_SHAPE_PUSHED_CENTER,
	I3UI_BUTTON_COMP3_SHAPE_PUSHED_RIGHT,

	I3UI_BUTTON_COMP3_SHAPE_ONMOUSED_LEFT,
	I3UI_BUTTON_COMP3_SHAPE_ONMOUSED_CENTER,
	I3UI_BUTTON_COMP3_SHAPE_ONMOUSED_RIGHT,

	I3UI_BUTTON_COMP3_SHAPE_DISABLED_LEFT,
	I3UI_BUTTON_COMP3_SHAPE_DISABLED_CENTER,
	I3UI_BUTTON_COMP3_SHAPE_DISABLED_RIGHT,

	I3UI_BUTTON_COMP3_SHAPE_COUNT
};

class I3_EXPORT_FRAMEWORK i3UITemplate_ButtonComposed3 : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_ButtonComposed3, i3UITemplate );

protected:
	i3UIShape	m_pShape[I3UI_BUTTON_COMP3_SHAPE_COUNT];

public:	
	virtual INT32			getShapeCount()	const		{	return I3UI_BUTTON_COMP3_SHAPE_COUNT;}
	virtual i3UIShape*		getShape(INT32 nIndex) override
	{
		I3_BOUNDCHK( nIndex, I3UI_BUTTON_COMP3_SHAPE_COUNT);

		return & m_pShape[nIndex];
	}

	virtual const char *	getShapeInfoString(INT32 nIndex) override;

	virtual const char *	getComment(void) override {	return "4-State composed3 Button";	}
	virtual const char *	getTemplateName(void) override {	return "4-State composed3 Button";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;

	virtual	bool			GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut) override;
};

#endif // __I3UITEMPLATE_BUTTON_H__




