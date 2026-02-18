#if !defined( __I3UITEMPLATE_BUTTONIMAGESET_H__)
#define __I3UITEMPLATE_BUTTONIMAGESET_H__

#include "i3UITemplate.h"
#include "i3UITemplate_User.h"
#include "i3UITemplate_ImageBox.h"

enum I3UI_IMAGESET_IB {
	I3UI_IMAGESET_IB_OVER = 0,
	I3UI_IMAGESET_IB_DOWN,
	I3UI_IMAGESET_IB_DISABLED,

	I3UI_IMAGESET_IB_COUNT
};

class I3_EXPORT_FRAMEWORK i3UITemplate_ButtonImageSet : public i3UITemplate_User
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_ButtonImageSet, i3UITemplate_User);

protected:
	i3UITemplate_ImageBox*	m_pIB[I3UI_IMAGESET_IB_COUNT];

public:
	i3UITemplate_ButtonImageSet();
	virtual ~i3UITemplate_ButtonImageSet();

	virtual INT32			addSubTemplate( i3UITemplate * pTemplate) override;

	virtual INT32			getSubTemplateCount(void) override;
	virtual i3UITemplate*	getSubTemplate( INT32 idx) override;
	virtual void			setSubTemplate( INT32 idx, i3UITemplate* pTemplate) override;

	virtual const char *	getComment(void) override {	return "Button Image Set";	}
	virtual const char *	getTemplateName(void) override {	return "Button Image Set";	}
	virtual I3UI_CATEGORY	getTemplateCategory(void) override { return I3UI_CATEGORY_BUTTON; }

	virtual bool			AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects = nullptr, INT32 shapeCount = 0) override;

	virtual i3ClassMeta*	GetInstanceMeta() override;
};

#endif // __I3UITEMPLATE_BUTTONIMAGESET_H__




