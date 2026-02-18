#ifndef __I3UITEMPLATE_RADIO_H__
#define __I3UITEMPLATE_RADIO_H__

#include "i3UITemplate_CheckButton.h"

class I3_EXPORT_FRAMEWORK i3UITemplate_Radio : public i3UITemplate_CheckButton
{
	I3_EXPORT_CLASS_DEFINE( i3UITemplate_Radio, i3UITemplate_CheckButton);

public:
	virtual const char *	getComment(void) override;
	virtual const char *	getTemplateName(void) override;
	virtual I3UI_CATEGORY	getTemplateCategory(void) override;
	virtual i3ClassMeta	*	GetInstanceMeta() override;

};
	
#endif	// __I3UITEMPLATE_RADIO_H__