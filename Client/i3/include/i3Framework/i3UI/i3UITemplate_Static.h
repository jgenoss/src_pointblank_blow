#if !defined( __I3UITEMPLATE_STATIC_H__)
#define __I3UITEMPLATE_STATIC_H__

#include "i3UITemplate.h"

enum I3UI_STATIC_SHAPE
{
	I3UI_STATIC_SHAPE_NORMAL = 0,
	I3UI_STATIC_SHAPE_DISABLED,

	I3UI_STATIC_SHAPE_COUNT,
};

class I3_EXPORT_FRAMEWORK i3UITemplate_Static : public i3UITemplate
{
	I3_CLASS_DEFINE( i3UITemplate_Static );

protected:
	i3UIImage	m_pShape[I3UI_STATIC_SHAPE_COUNT];

public:
	i3UITemplate_Static();
	virtual ~i3UITemplate_Static();
	
	virtual INT32			getShapeCount()			{	return I3UI_STATIC_SHAPE_COUNT;}
	virtual i3UIImage*		getShape(INT32 nIndex)
	{
		I3_BOUNDCHK( nIndex, I3UI_STATIC_SHAPE_COUNT);

		return & m_pShape[nIndex];
	}

	virtual const char *	getShapeInfoString(INT32 nIndex);

	virtual const char *	getComment(void);
};

#endif // __I3UITEMPLATE_STATIC_H__
