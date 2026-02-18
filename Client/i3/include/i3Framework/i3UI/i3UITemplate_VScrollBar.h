#if !defined( __I3UI_TEMPLATE_VERT_SCROLLBAR_H)
#define __I3UI_TEMPLATE_VERT_SCROLLBAR_H

#include "i3UITemplate.h"
#include "i3UITemplate_Button.h"
#include "i3UITemplate_ScrollBar.h"

class I3_EXPORT_FRAMEWORK i3UITemplate_VScrollBar : public i3UITemplate_ScrollBar
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_VScrollBar, i3UITemplate_ScrollBar);

public:
	i3UITemplate_VScrollBar(void);

	virtual const char *	getComment(void) override;
	virtual const char *	getTemplateName(void) override;

	virtual void			InitializeLayout(void) override;
};

#endif
