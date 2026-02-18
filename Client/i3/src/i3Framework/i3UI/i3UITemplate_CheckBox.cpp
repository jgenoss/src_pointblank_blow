#include "i3FrameworkPCH.h"
#include "i3UITemplate_CheckBox.h"
#include "I3UICheckBox.h"

#include "i3UITemplate_CheckButton.h"

I3_CLASS_INSTANCE( i3UITemplate_CheckBox);

const char * i3UITemplate_CheckBox::getComment(void)
{
	return "Check Box(Comment)";
}

const char * i3UITemplate_CheckBox::getTemplateName(void)
{
	return "Check Box";
}

I3UI_CATEGORY  i3UITemplate_CheckBox::getTemplateCategory(void)
{
	return I3UI_CATEGORY_CHECKBOX;
}

i3ClassMeta	* i3UITemplate_CheckBox::GetInstanceMeta()
{
	return i3UICheckBox::static_meta();
}

