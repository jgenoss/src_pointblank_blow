#include "i3GfxType.h"
#include "i3FullScreenWindowModeHelper.h"

#include "i3RenderContext.h"
#include "i3GfxGlobalVariableDX.h"

// 필요없으면 삭제할 객체이므로, 생성시점이 풀스크린 윈도우 모드가 필요한 시점임에 유의한다..

namespace
{
	bool FillPrimaryMonitorInitInfo(INT32& origMonitorWidth, INT32& origMonitorHeight, INT32& origBitsPerPixel)
	{

		bool bError = true;

	//	1. 일단은 프라이머리 모니터부터 찾을것... (원래는 서브모니터 지원에 맞춰 찾아야 하는데 ..일단은 기존에 맞춰서)

		DISPLAY_DEVICE dd;
		::memset(&dd, 0, sizeof(DISPLAY_DEVICE));
		dd.cb = sizeof(DISPLAY_DEVICE);
		
	
		for (DWORD dwDevNum = 0; ::EnumDisplayDevices(NULL, dwDevNum, &dd, 0) != FALSE; ++dwDevNum)
		{
			if (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			{
				bError = false;	break;
			}
		}
	
		if (bError)
			return false;

	//  2. 프라이머리 모니터의 레지스트리 셋팅값을 찾아봄..
		DEVMODE devMode;

		if (::EnumDisplaySettings(dd.DeviceName, ENUM_REGISTRY_SETTINGS, &devMode) == FALSE)
			return false;


		origMonitorWidth = INT32(devMode.dmPelsWidth);
		origMonitorHeight = INT32(devMode.dmPelsHeight);
		origBitsPerPixel = INT32(devMode.dmBitsPerPel);

		return true;
	}

	bool GetCurrentMonitorInitInfo(HMONITOR hCurrMonitor, INT32& origMonitorWidth, INT32& origMonitorHeight, INT32& origBitsPerPixel)
	{
		MONITORINFOEX monInfo;
		::memset(&monInfo, 0, sizeof(MONITORINFOEX));
		monInfo.cbSize = sizeof(MONITORINFOEX);

		if (::GetMonitorInfo(hCurrMonitor, &monInfo) == FALSE) // 모니터 디바이스 이름이 일단 필요해서..이걸 씀...
		{
			I3PRINTLOG(I3LOG_WARN, "can't find monitor name in FullScreenWindowMode Trial");
			return false;
		}

		DEVMODE devMode;

		if (::EnumDisplaySettings(monInfo.szDevice, ENUM_REGISTRY_SETTINGS, &devMode) == FALSE)
		{
			I3PRINTLOG(I3LOG_WARN, "can't find monitor registry settings in FullScreenWindowMode Trial");
			return false;
		}
		origMonitorWidth = INT32(devMode.dmPelsWidth);
		origMonitorHeight = INT32(devMode.dmPelsHeight);
		origBitsPerPixel = INT32(devMode.dmBitsPerPel);

		return true;
	}

	bool GetCurrentMonitorRect(HMONITOR hCurrMonitor, RECT& out_rcCurrMonitor)
	{
		out_rcCurrMonitor.left = 0;
		out_rcCurrMonitor.top = 0;
		out_rcCurrMonitor.right = 0;
		out_rcCurrMonitor.bottom = 0;

		MONITORINFOEX monInfo;
		::memset(&monInfo, 0, sizeof(MONITORINFOEX));
		monInfo.cbSize = sizeof(MONITORINFOEX);

		if (::GetMonitorInfo(hCurrMonitor, &monInfo) == FALSE) // 모니터 디바이스 이름이 일단 필요해서..이걸 씀...
		{
			I3PRINTLOG(I3LOG_WARN, "can't find monitor name in FullScreenWindowMode Trial");
			return false;
		}

		out_rcCurrMonitor = monInfo.rcMonitor;

		/*  R&D를 위해서만 남겨둠...
		DEVMODE devMode;

		if (::EnumDisplaySettings(monInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode) == FALSE)
		{
			I3PRINTLOG(I3LOG_WARN, "can't find current monitor resolution size in FullScreenWindowMode Trial");
			return false;
		}

		outCurrMonitorSize.cx = INT32(devMode.dmPelsWidth);
		outCurrMonitorSize.cy = INT32(devMode.dmPelsHeight);
		return true;
		*/
		return true;
	}


	// 그다지 정확치는 않겠지만...간편한 함수로 그냥 구한 버전..
	void FillPrimaryMonitorInitInfo_Alternative(INT32& origMonitorWidth, INT32& origMonitorHeight, INT32& origBitsPerPixel)
	{
		::ChangeDisplaySettings(NULL, 0);						// 어떤 모니터인지는 몰라도 프라이머리 모니터의 리셋처리임..(윈도우 모드로 복구됨..)

		origMonitorWidth = ::GetSystemMetrics(SM_CXSCREEN);		// 이값은 변경되서는 안된다..
		origMonitorHeight = ::GetSystemMetrics(SM_CYSCREEN);	// 이값은 변경되서는 안된다..
		HDC dc = GetDC(NULL);
		origBitsPerPixel = ::GetDeviceCaps(dc, BITSPIXEL);		// 이건 왤캐 지저분한건지..어쩔..
		ReleaseDC(NULL, dc);
	}

	// 깜장 화면 
	void DestroyFrameWindow(HWND& hFrameWnd)
	{
		if (hFrameWnd == 0)
			return;

		::DestroyWindow(hFrameWnd);
		hFrameWnd = 0;
		UnregisterClass("FullScreenWindowModeFrame", ::GetModuleHandle(NULL));
	}

	LRESULT	FullScreenFrameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		/*switch (message)
		{
		default:
			break;
		}*/

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	HWND CreateFrameWindow(  HMONITOR hCurrMonitor )
	{
		RECT monitorRect;

		if (GetCurrentMonitorRect(hCurrMonitor, monitorRect) == false)
		{
			monitorRect.left = 0;
			monitorRect.top = 0;
			monitorRect.right = GetSystemMetrics(SM_CXSCREEN);
			monitorRect.bottom = GetSystemMetrics(SM_CYSCREEN);
		}

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = (WNDPROC)FullScreenFrameWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = ::GetModuleHandle(NULL);
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);			// ??????
		wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcex.lpszMenuName = (LPCTSTR)NULL;
		wcex.lpszClassName = "FullScreenWindowModeFrame";
		wcex.hIconSm = NULL;
		RegisterClassEx(&wcex);

		// 현재 윈도우 기준으로 셋팅을 변경한다..
		HWND frameWnd_FullScreenEmulation =	CreateWindow("FullScreenWindowModeFrame", NULL, WS_POPUP, monitorRect.left, monitorRect.top, 
			monitorRect.right - monitorRect.left, monitorRect.bottom - monitorRect.top, NULL, NULL, wcex.hInstance, NULL);
		
		LONG_PTR old_exStyle = ::GetWindowLongPtr(frameWnd_FullScreenEmulation, GWL_EXSTYLE);

		::SetWindowLongPtr(frameWnd_FullScreenEmulation, GWL_EXSTYLE, old_exStyle | WS_EX_TOOLWINDOW);

		::ShowWindow(frameWnd_FullScreenEmulation, SW_SHOW);
		::UpdateWindow(frameWnd_FullScreenEmulation);
		::SetFocus(frameWnd_FullScreenEmulation);

		return frameWnd_FullScreenEmulation;
	}


	
	bool ChangeOSResoultionForFullscreenWindowMode_Imp(HMONITOR hCurrMonitor, INT32 origMonWidth, INT32 origMonHeight, INT32 origBitsPerPixel,
		INT32 iBackBufferWidth, INT32 iBackBufferHeight, INT32 desiredFreq, i3::vector<INT32>& skipModeNumber, bool& out_retryEnable)
	{
		out_retryEnable = true;

		MONITORINFOEX monInfo;
		::memset(&monInfo, 0, sizeof(MONITORINFOEX));
		monInfo.cbSize = sizeof(MONITORINFOEX);

		if (::GetMonitorInfo(hCurrMonitor, &monInfo) == FALSE) // 모니터 디바이스 이름이 일단 필요해서..이걸 씀...
		{
			I3PRINTLOG(I3LOG_WARN, "can't find monitor name in FullScreenWindowMode Trial");
			out_retryEnable = false;
			return false;
		}

	
		/*
		// 백버퍼 해상도가 아닌 모니터의 비율에 맞는 백버퍼 유사 해상도를 구합니다..이걸로 가중치를 계산한다..
		// REAL32 origMonRatioWH = REAL32(origMonWidth) / REAL32(origMonHeight);			// 주석은 참고용으로 남깁니다.

		REAL32 xRatio = (REAL32)iBackBufferWidth / (REAL32)origMonWidth;
		REAL32 yRatio = (REAL32)iBackBufferHeight / (REAL32)origMonHeight;

		INT32 targetBBWidth;	INT32 targetBBHeight;
		if (xRatio > yRatio)
		{
			targetBBHeight = iBackBufferWidth  * origMonHeight / origMonWidth;  // INT32( REAL32(scx) * (1.f / aspectRatio) ); // 주석은 참고용으로 남깁니다.
			targetBBWidth = iBackBufferWidth;
		}
		else // yRatio >= xRatio
		{
			targetBBWidth = iBackBufferHeight * origMonWidth / origMonHeight;	//	INT32(REAL32(scy) * aspectRatio));		// 주석은 참고용으로 남깁니다.
			targetBBHeight = iBackBufferHeight;
		}
		*/

		INT32 targetBBWidth = iBackBufferWidth;
		INT32 targetBBHeight = iBackBufferHeight;

		DEVMODE devMode;
		INT32 diff = 0;
		INT32 minDiff = INT_MAX;
		INT32 idxFind = -1;

		for (INT32 n = 0 ; ::EnumDisplaySettings(monInfo.szDevice, (DWORD)n, &devMode) != FALSE; ++n)			// 해당 모니터의 해상도셋팅들을 순회한다..	
		{																	// 최적의 셋팅을 일단 찾아보도록 하자..

			if (i3::vu::int_index_of(skipModeNumber, n) != -1)
			{
				continue;										// ㅇㅇ...한번 실패한것들은 그냥 스킵합니다...
			}

			diff = 0;
																						// FindAppropriateVideoInfo와 가장 큰 차이점은 모니터 해상도 비율이 비슷해야 한다는 점이다... 와이드 모니터에서 3:4 비율을 쓰면 납작해지기 때문에 피해야한다...
			if (iBackBufferWidth > (INT32)devMode.dmPelsWidth)			// 백버퍼보다 작은 모니터해상도는 가중치를 늘려준다.
			{
				diff += 1000;
			}

			if (iBackBufferHeight > (INT32)devMode.dmPelsHeight)		// 백버퍼보다 작은 모니터해상도는 가중치를 늘려준다.
			{
				diff += 1000;
			}
					
			if (INT32(devMode.dmBitsPerPel) != origBitsPerPixel)					// 픽셀당 비트수가 안맞는건 차이점이 다소 크다 판단되므로 가중치를 가장 높인다..
			{
				diff += 10000;														// 일단 안걸리기를 빌자..
			}

			diff += abs(targetBBWidth - (INT32)devMode.dmPelsWidth);
			diff += abs(targetBBHeight - (INT32)devMode.dmPelsHeight);

			diff += abs(desiredFreq - (INT32)devMode.dmDisplayFrequency);					//

																							// WM_DISPLAYCHANGE
			if (diff < minDiff)
			{
				minDiff = diff;
				idxFind = n;
			}
		}

		if (idxFind == -1)
		{
			I3PRINTLOG(I3LOG_WARN, "can't find appropriate resolution in FullScreenWindowMode Trial");
			out_retryEnable = false;
			return false;
		}

		::EnumDisplaySettings(monInfo.szDevice, idxFind, &devMode);

		// 이걸로 디스플레이 셋팅을 겁니다... OS해상도가 변경되므로, 최소한 어플리케이션 종료전에는 복귀루틴 처리해야합니다.

		devMode.dmFields |= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		// 반드시 Ex 함수여야 한다... 구버전 함수는 멀티모니터 지원 설정이 안된다...
		INT32 res = ::ChangeDisplaySettingsEx(monInfo.szDevice, &devMode, NULL, CDS_FULLSCREEN, NULL);

		if (res != DISP_CHANGE_SUCCESSFUL)
		{
			I3PRINTLOG(I3LOG_WARN, 
				"can't change OS Resolution in FullScreenWindowMode Trial. Res = %dX%d, Freq = %dHz, ModeNum = %d, ChangeDisplaySettings Error Return = %d", 
				devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmDisplayFrequency, idxFind, res);
			
			skipModeNumber.push_back(idxFind);

			return false;
		}

		I3PRINTLOG(I3LOG_NOTICE, "OS Resolution Changed for FullScreen WindowMode : Res = %dX%d, Freq = %dHz, ModeNum = %d, MonitorName = %s",
			devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmDisplayFrequency, idxFind, monInfo.szDevice);

		out_retryEnable = false;

		return true;
	}

