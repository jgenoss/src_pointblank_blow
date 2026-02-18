#include "i3FrameworkPCH.h"
#include "i3UITemplate_VScrollBar.h"
#include "i3UIScrollBar.h"

I3_CLASS_INSTANCE( i3UITemplate_VScrollBar);

i3UITemplate_VScrollBar::i3UITemplate_VScrollBar(void)
{
	m_Style = I3UI_SCROLLBAR_STYLE_VERT;
}


const char *	i3UITemplate_VScrollBar::getComment(void)
{
	return "V ScrollBar";
}

const char *	i3UITemplate_VScrollBar::getTemplateName(void)
{
	return "V ScrollBar";
}

void i3UITemplate_VScrollBar::InitializeLayout(void)
{
	AdjustLayout( 50.0f, 400.0f);
}
