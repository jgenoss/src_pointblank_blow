#include "i3FrameworkPCH.h"
#include "i3UITemplate_HScrollBar.h"
#include "i3UIScrollBar.h"


I3_CLASS_INSTANCE( i3UITemplate_HScrollBar);

i3UITemplate_HScrollBar::i3UITemplate_HScrollBar()
{
	m_Style = I3UI_SCROLLBAR_STYLE_HORZ;
}

const char *	i3UITemplate_HScrollBar::getComment(void)
{
	return "H Scroll Bar";
}

const char *	i3UITemplate_HScrollBar::getTemplateName(void)
{
	return "H Scroll Bar";
}

void i3UITemplate_HScrollBar::InitializeLayout(void)
{
	AdjustLayout( 400.0f, 50.0f);
}