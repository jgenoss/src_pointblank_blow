#pragma once

namespace WinMagnet
{
	/*	윈도우 창 자석 효과 함수(http://paulownia.egloos.com/4443241)
	*/
	void CreateProc(INT32 offset = 60); //자석 효과 초기화 함수 offset 만큼 가까워지면 윈도우는 붙는다. WM_CREATE 윈도우 메세지에서 호출하면 된다.
	void WindowPosChangingProc(LPARAM lParam); //WM_WINDOWPOSCHANGING 윈도우 메세지에서 호출하면 된다.
	void SetOffset(INT32 offset);
}