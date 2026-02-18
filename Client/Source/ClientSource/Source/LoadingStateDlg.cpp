#include "pch.h"
#include "GlobalFunc.h"
#include "LoadingStateDlg.h"
#include "atlimage.h"
#include "../i3Viewer/resource.h"


#include <fstream>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


namespace
{
	HWND g_Hwnd = 0;
	HANDLE g_hThread = 0;
	HBITMAP g_hBitmap = nullptr;
	bool	s_Continue = true;
}

namespace
{
	// 아무래도 BMP파일만 될거 같당...
	HBITMAP LoadBitmapFromMemory(void* pBuff, size_t buffSize)
	{
		tagBITMAPFILEHEADER* bfh = (tagBITMAPFILEHEADER*)pBuff;
		tagBITMAPINFOHEADER* bih = (tagBITMAPINFOHEADER*)((unsigned char*)pBuff + sizeof(tagBITMAPFILEHEADER));
		RGBQUAD*             rgb = (RGBQUAD*)((unsigned char*)pBuff + sizeof(tagBITMAPFILEHEADER) + sizeof(tagBITMAPINFOHEADER));

		BITMAPINFO bi;
		bi.bmiColors[0] = *rgb;
		bi.bmiHeader = *bih;

		char* pPixels = ((char*)pBuff + bfh->bfOffBits);
		char* ppvBits;

		HBITMAP	hBitmap = ::CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&ppvBits, NULL, 0);
		SetDIBits(NULL, hBitmap, 0, bih->biHeight, pPixels, &bi, DIB_RGB_COLORS);

		return hBitmap;
	}
	
}

namespace
{
	static void InitLoadingScreen()
	{
		::srand((unsigned int)::time(NULL));
		i3::vector<LPCTSTR> vec_strPath;						// 병맛같은 8배열 걷어내고 vector로 변경..

																//	TiXmlDocument doc("Gui/Loading/Unpackloading.xml");		// 이 함수에 리소스가 .... 
																//	bool loadDoc = doc.LoadFile();

		TiXmlDocument doc;					// 걍 비워도 될거심..

		{
			i3FileStream xmlStream;
			BOOL openOK = FALSE;

#if defined(LOCALE_RUSSIA)
			// 디버깅에서 브레이크 팝업이 계속 떠서 Packing 사용하는 국가만 예외처리 하는 걸로...
			openOK = xmlStream.Open("Gui/Loading/Unpackloading.packXML", STREAM_READ | STREAM_SHAREREAD);
#endif

			if (openOK == FALSE)
			{
				openOK = xmlStream.Open("Gui/Loading/Unpackloading.xml", STREAM_READ | STREAM_SHAREREAD);
			}

			if (openOK == FALSE)
			{
				I3PRINTLOG(I3LOG_FATAL, "initial BarImage Xml File Loading Failed..");
			}
			else
			{
				bool loadDoc = doc.LoadFile(&xmlStream);		// 일단 편리하니까..쓰긴 했지만..써드파티 모듈은 웬만하면 건드리지 말라고..
				I3ASSERT(loadDoc);			// -->  Assert 안에 실행파일 넣지 말라고 해도 말안듣는 사람은 퇴사했다....
			}
			xmlStream.Close();
		}

		TiXmlElement* pAnnounce = doc.FirstChildElement("ANNOUNCE");
		I3ASSERT(pAnnounce != NULL);

		TiXmlElement* pImage = pAnnounce->FirstChildElement("IMAGE");

		while (pImage != NULL)
		{
			if (NULL != pImage->Attribute("path"))
			{
				LPCTSTR szPath = pImage->Attribute("path");
				vec_strPath.push_back(szPath);					// XML데이터가 살아있는 동안만 유효하다.
			}
			pImage = pImage->NextSiblingElement("IMAGE");			// 쩐내나게 오래된 TiXml 문법..
		}

		if (!vec_strPath.empty())
		{
			INT idx = i3Math::Rand() % INT32(vec_strPath.size()); // 랜덤하게 하나 찝어내는 모냥..(구려)
																  // LoadImage함수가 ㅈ 같으니까....버퍼를 받아들이는 함수로 변경해야한다..

			i3FileStream stream;			// 이제 이걸 써야만 합니다...

			BOOL openOK = stream.Open(vec_strPath[idx], STREAM_READ | STREAM_SHAREREAD);

			if (openOK == FALSE)
			{
				I3PRINTLOG(I3LOG_FATAL, "initial Bar Loading Failed..");
			}
			else
			{
				UINT32 fileSize = stream.GetSize();
				char* buff = new char[fileSize];			// 에잉..Read함수의존적이라..일케밖에 안되...
				stream.Read(buff, fileSize);
				stream.Close();
				g_hBitmap = LoadBitmapFromMemory(buff, fileSize);
				I3ASSERT(g_hBitmap != NULL);

				delete[] buff;
			}

			//g_hBitmap = (HBITMAP)LoadImage(g_hInstance, vec_strPath[idx], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			//I3ASSERT(g_hBitmap != NULL);
		}
		else
		{
			g_hBitmap = NULL;
			I3ASSERT_0;
		}

	}

}
// 쓰레드 안에 윈도우가 있고, 그 윈도우 프로시저에서 이미지 로딩...