	// OS 해상도 변경에 처음 실패하면, 그다음번 차선 OS 셋팅으로 계속 넘깁니당..

	bool ChangeOSResoultionForFullscreenWindowMode(HMONITOR hCurrMonitor, INT32 origMonWidth, INT32 origMonHeight, INT32 origBitsPerPixel,
		INT32 iBackBufferWidth, INT32 iBackBufferHeight, INT32 desiredFreq)
	{
		i3::vector<INT32> vec_skipModeNumber;
		bool retryEnable = true;

		bool res = ChangeOSResoultionForFullscreenWindowMode_Imp(hCurrMonitor, origMonWidth, origMonHeight, origBitsPerPixel,
			iBackBufferWidth, iBackBufferHeight, desiredFreq, vec_skipModeNumber, retryEnable);

		if (res)
		{
			return true;
		}

		while (retryEnable)
		{
			res = ChangeOSResoultionForFullscreenWindowMode_Imp(hCurrMonitor, origMonWidth, origMonHeight, origBitsPerPixel,
				iBackBufferWidth, iBackBufferHeight, desiredFreq, vec_skipModeNumber, retryEnable);

			if (res)
			{
				return true;
			}
		}

		return false;
	}


	HMONITOR GetPrimaryMonitorHandle()
	{
		const POINT ptZero = { 0, 0 };
		return MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
	}

