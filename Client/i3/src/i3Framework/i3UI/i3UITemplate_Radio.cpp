#include "i3FrameworkPCH.h"
#include "i3UITemplate_Radio.h"
#include "I3UIRadio.h"

I3_CLASS_INSTANCE( i3UITemplate_Radio);


const char * i3UITemplate_Radio::getComment(void)
{
	return "Radio Button(Comment)";
}

const char * i3UITemplate_Radio::getTemplateName(void)
{
	return "Radio Button";
}

I3UI_CATEGORY  i3UITemplate_Radio::getTemplateCategory(void)
{
	return i3UI_CATEGORY_RADIO;
}

i3ClassMeta	* i3UITemplate_Radio::GetInstanceMeta()
{
	return i3UIRadio::static_meta();
}