LRESULT  CALLBACK PreLoadWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{	
	switch (iMsg)
	{
	case WM_CREATE:
		{
		
		
		}
		break;
	case WM_CLOSE:
		{
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT 	ps;
			HDC 			hdc;
			BITMAP 			bitmap;
			HDC 			hdcMem;
			HGDIOBJ 		oldBitmap;
			
			hdc = BeginPaint(g_Hwnd, &ps);
			int hdcID = ::SaveDC( hdc);

			hdcMem = CreateCompatibleDC(hdc);
			oldBitmap = SelectObject(hdcMem, g_hBitmap);

			GetObject(g_hBitmap, sizeof(bitmap), &bitmap);
			BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, oldBitmap);
			::RestoreDC( hdc, hdcID);
			//DeleteDC(nullptr);
			EndPaint(g_Hwnd, &ps);

			return 0;
		}
		break;
	case WM_DESTROY:
		{
			if (g_hBitmap)
				DeleteObject(g_hBitmap);
			s_Continue = false;
		}
	}

	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

DWORD WINAPI ThreadProc( LPVOID lpParam ) 
{ 
	//DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_LOADING_DLG), NULL, DlgProc);

	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass = {};
	LPCTSTR lpszClass= TEXT("PB");

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
	WndClass.hInstance = ::GetModuleHandle(NULL);
	WndClass.lpfnWndProc = PreLoadWndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = nullptr;

	RegisterClass(&WndClass);

	//WS_SYSMENU|WS_BORDER
	hWnd = CreateWindowEx(WS_OVERLAPPED,lpszClass,0,0,9,9,99,99,0,0,0,0);
	SetWindowLongW(hWnd, GWL_STYLE, 0);

	BITMAP 			bitmap;
	GetObject(g_hBitmap, sizeof(bitmap), &bitmap);

	int x = (GetSystemMetrics(SM_CXFULLSCREEN) / 2) - (bitmap.bmWidth/ 2);
	int y = (GetSystemMetrics(SM_CYFULLSCREEN) / 2) - (bitmap.bmHeight / 2);

	::MoveWindow(hWnd, x, y, bitmap.bmWidth, bitmap.bmHeight, FALSE);
	ShowWindow(hWnd, SW_SHOW);
	g_Hwnd = hWnd;

	while(GetMessage(&Message, g_Hwnd, 0, 0) && s_Continue)
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	CloseHandle( g_hThread);

	return 0; 
} 

void LoadingState::CreatePreLoadingScreen()
{
	InitLoadingScreen();

	DWORD dwThreadID = 0; 
	g_hThread = ::CreateThread( NULL, 0, ThreadProc, NULL, 0, &dwThreadID);
}

void LoadingState::DestroyPreLoadingScreen()
{
	::ShowWindow(g_Hwnd, SW_HIDE);
	PostMessage(g_Hwnd, WM_CLOSE, 0, 0);
}

void LoadingState::ShowPreLoadingScreen()
{
	::ShowWindow(g_Hwnd, SW_SHOW);
}

void LoadingState::HidePreLoadingScreen()
{
	::ShowWindow(g_Hwnd, SW_HIDE);
}