	void RestoreOSResoultionFromFullscreenWindowMode()
	{
		::ChangeDisplaySettings(NULL, 0);
		I3PRINTLOG(I3LOG_NOTICE, "OS Resolution Restored to Initial OS state.");
	}

}


i3FullScreenWindowModeHelper::i3FullScreenWindowModeHelper(i3RenderContext* pOwner) : m_pRenderContext(pOwner), m_hFrameWnd_FullScreenEmulation(NULL),
m_origMonitorWidth(0), m_origMonitorHeight(0), m_origBitsPerPixel(0), m_bWM_ACTIVATE_Lock(false)
{
	m_realWindowSize.cx = 0;
	m_realWindowSize.cy = 0;
	
}

i3FullScreenWindowModeHelper::~i3FullScreenWindowModeHelper()
{
	if (m_hFrameWnd_FullScreenEmulation)
	{
		DestroyFrameWindow(m_hFrameWnd_FullScreenEmulation);
		RestoreOSResoultionFromFullscreenWindowMode();
	}

}

void	i3FullScreenWindowModeHelper::Lock_WM_ACTIVATE()
{
	m_bWM_ACTIVATE_Lock = true;
}

void	i3FullScreenWindowModeHelper::Unlock_WM_ACTIVATE()
{
	m_bWM_ACTIVATE_Lock = false;
}

