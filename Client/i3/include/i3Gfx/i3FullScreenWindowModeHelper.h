#pragma once


class i3RenderContext;


class i3FullScreenWindowModeHelper
{
public:
	i3FullScreenWindowModeHelper(i3RenderContext* pOwner);
	~i3FullScreenWindowModeHelper();
	
	void					Lock_WM_ACTIVATE();
	void					Unlock_WM_ACTIVATE();
	void					Process_WM_ACTIVATE(bool isActive);


	void					CreateFullScreenWindow(bool& inoutNeedToCreateDevice);
	void					ResizeAndMoveWindow();
	void					modifyMouseCursorPosition(POINT& inout_point);

private:
	i3RenderContext*		m_pRenderContext;
	HWND					m_hFrameWnd_FullScreenEmulation;
	SIZE					m_realWindowSize;					// HWND의 풀스크린윈도모드상 실제 윈도우 사이즈..(백버퍼 사이즈와 달라집니다..)

	INT32					m_origMonitorWidth;					// 가상풀스크린 직전에 구해서 반영해둔다.. (생성자수준에서 고정하면 안됨..OS해상도라도 유동적이라고 가정해야한다)
	INT32					m_origMonitorHeight;				// 가상풀스크린 직전에 구해서 반영해둔다.. (생성자수준에서 고정하면 안됨..OS해상도라도 유동적이라고 가정해야한다)
	INT32					m_origBitsPerPixel;					// 가상풀스크린 직전에 구해서 반영해둔다.. (생성자수준에서 고정하면 안됨..OS해상도라도 유동적이라고 가정해야한다)

	bool					m_bWM_ACTIVATE_Lock;				// 조건에 의해 WM_ACTIVATE 처리를 막는 플래그 변수..
};
