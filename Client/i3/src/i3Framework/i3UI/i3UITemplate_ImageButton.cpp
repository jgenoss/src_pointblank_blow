#include "i3FrameworkPCH.h"
#include "i3UITemplate_ImageButton.h"

static const char * s_pszButtonInfoString[I3UI_IMAGEBUTTON_SHAPE_COUNT] = 
{
	"0: 버튼 바탕 : 선택되지 않은 상태",
	"1: 버튼 바탕 : 선택된 상태",
	"2: 버튼 바탕 : OnMouse 상태",
	"4: 버튼 바탕 : Disable 상태",
	"5: 이미지 : 선택되지 않은 상태",
	"6: 이미지 : 선택된 상태",
	"7: 이미지 : OnMouse 상태",
	"8: 이미지 : Disable 상태"
};

I3_CLASS_INSTANCE( i3UITemplate_ImageButton, i3UITemplate);

i3UITemplate_ImageButton::i3UITemplate_ImageButton()
{
}

i3UITemplate_ImageButton::~i3UITemplate_ImageButton()
{
}

const char * i3UITemplate_ImageButton::getShapeInfoString(INT32 nIndex)
{
	I3_BOUNDCHK( nIndex, I3UI_IMAGEBUTTON_SHAPE_COUNT);

	return s_pszButtonInfoString[nIndex];
}

const char * i3UITemplate_ImageButton::getComment(void)
{
	return "4-state Image Button";
}
