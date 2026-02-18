#include "stdafx.h"
#include "i3MFCWorkAround.h"
#include "i3Base/string/ext/generic_string_copy.h"

i3MFCWorkAround::i3MFCWorkAround()
{
	LOGFONT lf;
	afxGlobalData.fontVert.GetLogFont(&lf);

	m_fontTreeCtrl.CreateFont(
	-12, /*height*/
	 0, /*width*/
	 0, /*escapment angle 0.1 degrees - 0=normal horizontal, angle
	 winds counter clockwise*/
	 0, /*orientation angle 0.1 degrees*/
	 FW_NORMAL, /*font weight*/
	 FALSE, /*not italicized*/
	 FALSE, /*not underlined*/
	 FALSE, /*not strikeout*/
	 DEFAULT_CHARSET, /*charset*/
	 OUT_DEFAULT_PRECIS, /*output precission*/
	 CLIP_DEFAULT_PRECIS, /*clip precission*/
	 NONANTIALIASED_QUALITY, /*output quality*/
	 VARIABLE_PITCH | FF_DONTCARE, /*pitch and family*/
	 L"¸¼Àº °íµñ"); /*font face*/
	
}

i3MFCWorkAround::~i3MFCWorkAround()
{

}

