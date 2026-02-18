#ifndef __I3UITEMPLATE_3DVIEW_H__
#define __I3UITEMPLATE_3DVIEW_H__

#include "i3UITemplate.h"

class I3_EXPORT_FRAMEWORK i3UITemplate_3DView : public i3UITemplate
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_3DView, i3UITemplate );


public:	
	virtual INT32			getShapeCount()	const	{	return 0;}
	virtual i3UIShape*		getShape(INT32 nIndex) override {	return nullptr; }
	
	virtual const char *	getShapeInfoString(INT32 nIndex) override { return nullptr; }

	virtual const char *	getComment(void) override {	return "3DView Box";		}
	virtual const char *	getTemplateName(void) override {	return "3DView Box";		}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;

	virtual bool			GetControlRect( i3::pack::RECT * pRect) override;
	virtual void			InitializeLayout(void) override;
	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;
	virtual void			SpreadLayout(REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta	*	GetInstanceMeta() override;
};

#endif // __I3UITEMPLATE_3DVIEW_H__