static bool s_tempBuildingFullScreenWnd = false;			// 임시 변수..코드정리시 다르게 처리해야한다..

void	i3FullScreenWindowModeHelper::Process_WM_ACTIVATE(bool isActive)
{
	i3VideoInfo* pCurrVideoInfo = m_pRenderContext->GetVideoInfo();

	if (pCurrVideoInfo->m_bFullScreen == false)
		return;

	if (pCurrVideoInfo->m_bFullScreenWindowModeEnable == false)
		return;

	if (m_bWM_ACTIVATE_Lock == true)				// ChangeVideoInfo 중 일어나는 메세지변경을 막기 위한 플래그....
		return;

	HWND hWnd = m_pRenderContext->getHWND();

	if (isActive && ::IsIconic(hWnd) ) // 윈도우 창 최소화의 경우..
		return;

	m_bWM_ACTIVATE_Lock = true;

	if (isActive)
	{
		if (m_hFrameWnd_FullScreenEmulation == 0)
		{
			HMONITOR hMon = m_pRenderContext->GetCurrentMonitorHandle();

			if (GetCurrentMonitorInitInfo(hMon, m_origMonitorWidth, m_origMonitorHeight, m_origBitsPerPixel) == false)
			{
				I3PRINTLOG(I3LOG_WARN, "cant' find current monitor registry settings.");
				FillPrimaryMonitorInitInfo_Alternative(m_origMonitorWidth, m_origMonitorHeight, m_origBitsPerPixel);
				hMon = GetPrimaryMonitorHandle();
			}

			ChangeOSResoultionForFullscreenWindowMode(hMon, m_origMonitorWidth, m_origMonitorHeight, m_origBitsPerPixel,
				pCurrVideoInfo->m_Width, pCurrVideoInfo->m_Height, (INT32)pCurrVideoInfo->m_RefreshRate);

			HWND hFrameWnd_FullScreenEmulation = CreateFrameWindow(hMon);

			::SetFocus(m_pRenderContext->getHWND());

			m_hFrameWnd_FullScreenEmulation = hFrameWnd_FullScreenEmulation;

			INT32   currAdapterIndex = m_pRenderContext->FindAdapterIndex(hMon);

			pCurrVideoInfo->m_AdapterIndex = currAdapterIndex;
			g_iCurrentAdapterIndex = currAdapterIndex;		// 그냥 여기서 변경..
		}
		
		this->ResizeAndMoveWindow();

			/*
			m_CurHMonitor = hPrimaryMon;
			*/
		
	}
	else
	{
		if (m_hFrameWnd_FullScreenEmulation)
		{
			DestroyFrameWindow(m_hFrameWnd_FullScreenEmulation);
			SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			ShowWindow(hWnd, SW_MINIMIZE);
			RestoreOSResoultionFromFullscreenWindowMode();
		}
	}

	m_bWM_ACTIVATE_Lock = false;
	
}

