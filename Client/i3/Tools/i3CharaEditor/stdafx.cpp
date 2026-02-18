// stdafx.cpp : 표준 포함 파일을 포함하는 소스 파일입니다.
// i3CharaEditor.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj는 미리 컴파일된 형식 정보를 포함합니다.

#include "stdafx.h"


ChraEditor_NotifyBox_PFN	CharaEditorNotifyBox = &NotifyBox;

ChraEditor_NotifyBox_PFN SetChraEditor_NotifyBox_PFN(ChraEditor_NotifyBox_PFN newPFN)
{
	ChraEditor_NotifyBox_PFN old = CharaEditorNotifyBox;
	CharaEditorNotifyBox = newPFN;

	return old;
}