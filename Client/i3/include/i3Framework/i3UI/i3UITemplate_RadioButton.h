#if !defined( __I3UITEMPLATE_RADIOBUTTON_H__)
#define __I3UITEMPLATE_RADIOBUTTON_H__

#include "i3UITemplate.h"

enum I3UI_RADIOBUTTON_SHAPE
{
	I3UI_RADIOBUTTON_SHAPE_NORMAL = 0,
	I3UI_RADIOBUTTON_SHAPE_ONMOUSE,
	I3UI_RADIOBUTTON_SHAPE_DISABLED,
	I3UI_RADIOBUTTON_SHAPE_PUSH_NORMAL,
	I3UI_RADIOBUTTON_SHAPE_PUSH_ONMOUSE,
	I3UI_RADIOBUTTON_SHAPE_PUSH_DISABLED,

	I3UI_RADIOBUTTON_SHAPE_COUNT,
};

class I3_EXPORT_FRAMEWORK i3UITemplate_RadioButton : public i3UITemplate
{
	I3_CLASS_DEFINE( i3UITemplate_RadioButton );

protected:
	i3UIImage	m_pShape[I3UI_RADIOBUTTON_SHAPE_COUNT];

public:
	i3UITemplate_RadioButton();
	virtual ~i3UITemplate_RadioButton();
	
	virtual INT32			getShapeCount()			{	return I3UI_RADIOBUTTON_SHAPE_COUNT;}
	virtual i3UIImage*		getShape(INT32 nIndex)
	{
		I3_BOUNDCHK( nIndex, I3UI_RADIOBUTTON_SHAPE_COUNT);

		return & m_pShape[nIndex];
	}

	virtual const char *	getShapeInfoString(INT32 nIndex);

	virtual const char *	getComment(void);
};

#endif // __I3UITEMPLATE_RADIOBUTTON_H__
