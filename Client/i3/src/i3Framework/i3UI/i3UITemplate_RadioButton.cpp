#include "i3FrameworkPCH.h"
#include "i3UITemplate_RadioButton.h"

const char * s_pszRadioButtonInfoString[I3UI_RADIOBUTTON_SHAPE_COUNT] = 
{
	"0: 선택되지 않은 상태",
	"1: 선택된 상태",
	"2: OnMouse 상태",
	"4: Disable 상태"
};

I3_CLASS_INSTANCE( i3UITemplate_RadioButton, i3UITemplate);

i3UITemplate_RadioButton::i3UITemplate_RadioButton()
{
}

i3UITemplate_RadioButton::~i3UITemplate_RadioButton()
{
}

const char * i3UITemplate_RadioButton::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_RADIOBUTTON_SHAPE_COUNT);

	return s_pszRadioButtonInfoString[nIndex];
}

const char * i3UITemplate_RadioButton::getComment(void)
{
	return "4-state Radio Button";
}