void	i3FullScreenWindowModeHelper::CreateFullScreenWindow(bool& inoutNeedToCreateDevice)
{
	HMONITOR hMon = m_pRenderContext->GetCurrentMonitorHandle();

	if (GetCurrentMonitorInitInfo(hMon, m_origMonitorWidth, m_origMonitorHeight, m_origBitsPerPixel) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "cant' find current monitor registry settings.");
		FillPrimaryMonitorInitInfo_Alternative(m_origMonitorWidth, m_origMonitorHeight, m_origBitsPerPixel);
		hMon = GetPrimaryMonitorHandle();
	}

	/*
	if (!FillPrimaryMonitorInitInfo(m_origMonitorWidth, m_origMonitorHeight, m_origBitsPerPixel))			// 유동적일수 있으므로 최신값 사용한다..
	{
		I3PRINTLOG(I3LOG_WARN, "cant' find primary monitor or monitor registry settings.");
		FillPrimaryMonitorInitInfo_Alternative(m_origMonitorWidth, m_origMonitorHeight, m_origBitsPerPixel);
	}
	*/

	DestroyFrameWindow(m_hFrameWnd_FullScreenEmulation);

	// OS 해상도 변경..
//	HMONITOR hPrimaryMon = GetPrimaryMonitorHandle();				// 일단은 현재 PB기존에 맞춰서 프라이머리에만 적용함..

	if (m_pRenderContext->GetCurrentMonitorHandle() != hMon)
		inoutNeedToCreateDevice = true;								// 디바이스 재생성이 없으면 어플 종료시 크래시가 나는 DX버그가 있습니다..

	// m_CurHMonitor
	i3VideoInfo* pVideoInfo = m_pRenderContext->GetVideoInfo();

	ChangeOSResoultionForFullscreenWindowMode(hMon, m_origMonitorWidth, m_origMonitorHeight, m_origBitsPerPixel,
		pVideoInfo->m_Width, pVideoInfo->m_Height, (INT32)pVideoInfo->m_RefreshRate);

	// 프레임 윈도우 생성..

	HWND hFrameWnd_FullScreenEmulation = CreateFrameWindow(hMon);

	::SetFocus( m_pRenderContext->getHWND() );
	
	m_hFrameWnd_FullScreenEmulation = hFrameWnd_FullScreenEmulation;

	INT32    primAdapterIndex = m_pRenderContext->FindAdapterIndex(hMon);

	pVideoInfo->m_AdapterIndex = primAdapterIndex;
	g_iCurrentAdapterIndex = primAdapterIndex;		// 그냥 여기서 변경..


}

