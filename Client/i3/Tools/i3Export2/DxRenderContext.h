
#ifndef _DXRENDERCONTEXT_H_
#define _DXRENDERCONTEXT_H_


class DxRenderContext
{
public:
	static const char* CLS_NAME; 
	static const char* WND_NAME; 

	DxRenderContext();
	~DxRenderContext();

	i3RenderContext* GetContext() { return m_renderContext; }
	INT32 GetErr() { return m_err; }

protected:
	bool CreateDevWnd(INT32 width, INT32 height);
	void CleanUp();

	bool RegisterDevWndClass();
	static LRESULT WINAPI DevWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	INT32						m_err;

	HWND						m_devWnd;
	WNDPROC						m_oldDevWndProc;
	
	i3VideoInfo					m_videoInfo;
	i3RenderContext*			m_renderContext;

	static void I3_GFX_LOSTDEVICE_PROC(bool bLostDevice);
	static void I3_GFX_REVIVE_PROC(i3RenderContext * pCtx);
};

#endif
