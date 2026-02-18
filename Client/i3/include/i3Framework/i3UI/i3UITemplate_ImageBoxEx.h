#ifndef __I3UITEMPLATE_IMAGEBOXEX_H__
#define __I3UITEMPLATE_IMAGEBOXEX_H__

#include "i3UITemplate.h"

enum I3UI_IMAGEBOXEX_SHAPE
{
	I3UI_IMAGEBOXEX_SHAPE_IMAGE = 0,
	
	I3UI_IMAGEBOXEX_SHAPE_COUNT,
};

class I3_EXPORT_FRAMEWORK i3UITemplate_ImageBoxEx : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_ImageBoxEx, i3UITemplate );

protected:
	i3UIShape	m_pShape[I3UI_IMAGEBOXEX_SHAPE_COUNT];

public:	
	virtual INT32			getShapeCount()	const	{	return I3UI_IMAGEBOXEX_SHAPE_COUNT;}
	virtual i3UIShape*		getShape(INT32 nIndex) override;
	
	virtual const char *	getShapeInfoString(INT32 nIndex) override;

	virtual const char *	getComment(void) override {	return "Image Box Ex";		}
	virtual const char *	getTemplateName(void) override {	return "Image Box Ex";		}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;
};

#endif // __I3UITEMPLATE_IMAGEBOXEX_H__
