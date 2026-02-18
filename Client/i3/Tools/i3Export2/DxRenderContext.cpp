
#include "stdafx.h"
#include "resource.h"
#include "DxRenderContext.h"

#include <process.h>

const char* DxRenderContext::CLS_NAME = "i3Export_DxRenderContextDevWndCls";
const char* DxRenderContext::WND_NAME = "i3Export_DxRenderContextDevWnd";


DxRenderContext::DxRenderContext()
{
	m_renderContext = NULL;
	m_err = 0;

	m_videoInfo.m_Width			= 1;
	m_videoInfo.m_Height		= 1;
	m_videoInfo.m_bFullScreen	= FALSE;
	m_videoInfo.m_bMultiThread	= true;

	if(!CreateDevWnd(1,1))
	{
		m_err = -100;
		return;
	}

	m_renderContext = i3RenderContext::new_object();

	m_renderContext->setShaderFindFromModulePath(true);
//	m_renderContext->setShaderEnable(false);
	m_renderContext->SetLostDeviceHandler(I3_GFX_LOSTDEVICE_PROC);
	m_renderContext->SetReviveHandler(I3_GFX_REVIVE_PROC);
	m_renderContext->setDialogBoxMode(false);

	if(!m_renderContext->Create(&m_videoInfo, (void*)m_devWnd))
	{
		m_err = -200;
		return;
	} 

	m_renderContext->SetSkipRender(TRUE);

	COLOR col;
	i3Color::Set( &col, (UINT8) 50, 50, 50, 255);
	m_renderContext->SetClearColor(&col);
}

DxRenderContext::~DxRenderContext()
{
	CleanUp();
}

bool DxRenderContext::CreateDevWnd(INT32 width, INT32 height)
{
	RegisterDevWndClass();

	m_devWnd = CreateWindow(CLS_NAME, WND_NAME, WS_POPUP, 0, 0, 
		0, 0, NULL, NULL, OptiExpPlug_HInstance, (LPVOID)this);
	
	if(!m_devWnd)
		return false;

	ShowWindow(m_devWnd, SW_HIDE);

	UINT32 style = ::GetWindowLong(m_devWnd, GWL_STYLE);
	::SetWindowLong( m_devWnd, GWL_STYLE, style | WS_CLIPCHILDREN);

	return true;
}

void DxRenderContext::CleanUp()
{
	if(m_renderContext)
	{
		m_renderContext->Release();
		m_renderContext = NULL;
	}

	if(m_devWnd)
	{
		DestroyWindow(m_devWnd);
		m_devWnd = NULL;
	}

	m_err = 0;
}

void DxRenderContext::I3_GFX_LOSTDEVICE_PROC(bool bLostDevice)
{

}

void DxRenderContext::I3_GFX_REVIVE_PROC(i3RenderContext* pCtx)
{

}

bool DxRenderContext::RegisterDevWndClass()
{
	WNDCLASS wndClass;
	
	wndClass.cbClsExtra		= 0;
	wndClass.cbWndExtra		= 0;
	wndClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor		= LoadCursor(NULL,IDC_ARROW);
	wndClass.hIcon			= LoadIcon(NULL, MAKEINTRESOURCE(IDI_I3ICON));
	wndClass.hInstance		= OptiExpPlug_HInstance;
	wndClass.lpfnWndProc	= DevWndProc;
	wndClass.lpszClassName	= CLS_NAME;
	wndClass.lpszMenuName	= NULL;
	wndClass.style			= CS_HREDRAW | CS_VREDRAW;

	if(!RegisterClass(&wndClass))
		return false;

	return true;
}

LRESULT WINAPI DxRenderContext::DevWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DxRenderContext* context = (DxRenderContext*)GetWindowLongPtr(hWnd, GWL_USERDATA);

	switch(msg)
	{
	case WM_CREATE:
		{
			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
			SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)cs->lpCreateParams);
		}
		break;

	case WM_DESTROY:
		{
			SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)NULL);
		}
		break;

//	case WM_SHOWWINDOW:	GetRenderContext()->SetSkipRender((BOOL)!wParam); break;

	case WM_ERASEBKGND:
		return 1; // skip

	default:
		break;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