void	i3FullScreenWindowModeHelper::ResizeAndMoveWindow()
{
	if (m_hFrameWnd_FullScreenEmulation == NULL)
		return;

	RECT monitorRect;
	SIZE monitorSize;

	// HMONITOR hPrimaryMon = GetPrimaryMonitorHandle();				// 일단은 현재 PB기존에 맞춰서 프라이머리에만 적용함..

	HMONITOR hCurrMon =	m_pRenderContext->GetCurrentMonitorHandle();

	if (GetCurrentMonitorRect(hCurrMon, monitorRect) == false)
	{
		monitorRect.left = 0;
		monitorRect.top = 0;
		monitorRect.right = GetSystemMetrics(SM_CXSCREEN);
		monitorRect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}

	monitorSize.cx = monitorRect.right - monitorRect.left;
	monitorSize.cy = monitorRect.bottom - monitorRect.top;

	i3VideoInfo* pVideoInfo = m_pRenderContext->GetVideoInfo();

	INT32 cx = pVideoInfo->m_Width;
	INT32 cy = pVideoInfo->m_Height;
		
	REAL32 xRatio = (REAL32)cx / (REAL32)monitorSize.cx;
	REAL32 yRatio = (REAL32)cy / (REAL32)monitorSize.cy;
	//	REAL32 aspectRatio = (REAL32)cx / (REAL32)cy;										// 주석은 참고용으로 남깁니다.

	if (xRatio > yRatio)
	{
		cy = monitorSize.cx  * cy / cx;              // INT32( REAL32(scx) * (1.f / aspectRatio) ); // 주석은 참고용으로 남깁니다.
		cx = monitorSize.cx;
	}
	else // yRatio >= xRatio
	{
		cx = monitorSize.cy * cx / cy;				//	INT32(REAL32(scy) * aspectRatio));		// 주석은 참고용으로 남깁니다.
		cy = monitorSize.cy;
	}

	INT32 x = max(0, (monitorSize.cx >> 1) - (cx >> 1));
	INT32 y = max(0, (monitorSize.cy >> 1) - (cy >> 1));

	x += monitorRect.left;
	y += monitorRect.top;

	m_realWindowSize.cx = cx;
	m_realWindowSize.cy = cy;

	::SetWindowPos(m_hFrameWnd_FullScreenEmulation, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
	::SetWindowPos(m_pRenderContext->getHWND(), HWND_TOPMOST, x, y, cx, cy, SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void	i3FullScreenWindowModeHelper::modifyMouseCursorPosition(POINT& inout_point)
{
	if (m_realWindowSize.cx == 0 || m_realWindowSize.cy == 0)			// 에러상태인데..일단 스킵정도로만 넘깁니당..
		return;

	i3VideoInfo* pCurrInfo = m_pRenderContext->GetVideoInfo();

	inout_point.x = inout_point.x * pCurrInfo->m_Width / m_realWindowSize.cx;			// 정수연산이며 나누기가 포함되므로 누적연산자를 쓰면 안됩니다..
	inout_point.y = inout_point.y * pCurrInfo->m_Height / m_realWindowSize.cy;
	
}
